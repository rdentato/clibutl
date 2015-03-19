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

void uncatch()
{ 
  if (k==9999) {
    logInfo(lg,"Handled exception");
    logTest(lg,1);
    logTestStat(lg);
  }
}

int main (int argc, char *argv[])
{
  tryenv env=NULL;
  
  atexit(uncatch);
  
  logTestPlan(lg,"utl unit test: unhandled") {
    k = 0;
    try(env) { throw(env,2);}
    catch({ 
      case 1 : k = 1; break;
      case 2 : k = 2; break;
    });
    logEQint(lg,"Exception caught", 2,k);

    k = 9999;
    try(env) { throw(env,4); }
    catch ({
      case 1: k = 1; break;
      case 2: k = 2; break;
      default : rethrow;
    });
    logInfo(lg,"Unhandled exception");
    logTest(lg, 0);
  } 
  return 0;
}