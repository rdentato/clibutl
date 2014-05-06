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


void uncatch()
{ 
  if (k==9999) {
    TST("Exit for unhandled exception",1);
    TSTDONE();
  }
}

int main (int argc, char *argv[])
{
  tryenv env=NULL;
  
  atexit(uncatch);
  
  TSTPLAN("utl unit test: unhandled") {
    TSTSECTION("Handled catch") {
      TSTGROUP("catch 1") {
        TSTCODE {
          k = 0;
          try(env) { throw(env,2);}
          catch({ 
            case 1 : k = 1; break;
            case 2 : k = 2; break;
          });
        } TSTEQINT("Exception caught", 2,k);
        TSTCODE {
          k = 9999;
          try(env) { throw(env,4); }
          catch ({
            case 1: k = 1; break;
            case 2: k = 2; break;
            default : rethrow;
          });
        } TST("Exit for unhandled exception", 0);
      }
    }
  } 
}