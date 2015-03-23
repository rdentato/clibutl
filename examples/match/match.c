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

int main(int argc, char *argv[])
{
  int k,m;
  vec_t v;
  v = vecNew(char *);

  logLevel(lg,"DBG");  
  logTestPlan(lg,"pattern match") {
  
#if 0
    logTest(lg, 0  < utlMatch("pi","pippoX",v));
    logTest(lg, 0 == utlMatch("xi","pippoX",v));

    logTest(lg, 0  < utlMatch("%ai","pippoX",v));
    logTest(lg, 0 == utlMatch("%Ai","pippoX",v));
    logTest(lg, 0  < utlMatch("%Di","pippoX",v));

    logTest(lg, 0  < utlMatch("%d*x","123x",v));
    logTest(lg, 0  < utlMatch("%d*x","x",v));
    logTest(lg, 0 == utlMatch("%d*x","123y",v));

    logTest(lg, 0  < utlMatch("a|b","a",v));
    logTest(lg, 0  < utlMatch("a|b","b",v));
    logTest(lg, 0 == utlMatch("a|b","c",v));

    logTest(lg, 0  < utlMatch("%d+|b","123xy",v));
    logTest(lg, 0  < utlMatch("%d+|b","b",v));
    logTest(lg, 0 == utlMatch("%d+|b","xy",v));
    
    logTest(lg, 0  < utlMatch("a(bc)d","abcd",v));
    logTest(lg, 0  < utlMatch("a(bc)?d","abcd",v));
    logTest(lg, 0  < utlMatch("a(bc)?d","ad",v));
    logTest(lg, 0  < utlMatch("a(b|c)d","abd",v));
    logTest(lg, 0  < utlMatch("a(b|c)d","acd",v));
    logTest(lg, 0  < utlMatch("a(b|c)?d","abd",v));
#endif
    
    logTest(lg, 0 < utlMatch("a(b|c)?d","acd",v));
    logInfo(lg, "Match [0] len: %d", utlMatchLen(v,0));
    logInfo(lg, "Match [1] len: %d", utlMatchLen(v,1));

    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d %p %p", m, vecGet(char *,v,m*2,NULL),vecGet(char *,v,m*2+1,NULL));
    }
    
    logGTint(lg, "subexpr a(b|c)?d" , 0 , utlMatch("a(b|c)?d","ad",v));
    logInfo(lg, "Match [0] len: %d", utlMatchLen(v,0));
    logInfo(lg, "Match [1] len: %d", utlMatchLen(v,1));
    
    logEQint(lg, "match a(b|(c*e|fg*))?d ad" , 2 , utlMatch("a(b|(c*e|fg*))?d","ad",v));
    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d [%.*s]", m, utlMatchLen(v,m),utlMatchSub(v,m));
    }
    logEQint(lg, "match a(b|(c*e|fg*))?d abd" , 3 , utlMatch("a(b|(c*e|fg*))?d","abd",v));
    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d [%.*s]", m, utlMatchLen(v,m),utlMatchSub(v,m));
    }
    
    logEQint(lg, "match a(b|(c*e)|(fg*))?d accccced" , 8 , utlMatch("a(b|(c*e)|(fg*))?d","accccced",v));
    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d [%.*s]", m, utlMatchLen(v,m),utlMatchSub(v,m));
    }
    
    logEQint(lg, "match a(b|(c*e)|(fg*))?d afgggggd" , 8 , utlMatch("a(b|(c*e)|(fg*))?d","afgggggd",v));
    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d [%.*s]", m, utlMatchLen(v,m),utlMatchSub(v,m));
    }
    
    logGTint(lg, "patterns xd* (match)", 0, utlMatch("xd*","xddd",v));
    
    logEQint(lg, "match a(b(c)(g))d abcgd" , 5 , utlMatch("a(b(c)(g))d","abcgd",v));
    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d [%.*s]", m, utlMatchLen(v,m),utlMatchSub(v,m));
    }
    
    #if 1
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
    for (m = 0; m < UTL_MAX_CAPT; m++) {
      logInfo(lg, "%d [%.*s]", m, utlMatchLen(v,m),utlMatchSub(v,m));
    }
    
    logTest(lg, 0  < utlMatch("a@d","a123c",v));
    logTest(lg, 0  < utlMatch("a@d","a123",v));
    logTest(lg, 0 == utlMatch("a@d","axcc",v));
    logTest(lg, 0  < utlMatch("a%%c%|","a%c|",v));

    logTest(lg, 0  < utlMatch("a@|x","a||||x",v));
    logTest(lg, 0  < utlMatch("a@|?x","ax",v));

    logTest(lg, 0  < utlMatch("x[AZ]p","xAp",v));
    logTest(lg, 0  < utlMatch("x[AZ]p","xZp",v));
    logTest(lg, 0  < utlMatch("x[A-Z]p","xHp",v));

    logTest(lg, 0  < utlMatch("x[AZ]?p","xp",v));
    logTest(lg, 0  < utlMatch("x[AZ]?p","xp",v));
    logTest(lg, 0  < utlMatch("x[A-Z]?p","xp",v));

    logTest(lg, 0  < utlMatch("x[A-FH]p","xHp",v));
    logTest(lg, 0  < utlMatch("x[HA-F]p","xHp",v));
    logTest(lg, 0  < utlMatch("x[A-FH]p","xp",v));
    logTest(lg, 0  < utlMatch("x[HA-F]p","xp",v));
    #endif
    
  }
  
  vecFree(v);
  return 0;
}

