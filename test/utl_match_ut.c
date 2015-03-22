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
  int k;
  vec_t v;
  v = vecNew(char *);

  logLevel(lg,"DBG");  
  logTestPlan(lg,"pattern match") {
  
    #if 1
    logNEint(lg, 0, utlMatch("pi","pippoX",v));
    logEQint(lg, 0, utlMatch("xi","pippoX",v));

    logNEint(lg, 0, utlMatch("%ai","pippoX",v));
    logEQint(lg, 0, utlMatch("%Ai","pippoX",v));
    logNEint(lg, 0, utlMatch("%Di","pippoX",v));

    logNEint(lg, 0, utlMatch("%d*x","123x",v));
    logNEint(lg, 0, utlMatch("%d*x","x",v));
    logEQint(lg, 0, utlMatch("%d*x","123y",v));

    logNEint(lg, 0, utlMatch("a|b","a",v));
    logNEint(lg, 0, utlMatch("a|b","b",v));
    logEQint(lg, 0, utlMatch("a|b","c",v));

    logNEint(lg, 0, utlMatch("%d+|b","123xy",v));
    logNEint(lg, 0, utlMatch("%d+|b","b",v));
    logEQint(lg, 0, utlMatch("%d+|b","xy",v));
    
    logNEint(lg, 0, utlMatch("a(bc)d","abcd",v));
    logNEint(lg, 0, utlMatch("a(bc)?d","abcd",v));
    logNEint(lg, 0, utlMatch("a(bc)?d","ad",v));
    logNEint(lg, 0, utlMatch("a(b|c)d","abd",v));
    logNEint(lg, 0, utlMatch("a(b|c)d","acd",v));
    logNEint(lg, 0, utlMatch("a(b|c)?d","abd",v));
    
    logNEint(lg, 0, utlMatch("a(b|c)?d","acd",v));
    logInfo(lg, "Match [0] len: %d", utlMatchLen(v,0));
    logInfo(lg, "Match [1] len: %d", utlMatchLen(v,1));

    prtmatch(v);
    
    logNEint(lg, 0 , utlMatch("a(b|c)?d","ad",v));
    logInfo(lg, "Match [0] len: %d", utlMatchLen(v,0));
    logInfo(lg, "Match [1] len: %d", utlMatchLen(v,1));
    
    logEQint(lg, 2, utlMatch("a(b|(c*e|fg*))?d","ad",v));
    prtmatch(v);
    
    logEQint(lg, 8, utlMatch("a(b|(c*e)|(fg*))?d","accccced",v));
    prtmatch(v);
    
    logEQint(lg, 8, utlMatch("a(b|(c*e)|(fg*))?d","afgggggd",v));
    prtmatch(v);
    
    logNEint(lg, 0, utlMatch("xd*","xddd",v));
    
    logEQint(lg, 5, utlMatch("a(b(c)(g))d","abcgd",v));
    prtmatch(v);
    
    {
      vec_t v = vecNew(int);
      
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

    logTest(lg, 5 == utlMatch("a(@d)c","a123c",v));
    prtmatch(v);
    
    logNEint(lg, 0, utlMatch("a@d","a123c",v));
    logNEint(lg, 0, utlMatch("a@d","a123",v));
    logEQint(lg, 0, utlMatch("a@d","axcc",v));
    logNEint(lg, 0, utlMatch("a%%c%|","a%c|",v));

    logNEint(lg, 0, utlMatch("a@|x","a||||x",v));
    logNEint(lg, 0, utlMatch("a@|?x","ax",v));

    logNEint(lg, 0, utlMatch("x[AZ]p","xAp",v));
    logNEint(lg, 0, utlMatch("x[AZ]p","xZp",v));
    logNEint(lg, 0, utlMatch("x[A-Z]p","xHp",v));
    
    logNEint(lg, 0, utlMatch("x[AZ]+p","xZZZZZAp",v));
    logNEint(lg, 0, utlMatch("x[AZ]+p","xAAZZp",v));
    logNEint(lg, 0, utlMatch("x[A-Z]+p","xBVHp",v));

    logNEint(lg, 0, utlMatch("x[AZ]?p","xp",v));
    logNEint(lg, 0, utlMatch("x[AZ]?p","xp",v));
    logNEint(lg, 0, utlMatch("x[A-Z]?p","xp",v));

    logNEint(lg, 0, utlMatch("x[A-FH]p","xHp",v));
    logNEint(lg, 0, utlMatch("x[HA-F]p","xHp",v));
    
    logNEint(lg, 0, utlMatch("x[^A-FH]p","x5p",v));
    logNEint(lg, 0, utlMatch("x[^HA-F]p","x4p",v));
    
    logEQint(lg, 0, utlMatch("x[^A-FH]p","xBp",v));
    logEQint(lg, 0, utlMatch("x[^HA-F]p","xHp",v));
    
    logEQint(lg, 1, utlMatch("(a$Ab)","abc",v));
    logEQint(lg, 'A', utlMatchTok(v));
    prtmatch(v);
    
    logEQint(lg, 1, utlMatch("(a$Ab|x$By)","ab",v));
    logEQint(lg, 'A', utlMatchTok(v));
    prtmatch(v);

    logEQint(lg, 1, utlMatch("(a$Ab|x$By)","xy",v));
    logEQint(lg, 'B', utlMatchTok(v));
    prtmatch(v);
    
    logEQint(lg, 0, utlMatch("(a$Ab|x$By)","ac",v));
    prtmatch(v);

    logEQint(lg, 0, utlMatch("(a$Ab|x$By)","xz",v));
    prtmatch(v);
    
    logEQint(lg, 1, utlMatch("a|x","ab",v));
// prtmatch(v);
    
    logEQint(lg, 1, utlMatch("a|x","xy",v));
//  prtmatch(v);
    
    logEQint(lg, 2, utlMatch("ab|xy","ab",v));
    logEQint(lg, 2, utlMatch("ab|xy","xy",v));
    
    logNEint(lg, 0, utlMatch("A%.+B","ACB",v));
    logEQint(lg, 0, utlMatch("A%.+B","ACCCB",v));
    logNEint(lg, 0, utlMatch("A([^B]*B)+","ACCCB",v));
    logNEint(lg, 0, utlMatch("A([^B]*B)+","AB",v));
    logEQint(lg, 0, utlMatch("A%.+B","AB",v));
    #endif
    
    logNEint(lg, 0, utlMatch("AB\001CD","AB",v));
    logNEint(lg, 0, utlMatch("AB\001CD","CD",v));

    
    logEQint(lg, 3, utlMatch("a(b|(c*e|fg*))?d","afd",v));
    prtmatch(v);
    
    logEQint(lg, 3, utlMatch("a(b|(c*e|fg*))?d","abd",v));
    prtmatch(v);
    
    logNEint(lg, 0, utlMatch("@d@s?(cm|in)?","2",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("@d@s?(cm|in)?","2in",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("@d@s?(cm|in)?","2cm",v));
    prtmatch(v);

    logNEint(lg, 0, utlMatch("(@d)@s?(cm|in)?","2",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("(@d)@s?(cm|in)?","2 in",v));
    prtmatch(v);
    logNEint(lg, 0, utlMatch("(@d)@s?(cm|in)?","2   cm",v));
    prtmatch(v);

  }
  
  vecFree(v);
  return 0;
}

