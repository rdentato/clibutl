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

int prtmatch(vec_t v, void *data)
{
  int m,l;
  for (m = 0; m < UTL_MAX_CAPT; m++) {
   if ((l=utlMatchLen(v,m))) 
     logInfo(lg, "%d [%.*s]", m, l ,utlMatchSub(v,m));
  }    
  return 0;
}

int main(int argc, char *argv[])
{

  logLevel(lg,"DBG");  
  logTestPlan(lg,"lexical scan") {
  
    utlScan("abc","%a",prtmatch,NULL);

  }
  
  return 0;
}

