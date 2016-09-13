/* 
**  (C) 2016 by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                     ___   __
**                  __/  /_ /  )
**          ___  __(_   ___)  /
**         /  / /  )/  /  /  /  Minimalist
**        /  /_/  //  (__/  /  C utility 
**       (____,__/(_____(__/  Library
** 
*/

#include "utl.h"

//<<<//
#ifndef UTL_NOPMX
#ifdef UTL_MAIN
#if 0
static int(*utl_pmx_ext)(char *r, char *t) = NULL;
#endif

char     *utl_pmx_capt[utl_pmx_MAXCAPT][2] = {{0}} ;
uint8_t   utl_pmx_capnum                   =   0   ;
char     *utl_pmx_error                    = NULL  ;


#define utl_pmx_set_paterror(t) do {if (utl_pmx_error == NULL) {utl_pmx_error = t;}} while (0)

static int utl_pmx_utf8 = 0;

#define utl_pmx_FAIL       goto fail

#define utl_pmx_newcap(t) do {                                       \
                            if (utl_pmx_capnum < utl_pmx_MAXCAPT) {  \
                              utl_pmx_capt[utl_pmx_capnum][0] =      \
                              utl_pmx_capt[utl_pmx_capnum][1] = (t); \
                              utl_pmx_capnum++;                      \
                            }                                        \
                          } while(0)

typedef struct {
  char *pat;
  char *txt;
  int32_t min_n;
  int32_t max_n;
  int32_t n;
  int16_t inv;
  int16_t cap;
} utl_pmx_state_s;

utl_pmx_state_s utl_pmx_stack[utl_pmx_MAXCAPT];
uint8_t utl_pmx_stack_cnt = 0;

static void utl_pmx_state_reset()
{
  utl_pmx_stack_cnt = 0;
  utl_pmx_capnum = 0;
}

static int utl_pmx_state_push(char *pat, char *txt, int32_t min_n, int32_t max_n, int16_t inv)
{
  utl_pmx_state_s *state;
  
  if (utl_pmx_stack_cnt >= utl_pmx_MAXCAPT) return 0;
  
  state = utl_pmx_stack + utl_pmx_stack_cnt;
  
  state->pat   = pat;
  state->txt   = txt;
  state->min_n = min_n;
  state->max_n = max_n;
  state->n     = 0;
  state->inv   = inv;
  state->cap   = utl_pmx_capnum;
  
  utl_pmx_newcap(txt);
  utl_pmx_stack_cnt++;
  
  return 1;
}

static int utl_pmx_state_pop()
{
  if (utl_pmx_stack_cnt == 0) return 0;
  utl_pmx_stack_cnt--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top()
{
  if (utl_pmx_stack_cnt == 0) return NULL;
  return utl_pmx_stack + (utl_pmx_stack_cnt-1);
}

size_t utl_pmx_len(uint8_t n) {return pmxend(n)-pmxstart(n);}

static int utl_pmx_get_utf8(char*t, int32_t *ch)
{
  int len = 0;
  int32_t val;
  val=0;
  
       if ((*t & 0x80) == 0x00) { val =  *t        ; len = 1; }
  else if ((*t & 0xE0) == 0xC0) { val = (*t & 0x1F); len = 2; }
  else if ((*t & 0xF0) == 0xE0) { val = (*t & 0x0F); len = 3; }
  else if ((*t & 0xF8) == 0xF0) { val = (*t & 0x07); len = 4; }
  
  switch (len) {  /* WARNING: falls through! */
    case 4: if ((*++t & 0xC0) != 0x80) {len=0; break;}
            val = (val << 6) | (*t & 0x3F);
    case 3: if ((*++t & 0xC0) != 0x80) {len=0; break;}
            val = (val << 6) | (*t & 0x3F);
    case 2: if ((*++t & 0xC0) != 0x80) {len=0; break;}
            val = (val << 6) | (*t & 0x3F);
  }
  if (len && ch) *ch = val;
  return len;
}

// Returns the length in bytes of the character or 0 if it is '\0'
static int32_t utl_pmx_nextch(char *t, int32_t *c_ptr)
{
  int32_t len = 0;
  
  if (utl_pmx_utf8) len = utl_pmx_get_utf8(t, c_ptr);
  else if ((*c_ptr = (uint8_t)(*t))) len = 1;
  
  return len;
}

static int32_t utl_pmx_gethex(char *pat, int32_t *c_ptr)
{
  int32_t ch =0;
  int32_t len =0;
  
  while (*pat && (*pat != '>') && !isxdigit(*pat)) {len++; pat++;}
  while (*pat) {
         if ('0'<= *pat && *pat <= '9') ch = (ch << 4) + (*pat -'0');
    else if ('A'<= *pat && *pat <= 'F') ch = (ch << 4) + (*pat -'A'+10);
    else if ('a'<= *pat && *pat <= 'f') ch = (ch << 4) + (*pat -'a'+10);
    else break;
    len++; pat++;
  }
  *c_ptr = ch;
  return len;  
}

static int utl_pmx_isin(char *pat, char *pat_end, int32_t ch, int32_t (*nxt)(char *, int32_t *))
{
  int32_t c1,c2;
  int32_t len;
  
  while (pat<pat_end) {
    c2=0; c1=0;
    len = nxt(pat, &c1);
    if (ch == c1) return 1;
    pat +=len; 
    if (*pat == '-') {
      len = nxt(++pat, &c2);
      if (c1 <= ch && ch <= c2) return 1;
      pat += len; 
    }
  }
  return 0;
}

#define utl_pmx_isin_chars(p,e,c) utl_pmx_isin(p,e,c,utl_pmx_nextch)
#define utl_pmx_isin_codes(p,e,c) utl_pmx_isin(p,e,c,utl_pmx_gethex)

static int32_t utl_pmx_iscapt(char *pat, char *txt)
{
  int32_t len = 0;
  uint8_t capnum = 0; 
  char *cap;
  
  if ('1' <= *pat && *pat <= '9') {
    capnum = *pat - '0';
    if (capnum < utl_pmx_capnum) {
      cap = pmxstart(capnum);
      while (cap < pmxend(capnum) && *cap && (*cap == *txt)) {
        len++; txt++; cap++;
      }
      if (cap < pmxend(capnum)) len = 0;
    }
  }
  return len;
}

#if 0
static int utl_pmx_quoted(char *r, char *t, char *s)
{
  int ret = 0;
  int n;
  
  n=s-r;
  int32_t c_esc='\\';
  int32_t c_beg='"';
  int32_t c_end='"';
  int32_t ch;
  
  ch= *t;
  if (ch == '\0') return 0;
  _logdebug("QUOTED: [%s] [%s] [%s] (%d)",r,t,s,s-r);
  n=s-r;
  if (n <= 1) { /* just <q> or <q\\> */
         if (ch == '"')  {c_esc='\\';  c_beg=ch;  c_end='"';}
    else if (ch == '\'') {c_esc='\\';  c_beg=ch;  c_end='\'';}
    else if (ch == '(')  {c_esc='\0';  c_beg=ch;  c_end=')';}
    else if (ch == '[')  {c_esc='\0';  c_beg=ch;  c_end=']';}
    else if (ch == '{')  {c_esc='\0';  c_beg=ch;  c_end='}';}
    else if (ch == '<')  {c_esc='\0';  c_beg=ch;  c_end='>';}
  }
  if (n == 1) { /* escape character provided */
    c_esc = r[0];
  }
  else if (n == 2) { /* start/end but no escape character */
    c_esc = '\0'; c_beg = r[0]; c_end = r[1];
  }
  else if (n >= 3) { /* escape/start/end */
    c_esc = r[0]; c_beg = r[1]; c_end = r[2];    
  }
  _logdebug("QUOTED: '%ch' '%c' '%c'",c_esc?c_esc:' ',c_beg,c_end);
  if (*t == c_beg) {
    for (t++, n=0, ret=2; *t;  t++, ret++) {
      if (*t == c_esc && t[1] != '\0') {
        t++;
        ret++;
      }
      else if (*t == c_end) {
        if (n > 0) n--;
        else break;
      }
      else if (*t == c_beg) n++;
    }
    if (*t == '\0') ret = 0;
  }
  return ret;
}
#endif 

static int utl_pmx_class(char **pat_ptr, char **txt_ptr)
{
  int inv = 0;
  
  char *pat = *pat_ptr;
  char *txt = *txt_ptr;
  char *pat_end;
  
  int32_t len = 0;
  int32_t n = 0;
  int32_t min_n=0;
  int32_t max_n=0;
  int32_t ch;
  
  _logdebug("class:[%s][%s]",pat,txt);
                
  pat++;  /* skip the '<' */
  
  // {{ Find the end of the pattern
  pat_end=pat;
  while (*pat_end && *pat_end != '>') pat_end++;
  if (pat_end[1] == '>') pat_end++; /* allow just one '>' at the end of a pattern */
  //}}
  
  // {{ Get how many times the match has to occur.
  //      Examples: <2-4l> matches 2,3 or 4 lower case letters
  //                <-4d>  matches from 0 to 4 decimal digits
  //                <2-s>  matches at 2 spaces or more
  //                <3u>   matches exactly 3 upper case letters
  while ('0' <= *pat && *pat <= '9') 
    min_n = (min_n*10) + (*pat++ - '0');
  
  if (*pat == '-') {
    pat++;
    while ('0'<=*pat && *pat <= '9') 
      max_n = (max_n*10) + (*pat++ - '0'); 
    
    if (max_n == 0) max_n = INT32_MAX;
  }
  if (max_n < min_n) max_n = min_n;
  
  if (max_n == 0) {
    switch (*pat) {
      case '*' : min_n = 0; max_n = INT32_MAX; pat++; break;
      case '+' : min_n = 1; max_n = INT32_MAX; pat++; break;
      case '?' : min_n = 0; max_n = 1;         pat++; break;
      default  : min_n = 1; max_n = 1;                break;
    }
  }
  // }}
  
  if (*pat == '!') {inv = 1; pat++;}
  
  // {{ This is for handling repetition of patterns between parenthesis:
  //    Example: '<*>(\\'|<!='>)'   <--  single quoted string (\' as escaped)
  //              <3>(xa|pt)        <--  "ptptpt" or "ptxaxa" or "xaxapt" or ...
  if (pat[0] == '>' && pat[1] == '(') {
    pat += 2;
    if (!utl_pmx_state_push(pat,txt,min_n,max_n,inv)) 
      utl_pmx_set_paterror(pat);
    *pat_ptr = pat;
    return 1;
  }
  // }}
  
  // {{ Matches a pattern n times
  #define utl_W(tst) while ((len = utl_pmx_nextch(txt,&ch)) && ((!tst) == inv) && (n<max_n)) {n++; txt+=len;}
  switch (*pat) {
    case 'a' : utl_W(isalpha(ch))               ; break;
    case 's' : utl_W(isspace(ch))               ; break;
    case 'u' : utl_W(isupper(ch))               ; break;
    case 'l' : utl_W(islower(ch))               ; break;
    case 'd' : utl_W(isdigit(ch))               ; break;
    case 'x' : utl_W(isxdigit(ch))              ; break;
    case 'w' : utl_W(isalnum(ch))               ; break;
    case 'c' : utl_W(iscntrl(ch))               ; break;
    case 'g' : utl_W(isgraph(ch))               ; break;
    case 'i' : utl_W((ch < 0x80))               ; break;
    case 'k' : utl_W((ch == ' '  || ch =='\t')) ; break;
    case 'n' : utl_W((ch == '\r' || ch =='\n')) ; break;
    case 'p' : utl_W(ispunct(ch))               ; break;
    case 'q' : utl_W(isalnum(ch))               ; break;
    case 'r' : utl_W(isprint(ch))               ; break;
    
    case '.' : utl_W(ch)                        ; break;

    case '=' : utl_W(utl_pmx_isin_chars(pat+1,pat_end,ch)); break;
    case '#' : utl_W(utl_pmx_isin_codes(pat+1,pat_end,ch)); break;
    
    case 'N' : utl_W((txt[0]=='\r'
                            ? (txt[1] == '\n'? (len++) : 1)
                            : (txt[0] == '\n'?  1 : 0)) ) ; break;
    
    case '$' : if (*txt == '\0') n=min_n; break;
    
    case '>' : utl_pmx_set_paterror(pat); return 0;
  
    case '^' : if (inv) utl_pmx_set_paterror(pat); inv = 0;
               utl_W((len=utl_pmx_iscapt(pat+1,txt)));
               break;
  
    default  : utl_pmx_set_paterror(pat);
  }
  #undef utl_W
  // }}

  // {{ Advance pattern
  while (*pat_end == '>') pat_end++;
  *pat_ptr=pat_end;
  // }}
  
  if (n < min_n) return 0;
  
  // {{ Advance matched text
  *txt_ptr = txt;
  // }}
  
  return 1;
}

static char *utl_pmx_alt_skip(char *pat)
{
  int paren=0;
  
  while (*pat) {
    switch (*pat++) {
      case '%': if (*pat) pat++; /* works for utf8 as well */
                break;
                
      case '(': paren++;
                utl_pmx_newcap(NULL);
                break;
                
      case ')': if (paren == 0) return (pat-1);
                paren--;
                break;
                
      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;
    }
  }
  return pat;
}

static char *utl_pmx_alt(char *pat, char **txt_ptr)
{
  int paren=0;
  utl_pmx_state_s *state;
  int inv;
  char *ret = utl_emptystring;
  
  while (*pat) {
    switch (*pat++) {
      case '%': if (*pat) pat++; /* works for utf8 as well */
                break;
                
      case '(': paren++;
                utl_pmx_newcap(NULL);
                break;
                
      case ')': if (paren > 0) {
                  paren--;
                  break;
                }
                if (utl_pmx_stack_cnt < 2) {
                  utl_pmx_set_paterror(pat);
                  break;
                }
                
                // {{ If we are here, we have NOT matched what is in the (...) 
                state = utl_pmx_state_top();
                inv = state->inv;
                ret = pat;
                if (inv) *txt_ptr = pmxstart(state->cap);  /* It's ok, we WANTED to fail */
                else if (state->n >= state->min_n) {       /* Matched enough times, ok */
                  utl_pmx_capt[state->cap][0] = state->txt;
                  utl_pmx_capt[state->cap][1] = *txt_ptr;
                }
                else ret = utl_emptystring;                /* We didn't expect to fail */
                // }}

                utl_pmx_state_pop();
                return ret;

      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;

      case '|': if (paren == 0) {
                  state = utl_pmx_state_top();
                  *txt_ptr = pmxstart(state->cap); 
                  return pat;
                }
    }
  }
  return utl_emptystring;
}

static char *utl_pmx_match(char *pat, char *txt)
{
  int32_t len;
  int32_t ch;
  int32_t c1;
  int16_t inv =0;
  utl_pmx_state_s *state;
  
  utl_pmx_state_reset();
  utl_pmx_state_push(pat,txt,1,1,0);
  
  while (*pat) {
    logdebug("match %d [%s] [%s]",pmxcount(),pat,txt);
    c1 = 0; 
    switch (*pat) {
      case '(' : pat++;
                 if (*pat == '|') {inv = 1; pat++;}
                 if (!utl_pmx_state_push(pat,txt,1,1,inv)) 
                   utl_pmx_set_paterror(pat);
                 break;
                 
      case '|' : pat = utl_pmx_alt_skip(pat);
                 break;
      
      case ')' : pat++;
                 _logdebug(")->%d",utl_pmx_stack_cnt);
                 if (utl_pmx_stack_cnt < 2) {
                   utl_pmx_set_paterror(pat-1); 
                   break;
                 }
                 
                 /* If we are here, we have matched what is in the (...) */
                 state = utl_pmx_state_top();
                 inv = state->inv;
                 if (inv) { /* we shouldn't have matched it :( */
                   utl_pmx_state_pop();
                   utl_pmx_FAIL;
                 }
                 
                 utl_pmx_capt[state->cap][1] = txt;  
                 state->n++;
                 _logdebug("match #%d min:%d max:%d",state->n,state->min_n, state->max_n);
                 
                 if (state->n < state->max_n) { 
                   utl_pmx_capt[state->cap][0] = txt;
                   pat = state->pat; /* try to match once more */
                 }
                 else {
                   utl_pmx_capt[state->cap][0] = state->txt;  
                   utl_pmx_state_pop();
                 }
                 
                 break;
                 
      case '<' : if (!utl_pmx_class(&pat,&txt)) utl_pmx_FAIL;
                 break;

      case '%' : if (pat[1]) len = utl_pmx_nextch(++pat, &c1);

      default  : if (c1 == 0) len = utl_pmx_nextch(pat, &c1);
                 len = utl_pmx_nextch(txt, &ch);
                 if (ch != c1) utl_pmx_FAIL;
                 txt += len;
                 pat += len;
                 break;
                 
      fail     : pat = utl_pmx_alt(pat, &txt) ; /* search for an alternative */
                 if (*pat == '\0') utl_pmx_capnum = 0;
                 break;
    }
  }
  utl_pmx_capt[0][1] = txt;
  
  for (len = utl_pmx_capnum; len < utl_pmx_MAXCAPT; len++) {
    utl_pmx_capt[len][0] = utl_pmx_capt[len][1] = NULL;
  }
  _logdebug("res: %p - %p",utl_pmx_capt[0][0],utl_pmx_capt[0][1]);
  return utl_pmx_capt[0][0];
}

char *utl_pmx_search(char *pat, char *txt)
{
  char *ret=NULL;
  
  utl_pmx_error = NULL;
  
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_utf8=0;}
    
  if (*pat == '^')  ret = utl_pmx_match(pat+1,txt);
  else while (!(ret = utl_pmx_match(pat,txt)) && *txt) {
         txt += utl_pmx_utf8 ? utl_pmx_get_utf8(txt, NULL) : 1;
       }
  _logdebug("ret: %p",ret);
  return ret;
}
#endif
#endif
//>>>//
