/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**
*/

/***
isalnum 	  %m
isalpha 	  %a
isblank 	  %b
iscntrl 	  %c
isdigit 	  %d
isgraph 	  %g
islower 	  %l
isprint 	  %n
ispunct 	  %p
isspace 	  %s
isupper 	  %u
isxdigit 	  %x
isascii 	  %i
.           %.
%           %%
*           %*
+           %+
?           %?
^           %^
$           %$
[           %[
]           %]
(           %(
)           %)
|           %|
!           %!

[%a_]*[%m_]


*a 0 or more alpha
+a 1 or more alpha
?a 0 or 1 alpha
%a 1 alpha

*A 0 or more non-alpha

?_*[_%d%a]



 [ ]
 ()
 
 expr -> term+ '|' term+
 term -> '!'? fact [?*+]? 
 fact -> '(' expr  ')' | '[' '^'? range+ ']' | '%' any | any
 range -> any '-' any | '\' any | any
*/

#define UTL_LIB
#include "utl.h"

#define lg logStderr

int uchr(char **s)
{ /* one day will do UTF-8 */
  int c;
  c = **s;
  if (c) *s += 1;
  return c;
}

#define utl_endpat(c) (!(c) || ((c) == '|') || ((c) == ')'))
int utl_expr(char **ppat, char **pstr, vec_t v);

/*  Use of v->first and v->last
 * 
 *  We need a stack to keep track of parenthesis and a counter
 *  to know the highest level of parenthesis.
 * *
 */

utlAssume(sizeof(uint32_t) == 2 * sizeof(uint16_t));

int utl_match_push(vec_t v, int n)
{
  int sp = -1;
  uint32_t stk;
  uint16_t *ndx;
  
  if (v) {
    ndx = (uint16_t *)&(v->last);
    sp = ndx[0]; stk = v->first;
    logdbg("push stk: %08X sp: %d val: %X",v->first,ndx[0],n);
    n &= 0x0F; /* the numbers in the stack will be in the range 0-15 */
    if (sp < 8) {
      stk &= ~(0x0F << (4*sp));  /* clear old value */
      stk |=  (n    << (4*sp));  /* set new value */
      sp++;
      ndx[0] = sp; v->first = stk;
    }
  }  
  return sp;
}

int utl_match_pop(vec_t v)
{
  int sp;
  uint32_t stk;
  uint16_t *ndx;
  int n = -1;  
  
  if (v) {
    ndx = (uint16_t *)&(v->last);
    sp = ndx[0]; stk = v->first;
    if (sp > 0) {
      sp--;
      n   = stk & (0x0F << (4*sp));  /* isolate top value */
      n >>= 4 * sp;                  /*  */
      ndx[0] = sp;
    }
    logdbg("pop from stk: %08X sp: %d val: %X",v->first,ndx[0], n);
  }  
  return n;
}

int utl_match_open(vec_t v,char *s)
{
  uint16_t *ndx;
  int n = -1;
  if (v) {
    ndx = (uint16_t *)&(v->last);
    n = ndx[1];
    logdbg("expr( %p %d",v,n);
    if (n < 10) {
      vecSet(char *,v,n*2, s);
      vecSet(char *,v,n*2+1, s);
      utl_match_push(v,n);
      n++;
      ndx[1] = n;
    }
  }
  return n;
}

int utl_match_close(vec_t v, char *s)
{
  int n = -1;

  if (v) {
    n = utl_match_pop(v);
    logdbg("expr) %p %d",v,n);
    if (n >= 0) {
      vecSet(char *,v,n*2+1, s);
    }
  }
  return n;
}


int utl_fact(char **ppat, char **pstr, vec_t v)
{
  int ret = 0;
  char *p = *ppat;
  char *s = *pstr;
  int c,cp;
  int inv = 0;
  int max = 1;
  
  p = *ppat;
  cp = *p;
  if (cp == '@') {max = 0x7FFFFFFF; cp = '%';}
  
  #define utl_class(x,y,z) case x : inv = 1; case y : while ((ret < max) && (!(z) == inv)) ret++
  if (cp == '%') {
     c = uchr(&s);
     p++;
      switch (*p) {
         utl_class('A','a',isalpha(c)); break;
         utl_class('L','l',islower(c)); break;
         utl_class('D','d',isdigit(c)); break;
         default  : p--;
      }
      p++;   
  }
  else if (*p == '(') {
    p++;
    ret = utl_expr(&p,&s,v); 
  }
  else {
    cp = uchr(&p);
    c = uchr(&s);
    ret = ( cp == c);
    logNdbg("fact: '%c' == '%c' (%d)",cp,c,ret);
  }  

  *ppat = p;           /* Pattern is always consumed */
  if (ret) *pstr = s;  /* input text only if it matches */
  return ret;
}

int utl_term(char **ppat, char **pstr, vec_t v)
{
  char *p = *ppat;
  char *s = *pstr;
  char *ss;
  int nmatch = 0;
  int ret = 0;
  int inv = 0;
  
  if (*p == '!') {inv = 1; p++ ;} 
  while(!utl_endpat(*p)) {  /* to handle * and + */
    *ppat = p;
    logNdbg("term: [%s] [%s]",p,s);
    ret = utl_fact(&p,&s,v);
    if (ret) {
      nmatch++;
      if (*p == '+' || *p == '*') { p = *ppat; }   /* let's try again */
      else if (*p == '?') { p++; break; }          /* got it once, ok! */
    } else {
      if (*p == '?' || *p == '*') { ret = 1; p++; } /* no match, but it's ok */
      else  if (*p == '+') { p++; ret = (nmatch > 0); }  /* did it already match once? */
      break;
    }
  }
  logNdbg("termret: %d",ret);
  *ppat = p;              /* Pattern is always consumed! */
  if (inv) return !ret;   /* The ! operator does not consume input */
  if (ret) *pstr = s;     /* Only on match we consume input */
  
  return ret;
}
  
int utl_match_len(vec_t v, int n)
{
  int ret = 0;
  if (n < 10) ret = vecGet(char *,v,n*2+1,NULL) - vecGet(char *,v,n*2,NULL);
  return ret;
}

char *utl_match_capt(vec_t v, int n)
{
  char *ret = NULL;
  if (n < 10)  ret = vecGet(char *,v,n*2,NULL);
  return ret;
}

char *utl_consume(char *p)
{
  int k = 0; /* consume untried pattern */
  while (*p) {
    if (*p == ')') {
      if (k == 0) break;
      k--;
    }
    else if (*p == '(') k++;
    p++;
  }
  return p;
}

int utl_expr(char **ppat, char **pstr, vec_t v)
{
  int ret = 0;
  char *p = *ppat;
  char *s = *pstr;

  utl_match_open(v,*pstr);
  
  for(;;) { /* to handle alternatives */
    while (!utl_endpat(*p) && (ret = utl_term(&p,&s,v))) ;
    logNdbg("alt: [%s][%s]",p,s);
    if (*p == '|' && !ret) { p++; s=*pstr; } /* try next alternative */
    else break;
  }
  logNdbg("exprend (before): [%s]",p);
  
  p = utl_consume(p); /* consume untried pattern */
  if (*p == ')') p++;
  *ppat = p;
  logNdbg("exprend (after): [%s]",p);
  
  if (ret)  *pstr = s;

  utl_match_close(v,*pstr);
  
  return ret;
}

int utl_match(char *pat, char *str, vec_t v)
{
  char *p = pat;
  char *s = str;
  int ret = 0;

  if (v) {
    for (ret = 19; ret <=0; ret--) {
      vecSet(char *,v,ret, NULL);
    }
    v->first = 0;  
    v->last = 0;  
  }  
  ret = utl_expr(&p, &s,v);

  ret = s - str;
  logdbg("consumed: %d",ret);
  return ret; /* the amount of input consumed */
}

#define utlMatch utl_match


int main(int argc, char *argv[])
{
  int k,m;
  vec_t v;
  v = vecNew(char *);

  logLevel(lg,"DBG");  
  logTestPlan(lg,"pattern match") {
  
#if 1
    logGTint(lg, "no patterns (match)", 0, utlMatch("pi","pippoX",v));
    logEQint(lg, "no patterns (nomatch)", 0, utlMatch("xi","pippoX",v));

    logGTint(lg, "patterns %a (match)", 0, utlMatch("%ai","pippoX",v));
    logEQint(lg, "patterns %A (nomatch)", 0, utlMatch("%Ai","pippoX",v));
    logGTint(lg, "patterns %D (match)", 0, utlMatch("%Di","pippoX",v));

    logGTint(lg, "patterns %d*x (match)", 0, utlMatch("%d*x","123x",v));
    logGTint(lg, "patterns %d*x (match)", 0, utlMatch("%d*x","x",v));
    logEQint(lg, "patterns %d*x (nomatch)", 0, utlMatch("%d*x","123y",v));

    logGTint(lg, "alternative a|b (match 1st)", 0, utlMatch("a|b","a",v));
    logGTint(lg, "alternative a|b (match 2nd)", 0, utlMatch("a|b","b",v));
    logEQint(lg, "alternative a|b (nomatch)", 0, utlMatch("a|b","c",v));

    logGTint(lg, "alternative %d+|b (match 1st)", 0, utlMatch("%d+|b","123xy",v));
    logGTint(lg, "alternative %d+|b (match 2nd)", 0, utlMatch("%d+|b","b",v));
    logEQint(lg, "alternative %d+|b (nomatch)", 0, utlMatch("%d+|b","xy",v));
    
    logGTint(lg, "subexpr a(bc)d" , 0 , utlMatch("a(bc)d","abcd",v));
    logGTint(lg, "subexpr a(bc)?d" , 0 , utlMatch("a(bc)?d","abcd",v));
    logGTint(lg, "subexpr a(bc)?d" , 0 , utlMatch("a(bc)?d","ad",v));

    logGTint(lg, "subexpr a(b|c)d"  , 0 , utlMatch("a(b|c)d","abd",v));
    
    logGTint(lg, "subexpr a(b|c)d"  , 0 , utlMatch("a(b|c)d","acd",v));
    logGTint(lg, "subexpr a(b|c)?d" , 0 , utlMatch("a(b|c)?d","abd",v));
#endif
    
    logGTint(lg, "subexpr a(b|c)?d" , 0 , utlMatch("a(b|c)?d","acd",v));
    logInfo(lg, "Match [0] len: %d", utl_match_len(v,0));
    logInfo(lg, "Match [1] len: %d", utl_match_len(v,1));

    for (m = 0; m < 10; m++) {
      logInfo(lg, "%d %p %p", m, vecGet(char *,v,m*2,NULL),vecGet(char *,v,m*2+1,NULL));
    }
    
    logGTint(lg, "subexpr a(b|c)?d" , 0 , utlMatch("a(b|c)?d","ad",v));
    logInfo(lg, "Match [0] len: %d", utl_match_len(v,0));
    logInfo(lg, "Match [1] len: %d", utl_match_len(v,1));
    
    logEQint(lg, "match a(b|(c*e|fg*))?d ad" , 2 , utlMatch("a(b|(c*e|fg*))?d","ad",v));
    for (m = 0; m < 10; m++) {
      logInfo(lg, "%d [%.*s]", m, utl_match_len(v,m),utl_match_capt(v,m));
    }
    logEQint(lg, "match a(b|(c*e|fg*))?d abd" , 3 , utlMatch("a(b|(c*e|fg*))?d","abd",v));
    for (m = 0; m < 10; m++) {
      logInfo(lg, "%d [%.*s]", m, utl_match_len(v,m),utl_match_capt(v,m));
    }
    
    logEQint(lg, "match a(b|(c*e)|(fg*))?d accccced" , 8 , utlMatch("a(b|(c*e)|(fg*))?d","accccced",v));
    for (m = 0; m < 10; m++) {
      logInfo(lg, "%d [%.*s]", m, utl_match_len(v,m),utl_match_capt(v,m));
    }
    
    logEQint(lg, "match a(b|(c*e)|(fg*))?d afgggggd" , 8 , utlMatch("a(b|(c*e)|(fg*))?d","afgggggd",v));
    for (m = 0; m < 10; m++) {
      logInfo(lg, "%d [%.*s]", m, utl_match_len(v,m),utl_match_capt(v,m));
    }
    
    logGTint(lg, "patterns xd* (match)", 0, utlMatch("xd*","xddd",v));
    
    logEQint(lg, "match a(b(c)(g))d abcgd" , 5 , utlMatch("a(b(c)(g))d","abcgd",v));
    for (m = 0; m < 10; m++) {
      logInfo(lg, "%d [%.*s]", m, utl_match_len(v,m),utl_match_capt(v,m));
    }
    
    #if 1
    {
      vec_t v = vecNew(int);
      
      utl_match_push(v,2);
      logEQint(lg,"push 2",2,v->first);
      
      utl_match_push(v,3);
      logEQint(lg,"push 3",0x32,v->first);

      utl_match_push(v,20);
      logEQint(lg,"push 20 (4)",0x0432,v->first);

      utl_match_push(v,5);
      utl_match_push(v,6);
      utl_match_push(v,7);
      utl_match_push(v,8);

      logEQint(lg,"push 5,6,7,8",0x8765432,v->first);
      
      for (k=8;k>1;k--)
         logEQint(lg,"pop ",k,utl_match_pop(v));

      v= vecFree(v);
      
    }
    #endif
  }
  
  vecFree(v);
  return 0;
}

