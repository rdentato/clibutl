/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                        ___   __
**                     __/  /_ /  )
**             ___  __(_   ___)  /
**            /  / /  )/  /  /  /
**           /  (_/  //  (__/  / 
**          (____,__/(_____(__/
**    https://github.com/rdentato/clibutl
*/

#include "utl.h"

//<<<//
#ifndef UTL_NOPMX


pmx_t utl_pmx_ ;


int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch) = NULL;

#define utl_pmx_set_paterror(t) do {if (!utl_pmx_.error) {utl_pmx_.error = t;}} while (0)

#define utl_pmx_FAIL       goto fail

#define utl_pmx_newcap(t) do {                                       \
                            if (utl_pmx_.capnum < utl_pmx_MAXCAPT) {  \
                              utl_pmx_.capt[utl_pmx_.capnum][0] =      \
                              utl_pmx_.capt[utl_pmx_.capnum][1] = (t); \
                              utl_pmx_.capnum++;                      \
                            }                                        \
                          } while(0)


static void utl_pmx_state_reset(void)
{
  utl_pmx_.stack_ptr = 0;
  utl_pmx_.capnum = 0;
}

static int utl_pmx_state_push(const char *pat, const char *txt, int32_t min_n, int32_t max_n, int16_t inv)
{
  utl_pmx_state_s *state;
  
  if (utl_pmx_.stack_ptr >= utl_pmx_MAXCAPT) return 0;
  
  state = utl_pmx_.stack + utl_pmx_.stack_ptr;
  
  state->pat   = pat;
  state->txt   = txt;
  state->min_n = min_n;
  state->max_n = max_n;
  state->n     = 0;
  state->inv   = inv;
  state->cap   = utl_pmx_.capnum;
  
  utl_pmx_newcap(txt);
  utl_pmx_.stack_ptr++;
  
  return 1;
}

static int utl_pmx_state_pop(void)
{
  if (utl_pmx_.stack_ptr == 0) return 0;
  utl_pmx_.stack_ptr--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top(void)
{
  if (utl_pmx_.stack_ptr == 0) return NULL;
  return utl_pmx_.stack + (utl_pmx_.stack_ptr-1);
}

int utl_pmx_len(uint8_t n) {return (int)(pmxend(n)-pmxstart(n));}

// Returns the length in bytes of the character or 0 if it is '\0'
static int32_t utl_pmx_nextch(const char *t, int32_t *c_ptr)
{
  int32_t len = 0;
  
  if (utl_pmx_.utf8) len = utlnextutf8(t, c_ptr);
  else if ((*c_ptr = (uint8_t)(*t)) != 0) len = 1;
  
  return len;
}

static int32_t utl_pmx_gethex(const char *pat, int32_t *c_ptr)
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

static int utl_pmx_isin(const char *pat, const char *pat_end, int32_t ch, int32_t (*nxt)(const char *, int32_t *))
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

static int32_t utl_pmx_iscapt(const char *pat, const char *txt)
{
  int32_t len = 0;
  uint8_t capnum = 0; 
  const char *cap;
  
  if ('1' <= *pat && *pat <= '9') {
    capnum = *pat - '0';
    _logdebug("capt: %d %d",capnum,utl_pmx_.capnum);
    if (capnum < utl_pmx_.capnum) {
      cap = pmxstart(capnum);
      while (cap < pmxend(capnum) && *cap && (*cap == *txt)) {
        len++; txt++; cap++;
      }
      if (cap < pmxend(capnum)) len = 0;
    }
  }
  return len;
}

void utl_pmx_extend(int(*ext)(const char *, const char *, int, int32_t))
{
  utl_pmx_ext = ext;
}

#define UTL_PMX_QUOTED 0
#define UTL_PMX_BRACED 1

static int utl_pmx_get_limits(const char *pat, const char *pat_end, const char *txt, int braced,
                             int32_t *c_beg_ptr, int32_t *c_end_ptr, int32_t *c_esc_ptr)
{
  int32_t c_beg = '(';
  int32_t c_end = ')';
  int32_t c_esc = '\0';
  int32_t ch;
  
  _logdebug("BRACE: [%.*s]",pat_end-pat,pat);
  
  if (pat < pat_end) { /* <B()\> <Q""\>*/
    pat += utl_pmx_nextch(pat,&c_esc);
    if (pat < pat_end) {
      c_beg = c_esc; c_esc = '\0';
      pat += utl_pmx_nextch(pat,&c_end);
    }
    if (pat < pat_end) {
      pat += utl_pmx_nextch(pat,&c_esc);
    }
  }
  else {  /* Just <B> or <Q>, try to infer the braces */
    c_beg = '\0';
    (void)utl_pmx_nextch(txt,&ch);
    if (braced) {
           if (ch == '(')    {c_beg=ch; c_end=')';}
      else if (ch == '[')    {c_beg=ch; c_end=']';}
      else if (ch == '{')    {c_beg=ch; c_end='}';}
      else if (ch == '<')    {c_beg=ch; c_end='>';}
    }
    else { // Quoted string
      c_esc = '\\';
           if (ch == '"')    {c_beg=ch; c_end=ch;}
      else if (ch == '\'')   {c_beg=ch; c_end=ch;}
      else if (ch == '`')    {c_beg=ch; c_end=ch;}
      else if (ch == 0x91)   {c_beg=ch; c_end=0x92;}   /* ANSI single quotes */
      else if (ch == 0x93)   {c_beg=ch; c_end=0x94;}   /* ANSI double quotes */
      else if (ch == 0x2018) {c_beg=ch; c_end=0x2019;} /* Unicode single quotes */
      else if (ch == 0x201C) {c_beg=ch; c_end=0x201D;} /* Unicode double quotes */
    }
    if (c_beg=='\0') { /* Valid both as quoted string or braces */
           if (ch == '\xAB') {c_beg=ch; c_end='\xBB';} /* Unicode and ISO-8859-1 "<<" and ">>" */
      else if (ch == '\x8B') {c_beg=ch; c_end='\x9B';} /* Unicode and ISO-8859-1 "<"  and ">"  */
      else if (ch == 0x2039) {c_beg=ch; c_end=0x203A;} /* Unicode Single pointing Angle Quotation */
      else if (ch == 0x2329) {c_beg=ch; c_end=0x232A;} /* Unicode ANGLE BRACKETS */
      else if (ch == 0x27E8) {c_beg=ch; c_end=0x27E9;} /* Unicode MATHEMATICAL ANGLE BRACKETS */
      else if (ch == 0x27EA) {c_beg=ch; c_end=0x27EB;} /* Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS */
      else return 0;
    }
  }
  _logdebug("open:'%d' close:'%d' esc:'%d'",c_beg,c_end,c_esc);
  
  *c_beg_ptr = c_beg;
  *c_end_ptr = c_end;
  *c_esc_ptr = c_esc;
  return 1;
}

static int utl_pmx_get_delimited(const char *pat, const char *txt,int32_t c_beg, int32_t c_end, int32_t c_esc)
{
  int n;
  const char *s;
  int cnt;
  int32_t ch;
  
  s = txt;
  n = utl_pmx_nextch(s,&ch);
  if (n == 0 || ch != c_beg) return 0;
  cnt = 0;
  do {
    s += n;
    n = utl_pmx_nextch(s,&ch);
    if (ch == '\0') return 0;
    _logdebug("BRACE: '%c' cnt:%d",ch,cnt);
    
         if (ch == c_end) { if (cnt == 0) return (s+n)-txt;  else cnt--; }
    else if (ch == c_beg) { cnt++;                                       }
    else if (ch == c_esc) { s += n; n = utl_pmx_nextch(s,&ch);           }
    
  } while (ch);
  utl_pmx_set_paterror(pat);
  
  return s-txt;
  
}

static int utl_pmx_delimited(const char *pat, const char *pat_end, const char *txt, int braced)
{
  int32_t c_beg; int32_t c_end; int32_t c_esc;
  if (!utl_pmx_get_limits(pat,pat_end,txt, braced, &c_beg, &c_end, &c_esc)) return 0;
  return utl_pmx_get_delimited(pat,txt,c_beg,c_end,c_esc);  
}


static int utl_pmx_class(const char **pat_ptr, const char **txt_ptr)
{
  int inv = 0;
  
  const char *pat = *pat_ptr;
  const char *txt = *txt_ptr;
  const char *pat_end;
  
  int32_t len   = 0;
  int32_t n     = 0;
  int32_t min_n = 0;
  int32_t max_n = 0;
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
    case 'a' : utl_W(( utlisalpha(ch) )); break;
    case 's' : utl_W(( utlisspace(ch) )); break;
    case 'u' : utl_W(( utlisupper(ch) )); break;
    case 'l' : utl_W(( utlislower(ch) )); break;
    case 'd' : utl_W(( utlisdigit(ch) )); break;
    case 'k' : utl_W(( utlisblank(ch) )); break;
    case 'x' : utl_W(( utlisxdigit(ch))); break;
    case 'w' : utl_W(( utlisalnum(ch) )); break;
    case 'c' : utl_W(( utliscntrl(ch) )); break;
    case 'g' : utl_W(( utlisgraph(ch) )); break;
    case 'p' : utl_W(( utlispunct(ch) )); break;
    case 'r' : utl_W(( utlisprint(ch) )); break;

    case 'i' : utl_W((ch < 0x80))               ; break;
    
    case '.' : utl_W((ch !='\0' && ch !='\n'))  ; break;

    case '=' : utl_W(utl_pmx_isin_chars(pat+1,pat_end,ch)) ; break;
    case '#' : utl_W(utl_pmx_isin_codes(pat+1,pat_end,ch)) ; break;
    
    case 'N' : utl_W((txt[0]=='\r'
                            ? (txt[1] == '\n'? (len++) : 1)
                            : (txt[0] == '\n'?  1 : 0)    )) ; break;

    case 'Q' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_QUOTED))); break;
    case 'B' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_BRACED))); break;
    
    case 'I' : utl_pmx_.csens = 0; n=min_n; break;
    case 'C' : utl_pmx_.csens = 1; n=min_n; break;
                            
    case '$' : if (*txt == '\0') n=min_n; break;
    
    case '>' : utl_pmx_set_paterror(pat); return 0;
  
    case '^' : if (inv) utl_pmx_set_paterror(pat); inv = 0;
               utl_W((len=utl_pmx_iscapt(pat+1,txt)));
               break;
  
    case ':' : if (utl_pmx_ext)
                 utl_W((len=utl_pmx_ext(pat+1,txt,len,ch)));
               break;
  
    default  : ; //utl_pmx_set_paterror(pat);
  }
  #undef utl_W
  // }}

  // {{ Advance pattern
  while (*pat_end == '>') pat_end++;
  *pat_ptr=pat_end;
  // }}
  
  /* Not enough matches */
  if (n < min_n) return 0;
  
  // {{ Advance matched text
  *txt_ptr = txt;
  // }}
  
  return 1;
}

static const char *utl_pmx_alt_skip(const char *pat)
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

static const char *utl_pmx_alt(const char *pat, const char **txt_ptr)
{
  int paren=0;
  utl_pmx_state_s *state;
  int inv;
  const char *ret = utl_emptystring;
  
  while (*pat) {
    _logdebug("ALT: %s (%d)",pat,utl_pmx_.stack_ptr);
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
                if (utl_pmx_.stack_ptr < 2) {
                  utl_pmx_set_paterror(pat);
                  break;
                }
                
                // {{ If we are here, we have NOT matched what is in the (...) 
                state = utl_pmx_state_top();
                inv = state->inv;
                ret = pat;
                if (inv) { /* It's ok, we WANTED to fail */
                  *txt_ptr = pmxstart(state->cap);  
                  utl_pmx_state_pop();
                  return ret;
                }
                if (state->n >= state->min_n) {       /* It's ok, we matched enough times */
                  utl_pmx_.capt[state->cap][0] = state->txt;
                  utl_pmx_.capt[state->cap][1] = *txt_ptr;
                  utl_pmx_state_pop();
                  return ret;
                }
                // }}
                break;

      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;

      case '|': if (paren == 0) {
                  state = utl_pmx_state_top();
                  *txt_ptr = pmxstart(state->cap); 
                  return pat;
                }
                break;
    }
  }
  return utl_emptystring;
}

static const char *utl_pmx_match(const char *pat, const char *txt)
{
  int32_t len;
  int32_t ch;
  int32_t c1;
  int16_t inv =0;
  utl_pmx_state_s *state;
  
  utl_pmx_state_reset();
  utl_pmx_state_push(pat,txt,1,1,0);
  
  while (*pat) {
    _logdebug("[MATCH] %d [%s] [%s]",pmxcount(),pat,txt);
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
                 _logdebug(")->%d",utl_pmx_.stack_ptr);
                 if (utl_pmx_.stack_ptr < 2) {
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
                 
                 utl_pmx_.capt[state->cap][1] = txt;  
                 state->n++;
                 _logdebug("match #%d min:%d max:%d",state->n,state->min_n, state->max_n);
                 
                 if (state->n < state->max_n) { 
                   utl_pmx_.capt[state->cap][0] = txt;
                   pat = state->pat; /* try to match once more */
                 }
                 else {
                   utl_pmx_.capt[state->cap][0] = state->txt;  
                   utl_pmx_state_pop();
                 }
                 
                 break;
                 
      case '<' : if (!utl_pmx_class(&pat,&txt)) utl_pmx_FAIL;
                 break;

      case '%' : if (pat[1]) len = utl_pmx_nextch(++pat, &c1);

      default  : if (c1 == 0) len = utl_pmx_nextch(pat, &c1);
                 len = utl_pmx_nextch(txt, &ch);
                 if (!utl_pmx_.csens) {
                   ch = utlfoldchar(ch);
                   c1 = utlfoldchar(c1);
                 }
                 if (ch != c1) {
                   _logdebug("FAIL: %d %d",c1,ch);
                   utl_pmx_FAIL;
                 }
                 txt += len;
                 pat += len;
                 break;
                 
      fail     : pat = utl_pmx_alt(pat, &txt) ; /* search for an alternative */
                 if (*pat == '\0') utl_pmx_.capnum = 0;
                 break;
    }
  }
  utl_pmx_.capt[0][1] = txt;
  
  for (len = utl_pmx_.capnum; len < utl_pmx_MAXCAPT; len++) {
    utl_pmx_.capt[len][0] = utl_pmx_.capt[len][1] = NULL;
  }
  _logdebug("res: %p - %p",utl_pmx_.capt[0][0],utl_pmx_.capt[0][1]);
  return utl_pmx_.capt[0][0];
}

const char *utl_pmx_search(const char *pat, const char *txt, int fromstart)
{
  const char *ret=NULL;
  
  utl_pmx_.error = NULL;
  utl_pmx_.csens = 1;
  
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_.utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_.utf8=0;}
    
  if (*pat == '^')  ret = utl_pmx_match(pat+1,txt);
  else while (!(ret = utl_pmx_match(pat,txt)) && *txt && !fromstart) {
         txt += utl_pmx_.utf8 ? utlnextutf8(txt) : 1;
       }
  _logdebug("ret: %p",ret);
  return ret;
}

const char *utl_pmx_scan(const char *pat, const char *txt, pmxaction_t func, void *aux)
{
  const char *ret =NULL;  
  utl_pmx_.error = NULL;
  utl_pmx_.csens = 1;
 
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_.utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_.utf8=0;}
      
  while (*txt)  {
     if ((ret = utl_pmx_match(pat,txt))) {
       if (func(utl_pmx_.capt[0][0],utl_pmx_.capt[0][1],aux))
         break;
       txt = utl_pmx_.capt[0][1];
     }
     else {
       txt += utl_pmx_.utf8 ? utlnextutf8(txt) : 1;
     }
  }

  return txt;
}

void utl_pmx_save(pmx_t *p)
{ 
  if (p) memcpy(p,&utl_pmx_,sizeof(pmx_t));
  utl_pmx_state_reset();
}

void utl_pmx_restore(pmx_t *p)
{
  if (p) memcpy(&utl_pmx_,p,sizeof(pmx_t));  
}



#endif
//>>>//
