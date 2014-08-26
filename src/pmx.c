/*#define UTL_NOMATCH*/
#ifndef UTL_NOMATCH

/*
  pmx = term
  term = atom ('|' atom)*
  atom = op? (class | char | '(' term ')' | '{' term '}')
  class = '%' classid | '[' '^'? range+ ']' 
  range = char | char '-' char
  op = '*' | '?' | '+' | '!' | '&'
  
  %a alpha-numeric
  %x hex digit
  %d digit
  %s space (includes NL)
  %n newline
  %w white space (no NL)
  %l letter 
  
  %f floating point
  %i integer
  %b balanced
  %e escape
*/

typedef struct {
  char *orig_str;
  char *orig_pat;
  char *cur_str;
  char *cur_pat;
  int   cur_lvl;
  int   nmatches;
  char *matches[10][2];
} pmx_t;



int utl_pmxMatch(char *pat, char *str, pmx_t *p);
#define pmxMatch utl_pmxMatch

#ifdef UTL_LIB

#define utl_cur_pat(p)   (p)->cur_pat
#define utl_adv_pat(p)   ((p)->cur_pat += 1)

#define utl_cur_str(p)   ((p)->cur_str)
#define utl_adv_str(p)   ((p)->cur_str += 1)

#define utl_cur_start(p) (p)->matches[(p)->cur_lvl][0]
#define utl_cur_end(p)   (p)->matches[(p)->cur_lvl][0]
#define utl_cur_level(p) (p)->cur_lvl

static utl_skipatom(pmx_t *p)
{
  char *pat = utl_cur_pat(p);
  int cnt = 0;
  
  if (!*pat) return;
  if (strchr("*?+!",*pat)) pat++;
  if (*pat == '%') { if (*++pat) pat++;}
  else if (*pat == '[' ) {
    if (*++pat == ']') *pat++;
    while (*pat && *pat++ != ']') ;
  } 
  else if (*pat == '(') {
    pat++;
    while (*pat) {
      if (*pat == '(') cnt++;
      else if (*pat == ')') if (--cnt == 0) {pat++; break;}
      pat++;
    }
  }
  else pat++;
  utl_cur_pat(p) = pat;
}

static int utl_term(pmx_t *p);

static int utl_atom(pmx_t *p)
{
  int r = 0;
  int up = 0;
  char *str = utl_cur_str(p);
  char *pat = utl_cur_pat(p);
  
  if (*pat == '\0') {r = 1;}
  else if (*pat == ')') {r = 1; utl_pmxpop(p); pat++;}
  else if (*pat == '(') {utl_pmxpush(p); pat++;}
  else if (*pat == '%') {
    switch (*++pat) {
      case 'd' : if ( isdigit(*str)) {r = 1; str++;}  break;
      case 'a' : if ( isalpha(*str)) {r = 1; str++;}  break;
      case 'D' : if (!isdigit(*str)) {r = 1; str++;}  break;
      case 'A' : if (!isalpha(*str)) {r = 1; str++;}  break;
      default  : r = 0;
    }
  }
  else {
    if (*pat == *str) {r = 1;  str++;}
    pat++;
  }
    
  if (r) {
    if (utl_cur_start(p) == NULL) utl_cur_start(p) = utl_cur_str(p);
    utl_cur_end(p) = str;
    utl_cur_str(p) = str;
  }
  p->cur_pat = pat;
  return r;
}

static int utl_term(pmx_t *p)
{
  int r = 0;
  char *old_pat = p->cur_pat;
  do {
    if (utl_cur_pat(p)[0] == '|') p->cur_pat += 1;
    r = utl_atom(p);
  } while (r == 0 && utl_cur_pat(p)[0] == '|');
  while (utl_cur_pat(p)[0] == '|') {
    p->cur_pat += 1;
    utl_skipatom(p);
  }
  return r;
}

int utl_pmxMatch(char *pat, char *str, pmx_t *p)
{
  int r;
  p->nmatches = 0;
  p->orig_str = str; p->cur_str = str;
  p->orig_pat = pat; p->cur_pat = pat; 
  utl_cur_level(p) = 0;
  utl_cur_start(p) = NULL;
  utl_cur_end(p) = NULL;
  r =utl_term(p);
  return r;
}
#endif
#endif /* UTL_NOMATCH */