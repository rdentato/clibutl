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
char *p;
int k=0;
int c=0;
int enabled = 1;
utlLogger lg = NULL;

#define l logStderr

int main (int argc, char *argv[])
{
  logTestPlan(l,"utl test: logging") {
    
    lg = NULL;
    logTestNULL(l,"logger is NULL",  lg);
    logTestEQPtr(l,"log file is NULL", NULL,logFile(lg));
    logTestEQInt(l,"log is disabled", log_X , logLevel(lg,"?") );
    logTestCode(l) {
      logLevel(lg,"MSG");
    } logTestEQInt(l,"NULL logger level can't be changed", log_X, logLevel(lg,"") );
    
    logTestCode(l) {
      lg = logStdout;
    } 
    logTestNNULL(l,"logger is not NULL",lg);
    logTestEQPtr(l,"log file is stdout", stdout,logFile(lg));
    logTestEQInt(l,"logLevel is Warn", log_W, logLevel(lg,"") );
    
    logTestCode(l) {
      logLevel(lg,"Msg");
    } logTestEQInt(l,"Log level changed", log_M, logLevel(lg,"?") );
    
    logTestCode(l) {
      k = 0;
      logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
      logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
      logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
    } logTestEQInt(l,"logLevel() and logIf()", 110, k );
    
    logTestCode(l) {
      lg = logClose(lg);
    }
    logTestEQPtr(l,"logger is NULL", NULL,lg);
    
    logTestCode(l) {
      lg = logStderr;
    } 
    logTestNNULL(l,"logger is not NULL",lg);
    #ifndef UTL_NOLOGGING
    logTestFailNote(l,"&log_stderr = %p logStderr = %p",&utl_log_stderr,logStderr);
    #endif
    logTestEQPtr(l,"log file is stderr", stderr,logFile(lg));
    logTestEQInt(l,"logLevel is Warn", log_W, logLevel(lg,"") );
    
    logTestCode(l) {
      logLevel(lg,"Msg");
    } logTestEQInt(l,"Log level changed", log_M, logLevel(lg,"?") );
    
    logTestCode(l) {
      k = 0;
      logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
      logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
      logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
    } logTestEQInt(l,"logLevel() and logIf()", 110, k );
    
    logTestCode(l) {
      lg = logClose(lg);
    }
    logTestEQPtr(l,"logger is NULL", NULL,lg);
    
    logTestCode(l) {
      lg = logOpen("test.log","w"); 
    } 
    logTestNNULL(l,"logger is not NULL", lg);
    logTestNNULL(l,"log file is not NULL", logFile(lg));
    logTestEQInt(l,"logLevel is Warn", log_W, logLevel(lg,"") );
    
    logTestCode(l) {
      logLevel(lg,"Msg");
    } logTestEQInt(l,"Log level changed", log_M, logLevel(lg,"") );
    
    logTestCode(l) {
      k = 0;
      logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
      logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
      logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
    } logTestEQInt(l,"logLevel() and logIf()", 110, k );
    
    logTestCode(l) {
      lg = logClose(lg);
    }
    logTestNULL(l,"logger is NULL",lg);
    
    logTestCode(l) {
      f = fopen("test.log","r");
    }
    logTestNNULL(l,"file created",f);
    if(f) fclose(f);
    
    logTestCode(l) {
      lg = logOpen("test.log","a"); 
      lg = logClose(lg);
    } 
    
    logTestCode(l) {
      f = fopen("test.log","r");
    }
    logTestNNULL(l,"file still there",f);
    
    logTestCode(l) {
      buf[0] = '\0';
      if(f) fgets(buf,120,f);
      p=buf;
      while (*p && *p != '\n') p++; *p='\0';
    }
    logTestEQInt(l,"Log creating ok",0,strcmp(buf+20,"LOG CREATED \"test.log\""));
    logTestFailNote(l,"First line: [%s]",buf);
    
    logTestCode(l) {
      buf[0] = '\0';
      if(f) fgets(buf,120,f);
      p=buf;
      while (*p && *p != '\n') p++;
      *p='\0';
    }
    logTestEQInt(l,"Log appending ok",0,strcmp(buf+20,"LOG ADDEDTO \"test.log\""));
    logTestFailNote(l,"Second line: [%s]",buf);
    
    if(f) fclose(f);
    
  }
  return 0;
}
