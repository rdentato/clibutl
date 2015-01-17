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
    logNEptr(lg,"Is not NULL", NULL, s );
    logEQint(lg,"Mem Valid", utlMemValid, utlMemCheck(s));
    logEQint(lg,"Len 0", 0, bufLen(s) );
    logEQint(lg,"Max 0", 0, bufMax(s) );
    logEQptr(lg,"NULL str", NULL, bufStr(s) );
    logEQint(lg,"Get out out bound", '\0', bufGet(s,31));

    bufSet(s,0,'a');
    logEQint(lg,"Mem Valid", utlMemValid, utlMemCheck(bufStr(s)));
    logEQint(lg,"Direct access", 'a', bufStr(s)[0] );
    logEQint(lg,"Get (chrAt)", 'a', bufGet(s,0) );
    logEQint(lg,"Len 1", 1, bufLen(s) );

    bufSet(s,1,'b');
    logEQint(lg,"Len 2", 2, bufLen(s));
    logEQint(lg,"Len 2", strlen(bufStr(s)), bufLen(s));
    logEQint(lg,"Direct access", 0, strcmp("ab",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufAdd(s,'c');
    logEQint(lg,"Len 3", 3, bufLen(s));
    logEQint(lg,"Len 3", strlen(bufStr(s)), bufLen(s));
    logEQint(lg,"Set properly direct access", 0, strcmp("abc",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufAddStr(s,"xyz");
    logEQint(lg,"Len 6", 6, bufLen(s));
    logEQint(lg,"Len 6", strlen(bufStr(s)), bufLen(s));
    logEQint(lg,"Set properly direct access", 0, strcmp("abcxyz",bufStr(s)) );
    logTestFailNote(lg,"str: [%s]\n",bufStr(s));

    bufFormat(s,"|%d|",123);
    logEQint(lg,"Len 5", 5, bufLen(s));
    logEQint(lg,"Len 5", strlen(bufStr(s)), bufLen(s));
    logEQint(lg,"Set properly direct access", 0, strcmp("|123|",bufStr(s)) );

    bufFormat(s,"[%d-%d]",2,3);
    logEQint(lg,"Format 1",0,strcmp("[2-3]",bufStr(s)));
    logTestCode(lg) {
      unsigned long x = 0xFFFFFFFF;
      size_t oldmax = s->max;
      bufFormat(s,"[%X-%X-%X]",x,x,x);
      logEQint(lg,"expanded string",0,strcmp("[FFFFFFFF-FFFFFFFF-FFFFFFFF]",bufStr(s)));
      logTestFailNote(lg,"str: [%s]\n",bufStr(s));
      logGTint(lg,"expanded len",oldmax,s->max);
    }
    logTestCode(lg) {
      int l = bufMax(s);
      bufClr(s);
      logEQint(lg,"Len 0", 0, bufLen(s));
      logEQint(lg,"Empty", 0, bufStr(s)[0]);
      logEQint(lg,"Not shrunk", l, bufMax(s));
    }
    {
      FILE *f = fopen(__FILE__,"r");
      size_t k;
      size_t n=0;
      logTestSkip(lg,"Unable to open file "__FILE__,!f) {
        logTestCode(lg) {
          bufAddLine(s,f);
        }
        logEQint(lg,"First line", 0, strcmp("/* TEST LINE 1\n",bufStr(s)));
        logTestCode(lg) {
          bufAddLine(s,f);
        }
        logEQint(lg,"Second line", '2', bufGet(s,bufLen(s)-2));
        logTestCode(lg) {
          bufAddFile(s,f);
          k = bufLen(s);
        }
        while (k>0 && isspace(bufGet(s,--k)) ) ; 
        while (k>0 && (bufGet(s,k-1) != '\n') ) {k--;n++;}; 
        
        logEQint(lg,"Last line 2", 0, strncmp("/* TEST LINE LAST */",bufStr(s)+k,n));
      }
      if (f) fclose(f);
    }
    
    logNEptr(lg,"Is Not Null", NULL, s);
    s = bufFree(s);
    logEQptr(lg,"Is Null", NULL, s);
    
    logTestCode(lg) {
      s = bufNew();
      logTestSkip(lg,"Unable to create buffer!",!s) {
        bufAddStr(s,"AC");
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Created \"AC\"",2,bufLen(s));
        bufIns(s,1,'B');
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted 'B'",3,bufLen(s));
        logEQint(lg,"Inserted 'B'",0,strcmp(bufStr(s),"ABC"));
        
        bufIns(s,0,'@');
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted '@'",4,bufLen(s));
        logEQint(lg,"Inserted '@'",0,strcmp(bufStr(s),"@ABC"));

        bufIns(s,4,'D');
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted 'D'",5,bufLen(s));
        logEQint(lg,"Inserted 'D'",0,strcmp(bufStr(s),"@ABCD"));

        bufIns(s,100,'E');
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted 'E'",6,bufLen(s));
        logEQint(lg,"Inserted 'E'",0,strcmp(bufStr(s),"@ABCDE"));

        bufInsStr(s,0,"  ");
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted string \"  \"",8,bufLen(s));
        logEQint(lg,"Inserted string \"  \"",0,strcmp(bufStr(s),"  @ABCDE"));

        bufInsStr(s,3,"++");
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted string \"++\"",10,bufLen(s));
        logEQint(lg,"Inserted string \"++\"",0,strcmp(bufStr(s),"  @++ABCDE"));

        bufInsStr(s,10,"__");
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted string \"__\"",12,bufLen(s));
        logEQint(lg,"Inserted string \"__\"",0,strcmp(bufStr(s),"  @++ABCDE__"));
        
        bufInsStr(s,120,"||");
        logTestNote(lg,"Buf: \"%s\"",bufStr(s));
        logEQint(lg,"Inserted string \"||\"",14,bufLen(s));
        logEQint(lg,"Inserted string \"||\"",0,strcmp(bufStr(s),"  @++ABCDE__||"));
     }
      s = bufFree(s);
    }
  }
  return 0;
}

/* DO NOT REMOVE NEXT LINE. IT'S USED FOR TESTING */
/* TEST LINE LAST */