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

int main (int argc, char *argv[])
{

  TSTPLAN("utl unit test: fsm") {
    TSTSECTION("Simple FSM") {
      TSTGROUP("Start state") {
        TSTCODE {
          fsm ({
            case fsmSTART : k = 1; 
          });
        } TSTEQINT("Start executed", 1,k);
        TSTCODE {
          k = 0;
          fsm ({
            case OTHER : k = 1; 
          });
        } TSTEQINT("No start state", 0,k);
        TSTCODE {
          k = 0;
          fsm ({
            case OTHER : k = 2; break;
            case fsmSTART : k = 1; break;
            case 2 : k = 3; break;
          });
        } TSTEQINT("Start is not the first", 1,k);
        TSTCODE {
          fsm ({
            case fsmSTART : k = 1; break;
            case OTHER : k += 2; break;
          });
        } TSTEQINT("No fallthrough",1,k);
      }
    }
    
    TSTSECTION("Nested FSM") {
      TSTGROUP("Start state") {
        TSTCODE {
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
        } TSTEQINT("Nested",1111,k);
      }
    }
  } 
}
