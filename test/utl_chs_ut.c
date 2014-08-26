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

#define lg logStderr

int main (int argc, char *argv[])
{
  logTesPlan(lg,"utl unit test: chs") {
  
    chsNew(s);
    logTestNEPtr(lg,"Is not NULL", NULL, s );
    logTestEQInt(lg,"Len 0", 0, chsLen(s) );

    chsSetChr(s,0,'a');
    logTestNEPtr(lg,"Is not NULL", NULL, s );
    logTestEQInt(lg,"Set properly direct access", 'a', s[0] );
    logTestEQInt(lg,"Set properly (chrAt)", 'a', chsChrAt(s,0) );
    logTestEQInt(lg,"Len 1", 1, chsLen(s) );
        
  }
}
