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

typedef struct {
  int x;
  int y;
} mydata_t;

int prtmatch(vec_t v, void *data)
{
  int m,l;
  for (m = 0; m < UTL_MAX_CAPT; m++) {
   if ((l=utlMatchLen(v,m))) 
     logInfo(lg, "%d [%.*s]", m, l ,utlMatchSub(v,m));
  }    
  return 0;
}

int sum_x(vec_t v, void *data)
{
  mydata_t *m = data;
  
  logInfo(lg,"Tok: [%c]",utlMatchTok(v));
  if (data && utlMatchTok(v) == 'N') {
    m->x += atoi(utlMatchSub(v,1));
  }
    
  return 0;
}

int main(int argc, char *argv[])
{

  mydata_t md;
  
  logLevel(lg,"DBG");  
  logTestPlan(lg,"lexical scan") {
  
    logEQint(lg,3,utlScan("abc","%a",prtmatch,NULL));
    logEQint(lg,6,utlScan("a2b3c4","%a(%d)",prtmatch,NULL));

    md.x = md.y = 0;
    logNEint(lg,0,utlScan("4 3 5","(%d)+$N|(%D)+$A",sum_x,&md));
    logEQint(lg,12,md.x);    
    
    md.x = md.y = 0;
    logNEint(lg,0,utlScan("a14bbb3ccc5dddd","(%d)+$N|(%D)+$A",sum_x,&md));
    logEQint(lg,22,md.x);    
    
  }
  
  return 0;
}

