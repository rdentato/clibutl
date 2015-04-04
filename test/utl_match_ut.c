/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**
*/

/***
 
 expr -> term+ '|' term+
 term -> '!'? fact [?*+]? 
 fact -> '(' expr  ')' | '[' '^'? range+ ']' | '%' any | any
 range -> any '-' any | '\' any | any
*/

#define UTL_LIB
#include "utl.h"

#define lg logStderr

void prtmatch(vec_t v)
{
  int m,l;
  for (m = 0; m < UTL_MAX_CAPT; m++) {
   if ((l=utlMatchLen(v,m))) 
     logInfo(lg, "%d [%.*s]", m, l ,utlMatchSub(v,m));
  }    
}


int main(int argc, char *argv[])
{
  vec_t v;
  v = vecNew(char *);

  logLevel(lg,"DBG");  
  logTestPlan(lg,"pattern match") {
  
    #if 1
    logNEint(lg, 0, utlMatch("pippoX","pi",v));
    logEQint(lg, 0, utlMatch("pippoX","xi",v));

    logNEint(lg, 0, utlMatch("pippoX","%ai",v));
    logEQint(lg, 0, utlMatch("pippoX","%Ai",v));
    logNEint(lg, 0, utlMatch("pippoX","%Di",v));

    logNEint(lg, 0, utlMatch("123x","%d*x",v));
    logNEint(lg, 0, utlMatch("x","%d*x",v));
    logEQint(lg, 0, utlMatch("123y","%d*x",v));

    logNEint(lg, 0, utlMatch("a","a|b",v));
    logNEint(lg, 0, utlMatch("b","a|b",v));
    logEQint(lg, 0, utlMatch("c","a|b",v));

    logNEint(lg, 0, utlMatch("123xy","%d+|b",v));
    logNEint(lg, 0, utlMatch("b","%d+|b",v));
    logEQint(lg, 0, utlMatch("xy","%d+|b",v));
    
    logNEint(lg, 0, utlMatch("abcd","a(bc)d",v));
    logNEint(lg, 0, utlMatch("abcd","a(bc)?d",v));
    logNEint(lg, 0, utlMatch("ad","a(bc)?d",v));
    logNEint(lg, 0, utlMatch("abd","a(b|c)d",v));
    logNEint(lg, 0, utlMatch("acd","a(b|c)d",v));
    logNEint(lg, 0, utlMatch("abd","a(b|c)?d",v));
    
    logNEint(lg, 0, utlMatch("acd","a(b|c)?d",v));
    logInfo(lg, "Match [0] len: %d", utlMatchLen(v,0));
    logInfo(lg, "Match [1] len: %d", utlMatchLen(v,1));

    prtmatch(v);
    
    logNEint(lg, 0 , utlMatch("ad","a(b|c)?d",v));
    logInfo(lg, "Match [0] len: %d", utlMatchLen(v,0));
    logInfo(lg, "Match [1] len: %d", utlMatchLen(v,1));
    
    logEQint(lg, 2, utlMatch("ad","a(b|(c*e|fg*))?d",v));
    prtmatch(v);
    
    logEQint(lg, 8, utlMatch("accccced","a(b|(c*e)|(fg*))?d",v));
    prtmatch(v);
    
    logEQint(lg, 8, utlMatch("afgggggd","a(b|(c*e)|(fg*))?d",v));
    prtmatch(v);
    
    logNEint(lg, 0, utlMatch("xddd","xd*",v));
    
    logEQint(lg, 5, utlMatch("abcgd","a(b(c)(g))d",v));
    prtmatch(v);
    
    {
      vec_t v = vecNew(int);
      int k;

      
      utl_match_push(v,2);
      logTest(lg,2 == v->first);
      
      utl_match_push(v,3);
      logTest(lg,0x32 == v->first);

      utl_match_push(v,20);
      logTest(lg, 0x0432 == v->first);

      utl_match_push(v,5);
      utl_match_push(v,6);
      utl_match_push(v,7);
      utl_match_push(v,8);

      logTest(lg, 0x8765432 == v->first);
      
      for (k=8;k>1;k--)
         logTest(lg, k == utl_match_pop(v));

      v= vecFree(v);
      
    }

    logTest(lg, 5 == utlMatch("a123c","a(@d)c",v));
    prtmatch(v);
    
    logNEint(lg, 0, utlMatch("abc","%a",v));
    logNEint(lg, 0, utlMatch("a123c","a@d",v));
    logNEint(lg, 0, utlMatch("a123","a@d",v));
    logEQint(lg, 0, utlMatch("axcc","a@d",v));
    logNEint(lg, 0, utlMatch("a%c|","a%%c%|",v));

    logNEint(lg, 0, utlMatch("a||||x","a@|x",v));
    logNEint(lg, 0, utlMatch("ax","a@|?x",v));

    logNEint(lg, 0, utlMatch("xAp","x[AZ]p",v));
    logNEint(lg, 0, utlMatch("xZp","x[AZ]p",v));
    logNEint(lg, 0, utlMatch("xHp","x[A-Z]p",v));
    
    logNEint(lg, 0, utlMatch("xZZZZZAp","x[AZ]+p",v));
    logNEint(lg, 0, utlMatch("xAAZZp","x[AZ]+p",v));
    logNEint(lg, 0, utlMatch("xBVHp","x[A-Z]+p",v));

    logNEint(lg, 0, utlMatch("xp","x[AZ]?p",v));
    logNEint(lg, 0, utlMatch("xp","x[AZ]?p",v));
    logNEint(lg, 0, utlMatch("xp","x[A-Z]?p",v));

    logNEint(lg, 0, utlMatch("xHp","x[A-FH]p",v));
    logNEint(lg, 0, utlMatch("xHp","x[HA-F]p",v));
    
    logNEint(lg, 0, utlMatch("x5p","x[^A-FH]p",v));
    logNEint(lg, 0, utlMatch("x4p","x[^HA-F]p",v));
    
    logEQint(lg, 0, utlMatch("xBp","x[^A-FH]p",v));
    logEQint(lg, 0, utlMatch("xHp","x[^HA-F]p",v));
    
    logEQint(lg, 1, utlMatch("abc","(a$Ab)",v));
    logEQint(lg, 'A', utlMatchTok(v));
    prtmatch(v);
    
    logEQint(lg, 1, utlMatch("ab","(a$Ab|x$By)",v));
    logEQint(lg, 'A', utlMatchTok(v));
    prtmatch(v);

    logEQint(lg, 1, utlMatch("xy","(a$Ab|x$By)",v));
    logEQint(lg, 'B', utlMatchTok(v));
    prtmatch(v);
    
    logEQint(lg, 0, utlMatch("ac","(a$Ab|x$By)",v));
    prtmatch(v);

    logEQint(lg, 0, utlMatch("xz","(a$Ab|x$By)",v));
    prtmatch(v);
    
    logEQint(lg, 1, utlMatch("ab","a|x",v));
// prtmatch(v);
    
    logEQint(lg, 1, utlMatch("xy","a|x",v));
//  prtmatch(v);
    
    logEQint(lg, 2, utlMatch("ab","ab|xy",v));
    logEQint(lg, 2, utlMatch("xy","ab|xy",v));
    
    logEQint(lg, 0, utlMatch("ACB","A%.+B",v));
    logEQint(lg, 0, utlMatch("ACCCB","A%.+B",v));
    logNEint(lg, 0, utlMatch("ACCCB","A([^B]*B)+",v));
    logNEint(lg, 0, utlMatch("AB","A([^B]*B)+",v));
    logEQint(lg, 0, utlMatch("AB","A%.+B",v));

    logNEint(lg, 0, utlMatch("AB","AB\1CD",v));
    logNEint(lg, 0, utlMatch("CD","AB\1CD",v));

    
    logEQint(lg, 3, utlMatch("afd","a(b|(c*e|fg*))?d",v));
    prtmatch(v);
    
    logEQint(lg, 3, utlMatch("abd","a(b|(c*e|fg*))?d",v));
    prtmatch(v);
    
    logNEint(lg, 0, utlMatch("2","@d@s?(cm|in)?",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("2in","@d@s?(cm|in)?",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("2cm","@d@s?(cm|in)?",v));
    prtmatch(v);

    logNEint(lg, 0, utlMatch("2", "(@d)@s?(cm|in)?",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("2 in","(@d)@s?(cm|in)?",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("2   cm","(@d)@s?(cm|in)?",v));
    prtmatch(v);

    logNEint(lg, 0, utlMatch("abc","a$A",v));
    
   #endif
   #if 0    
    /*** TEST Syntax ERRORS ***/
    
    logEQint(lg, -4, utlMatch("abc","a(bc",v));
    logEQint(lg, -6, utlMatch("abc","(a(b)c",v));
    logEQint(lg, -3, utlMatch("abc","ab)c",v));
    logEQint(lg, -5, utlMatch("abc","a(b))c",v));

    logEQint(lg, -4, utlMatch("abc","ab+*c",v));
    logEQint(lg, -1, utlMatch("abc","+ab*c",v));
    logEQint(lg, -1, utlMatch("abc",")ab*c",v));
   #endif
   #if 1    

    /*** TEST not (!) ***/
    
    logEQint(lg, 0, utlMatch("abc","a!%l.c",v));
    logNEint(lg, 0, utlMatch("a3c","a!%l%.c",v));
    logEQint(lg, 0, utlMatch("a3bc","a!%dbc",v));
    
    logNEint(lg, 0, utlMatch("2pt","@d!(in|cm)(%l%l)",v));
    prtmatch(v);
    logEQint(lg, 0, utlMatch("2cm","@d!(in|cm)(%l%l)",v));
    prtmatch(v);
   #endif
    

  
  }
  
  vecFree(v);
  return 0;
}

