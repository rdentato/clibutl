/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_C
#define UTL_UNITTEST
#define UTL_MEMCHECK
#include "utl.h"

FILE *f = NULL;
char buf[512];
int k=0;
int c=0;
buf_t s=NULL;

pmx_t p;

int main (int argc, char *argv[])
{
  logLevel(logStderr,"DBG");
  logPre(logStderr,"#");
  
  TSTPLAN("utl unit test: pmx") {
  
    TSTSECTION("atoms") {
      TSTGROUP("simple") {
        TSTEQINT("simple letter (equal)",1,pmxMatch("a","a",&p));
        TSTEQINT("simple letter (different)",0,pmxMatch("a","b",&p));
        TSTEQINT("Alpha char ",1,pmxMatch("%a","b",&p));
        TSTEQINT("Alpha char (not)",0,pmxMatch("%a","(",&p));
        TSTEQINT("Non-alpha char ",0,pmxMatch("%A","b",&p));
        TSTEQINT("Non-alpha char (not)",1,pmxMatch("%A","(",&p));
      }
      TSTGROUP("alternate") {
        TSTEQINT("alternate letter (first)",1,pmxMatch("a|b","a",&p));
        TSTEQINT("pattern consumed",'\0',cur_pat(&p)[0]);
        TSTFAILNOTE("pattern: '%s'",cur_pat(&p));
        TSTEQINT("alternate letter (second)",1,pmxMatch("a|b","b",&p));
        TSTEQINT("pattern consumed",'\0',cur_pat(&p)[0]);
        TSTEQINT("alternate letter (third)",1,pmxMatch("a|b|c","c",&p));
        TSTEQINT("alternate letter (fail)",0,pmxMatch("a|b|c","d",&p));
        TSTEQINT("pattern consumed",'\0',cur_pat(&p)[0]);
      }
    }
  }
}
