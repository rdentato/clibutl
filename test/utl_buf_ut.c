/* TEST LINE 1
   TEST LINE (a rather long like, to tell the truth) 2
   TEST LINE 3
*/
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

#define lg logStderr

int main (int argc, char *argv[])
{
  logLevel(logStderr,"DBG");
  
  logTestPlan(lg,"utl unit test: buf") {
  
    s = bufNew();
    logNEptr(lg, NULL, s );
    logEQint(lg, utlMemValid, utlMemCheck(s));
    logEQint(lg, 0, bufLen(s) );
    logEQint(lg, 0, bufMax(s) );
    logEQptr(lg, NULL, bufStr(s) );
    logEQint(lg, '\0', bufGet(s,31));

    bufSet(s,0,'a');
    logEQint(lg, utlMemValid, utlMemCheck(bufStr(s)));
    logEQint(lg, 'a', bufStr(s)[0] );
    logEQint(lg, 'a', bufGet(s,0) );
    logEQint(lg, 1, bufLen(s) );

    bufSet(s,1,'b');
    logEQint(lg, 2, bufLen(s));
    logEQint(lg, strlen(bufStr(s)), bufLen(s));
    logEQint(lg, 0, strcmp("ab",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufAdd(s,'c');
    logEQint(lg, 3, bufLen(s));
    logEQint(lg, strlen(bufStr(s)), bufLen(s));
    logEQint(lg, 0, strcmp("abc",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufAddStr(s,"xyz");
    logEQint(lg, 6, bufLen(s));
    logEQint(lg, strlen(bufStr(s)), bufLen(s));
    logEQint(lg, 0, strcmp("abcxyz",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufFormat(s,"|%d|",123);
    logEQint(lg, 5, bufLen(s));
    logEQint(lg, strlen(bufStr(s)), bufLen(s));
    logEQint(lg, 0, strcmp("|123|",bufStr(s)) );

    bufFormat(s,"[%d-%d]",2,3);
    logEQint(lg,0,strcmp("[2-3]",bufStr(s)));
    logTestCode(lg) {
      unsigned long x = 0xFFFFFFFF;
      size_t oldmax = s->max;
      bufFormat(s,"[%X-%X-%X]",x,x,x);
      logEQint(lg,0,strcmp("[FFFFFFFF-FFFFFFFF-FFFFFFFF]",bufStr(s)));
      logTestFailNote(lg,"str: [%s]\n",bufStr(s));
      logGTint(lg,oldmax,s->max);
    }
    logTestCode(lg) {
      int l = bufMax(s);
      bufClr(s);
      logEQint(lg, 0, bufLen(s));
      logEQint(lg, 0, bufStr(s)[0]);
      logEQint(lg, l, bufMax(s));
    }
    {
      FILE *f = fopen(__FILE__,"r");
      size_t k = 0;
      size_t n=0;
      logTestSkip(lg,"Unable to open file "__FILE__,!f) {
        logTestCode(lg) {
          bufAddLine(s,f);
        }
        logEQint(lg, 0, strcmp("/* TEST LINE 1\n",bufStr(s)));
        logTestCode(lg) {
          bufAddLine(s,f);
        }
        logEQint(lg, '2', bufGet(s,bufLen(s)-2));
        logTestCode(lg) {
          bufAddFile(s,f);
          k = bufLen(s);
        }
        while (k>0 && isspace(bufGet(s,--k)) ) ; 
        while (k>0 && (bufGet(s,k-1) != '\n') ) {k--;n++;}; 
        
        logEQstrn(lg, "/* TEST LINE LAST */",bufStr(s)+k,n);
      }
      if (f) fclose(f);
    }
    
    logNNULL(lg, s);
    s = bufFree(s);
    logNULL(lg, s);
    
    logTestCode(lg) {
      s = bufNew();
      logTestSkip(lg,"Unable to create buffer!",!s) {
        bufAddStr(s,"AC");
        logEQint(lg,2,bufLen(s));
        bufIns(s,1,'B');
        logEQint(lg,3,bufLen(s));
        logEQstr(lg,"ABC",bufStr(s));
        
        bufIns(s,0,'@');
        logEQint(lg,4,bufLen(s));
        logEQstr(lg,"@ABC",bufStr(s));

        bufIns(s,4,'D');
        logEQint(lg,5,bufLen(s));
        logEQstr(lg,"@ABCD",bufStr(s));

        bufIns(s,100,'E');
        logEQint(lg,6,bufLen(s));
        logEQstr(lg,"@ABCDE",bufStr(s));

        bufInsStr(s,0,"  ");
        logEQint(lg,8,bufLen(s));
        logEQstr(lg,"  @ABCDE",bufStr(s));

        bufInsStr(s,3,"++");
        logEQint(lg,10,bufLen(s));
        logEQstr(lg,"  @++ABCDE",bufStr(s));

        bufInsStr(s,10,"__");
        logEQint(lg,12,bufLen(s));
        logEQstr(lg,"  @++ABCDE__",bufStr(s));
        
        bufInsStr(s,120,"||");
        logEQint(lg,14,bufLen(s));
        logEQstr(lg,"  @++ABCDE__||",bufStr(s));
     }
      s = bufFree(s);
    }
  }
  return 0;
}

/* DO NOT REMOVE NEXT LINE. IT'S USED FOR TESTING */
/* TEST LINE LAST */