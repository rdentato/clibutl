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

#define START fsmSTART
#define OTHER 1 

#define lg logStderr
int main (int argc, char *argv[])
{

  logTestPlan(lg,"utl unit test: fsm") {
    fsm ({
      case fsmSTART : k = 1; 
    });
    logEQint(lg, 1,k);
    
    k = 0;
    fsm ({
      case OTHER : k = 1; 
    });
    logEQint(lg, 0,k);
    
    k = 0;
    fsm ({
      case OTHER : k = 2; break;
      case fsmSTART : k = 1; break;
      case 2 : k = 3; break;
    });
    logEQint(lg, 1,k);
    
    fsm ({
      case fsmSTART : k = 1; break;
      case OTHER : k += 2; break;
    });
    logEQint(lg,1,k);
    
    k = 0;
    fsm({
      case fsmSTART: k = 1; fsmGoto(1);
      case 2: k = 3; break;
      case 1: k += 10;
              fsm ({
                 case fsmSTART: k += 100; fsmGoto(2);
                 case 2: k += 1000; break;
              });
              break;
    });
    logEQint(lg,1111,k);
    
    k=0;
    fsm({
      case fsmSTART: k = 1 ;fsmGoto(2);
      case 2: if (k==1) fsmGoto(3); else fsmGoto(4);
      case 4: fsmExit;
      case 3: k++; fsmGoto(2);
    });
    logEQint(lg,2,k);
  } 
  
  return 0;
}
