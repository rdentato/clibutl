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
  
    logNEptr(lg,"utlEmptyFun is not NULL", NULL, utlEmptyFun );
    logEQint(lg,"utlEmptyFun can be called", 0,utlEmptyFun());
  
    logNEptr(lg,"utlEmptyString is not NULL", NULL,utlEmptyString);
    logEQint(lg,"utlEmptyString is empty", '\0', utlEmptyString[0]);
    
    logEQint(lg,"utlZero is zero", 0, utlZero);
  }
  return 0;
}
