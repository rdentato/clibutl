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
chs_t s=NULL;

int main (int argc, char *argv[])
{

  TSTPLAN("utl unit test: chs") {
  
    TSTSECTION("chs creation") {
      TSTGROUP("chsNew()") {
        chsNew(s);
        TSTNEQPTR("Is not NULL", NULL, s );
      }
    }
    TSTSECTION("chs add") {
      TSTGROUP("chs set()") {
        TSTEQINT("Len 0", 0, chsLen(s) );
        chsSetChr(s,0,'a');
        TSTNEQPTR("Is not NULL", NULL, s );
        TSTEQINT("Set properly direct access", 'a', s[0] );
        TSTEQINT("Set properly (chrAt)", 'a', chsChrAt(s,0) );
        TSTEQINT("Len 1", 1, chsLen(s) );
        
      }
    }
  }
}
