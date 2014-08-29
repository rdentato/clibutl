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
    logTestNEPtr(lg,"Is not NULL", NULL, s );
    logTestEQInt(lg,"Mem Valid", utlMemValid, utlMemCheck(s));
    logTestEQInt(lg,"Len 0", 0, bufLen(s) );
    logTestEQInt(lg,"Max 0", 0, bufMax(s) );
    logTestEQPtr(lg,"NULL str", NULL, bufStr(s) );
    logTestEQInt(lg,"Get out out bound", '\0', bufGet(s,31));

    bufSet(s,0,'a');
    logTestEQInt(lg,"Mem Valid", utlMemValid, utlMemCheck(bufStr(s)));
    logTestEQInt(lg,"Direct access", 'a', bufStr(s)[0] );
    logTestEQInt(lg,"Get (chrAt)", 'a', bufGet(s,0) );
    logTestEQInt(lg,"Len 1", 1, bufLen(s) );

    bufSet(s,1,'b');
    logTestEQInt(lg,"Len 2", 2, bufLen(s));
    logTestEQInt(lg,"Len 2", strlen(bufStr(s)), bufLen(s));
    logTestEQInt(lg,"Direct access", 0, strcmp("ab",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufAdd(s,'c');
    logTestEQInt(lg,"Len 3", 3, bufLen(s));
    logTestEQInt(lg,"Len 3", strlen(bufStr(s)), bufLen(s));
    logTestEQInt(lg,"Set properly direct access", 0, strcmp("abc",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufAddStr(s,"xyz");
    logTestEQInt(lg,"Len 6", 6, bufLen(s));
    logTestEQInt(lg,"Len 6", strlen(bufStr(s)), bufLen(s));
    logTestEQInt(lg,"Set properly direct access", 0, strcmp("abcxyz",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufFormat(s,"|%d|",123);
    logTestEQInt(lg,"Len 5", 5, bufLen(s));
    logTestEQInt(lg,"Len 5", strlen(bufStr(s)), bufLen(s));
    logTestEQInt(lg,"Set properly direct access", 0, strcmp("|123|",bufStr(s)) );

    bufFormat(s,"[%d-%d]",2,3);
    logTestEQInt(lg,"Format 1",0,strcmp("[2-3]",bufStr(s)));
    logTestCode(lg) {
      unsigned long x = 0xFFFFFFFF;
      size_t l = s->max;
      bufFormat(s,"[%X-%X-%X]",x,x,x);
      logTestEQInt(lg,"expanded string",0,strcmp("[FFFFFFFF-FFFFFFFF-FFFFFFFF]",bufStr(s)));
      logTestFailNote(lg,"str: [%s]\n",bufStr(s));
      logTestGTInt(lg,"expanded len",s->max,l);
    }
    logTestCode(lg) {
      int l = bufMax(s);
      bufClr(s);
      logTestEQInt(lg,"Len 0", 0, bufLen(s));
      logTestEQInt(lg,"Empty", 0, bufStr(s)[0]);
      logTestEQInt(lg,"Not shrunk", l, bufMax(s));
    }
    {
      FILE *f = fopen(__FILE__,"r");
      size_t k;
      size_t n=0;
      logTestSkip(lg,"Unable to open file "__FILE__,!f) {
        logTestCode(lg) {
          bufAddLine(s,f);
        }
        logTestEQInt(lg,"First line", 0, strcmp("/* TEST LINE 1\n",bufStr(s)));
        logTestCode(lg) {
          bufAddLine(s,f);
        }
        logTestEQInt(lg,"Second line", '2', bufGet(s,bufLen(s)-2));
        logTestCode(lg) {
          bufAddFile(s,f);
          k = bufLen(s);
        }
        while (k>0 && isspace(bufGet(s,--k)) ) ; 
        while (k>0 && (bufGet(s,k-1) != '\n') ) {k--;n++;}; 
        
        logTestEQInt(lg,"Last line 2", 0, strncmp("/* TEST LINE LAST */",bufStr(s)+k,n));
      }
      if (f) fclose(f);
    }
    
    logTestNEPtr(lg,"Is Not Null", NULL, s);
    s = bufFree(s);
    logTestEQPtr(lg,"Is Null", NULL, s);
  }
  return 0;
}

/* TEST LINE LAST */
