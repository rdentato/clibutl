/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_C
#define UTL_UNITTEST

#include "utl.h"

FILE *f = NULL;
char buf[512];
int k=0;
int c=0;

#define lg logStderr

int main (int argc, char *argv[])
{
 
  logTestPlan(lg, "utl unit test: general") {
  
    logNNULL(lg, utlEmptyFun );
    logEQint(lg,0,utlEmptyFun());
  
    logNNULL(lg, utlEmptyString);
    logEQint(lg, '\0', utlEmptyString[0]);
    
    logEQint(lg, 0, utlZero);
  }
  return 0;
}
