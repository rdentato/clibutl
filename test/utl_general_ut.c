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

int main (int argc, char *argv[])
{

  TSTPLAN("utl unit test: general") {
  
    TSTSECTION("General use constants") {
      TSTGROUP("utlEmptyFun()") {
        TSTNEQPTR("Is not NULL", NULL, utlEmptyFun );
        TSTEQINT("Can be called", 0,utlEmptyFun());
      }
  
      TSTGROUP("utlEmptyString") {
        TSTNEQPTR("Is not NULL", NULL,utlEmptyString);
        TSTEQINT("Is empty", '\0', utlEmptyString[0]);
      }
  
      TSTGROUP("utlZero") {
        TSTEQINT("Is zero", 0, utlZero);
      }
    }
  }
}
