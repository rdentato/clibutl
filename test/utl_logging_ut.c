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
log_t lg = logNull;

#define l logStderr

int ret0() {return 0;}
  

int main (int argc, char *argv[])
{
  logTestPlan(l,"utl test: logging") {
    
    lg = NULL;
    logNULL(l,  lg);
    logEQptr(l, NULL,logFile(lg));
    logEQint(l, log_X , logLevel(lg,"?") );
    logTestCode(l) {
      logLevel(lg,"MSG");
    } logEQint(l, log_X, logLevel(lg,"") );
    
    logTestCode(l) {
      lg = logStdout;
    } 
    logNNULL(l,lg);
    logEQptr(l, stdout,logFile(lg));
    logEQint(l, log_W, logLevel(lg,"") );
    
    logTestCode(l) {
      logLevel(lg,"Msg");
    } logEQint(l, log_M, logLevel(lg,"?") );
    
    logTestCode(l) {
      k = 0;
      logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
      logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
      logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
    } logEQint(l, 110, k );
    
    logTestCode(l) {
      lg = logClose(lg);
    }
    logEQptr(l, NULL,lg);
    
    logTestCode(l) {
      lg = logStderr;
    } 
    logNNULL(l,lg);
    #ifndef UTL_NOLOGGING
    logTestFailNote(l,"&log_stderr = %p logStderr = %p",&utl_log_stderr,logStderr);
    #endif
    logEQptr(l, stderr,logFile(lg));
    logEQint(l, log_W, logLevel(lg,"") );
    
    logTestCode(l) {
      logLevel(lg,"Msg");
    } logEQint(l, log_M, logLevel(lg,"?") );
    
    logTestCode(l) {
      k = 0;
      logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
      logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
      logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
    } logEQint(l, 110, k );
    
    logTestCode(l) {
      lg = logClose(lg);
    }
    logEQptr(l, NULL,lg);
    
    logTestCode(l) {
      lg = logOpen("test.log","w"); 
    } 
    logNNULL(l, lg);
    logNNULL(l, logFile(lg));
    logEQint(l, log_W, logLevel(lg,"") );
    
    logTestCode(l) {
      logLevel(lg,"Msg");
    } logEQint(l, log_M, logLevel(lg,"") );
    
    logTestCode(l) {
      k = 0;
      logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
      logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
      logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
    } logEQint(l, 110, k );
    
    logTestCode(l) {
      lg = logClose(lg);
    }
    logNULL(l,lg);
    
    logTestCode(l) {
      f = fopen("test.log","r");
    }
    logNNULL(l,f);
    if(f) fclose(f);
    
    logTestCode(l) {
      lg = logOpen("test.log","a"); 
      lg = logClose(lg);
    } 
    
    logTestCode(l) {
      f = fopen("test.log","r");
    }
    logNNULL(l,f);
    
    logTestCode(l) {
      buf[0] = '\0';
      if(f) fgets(buf,120,f);
      p=buf;
      while (*p && *p != '\n') p++; *p='\0';
    }
    logEQint(l,0,strcmp(buf+20,"LOG CREATED \"test.log\""));
    logTestFailNote(l,"First line: [%s]",buf);
    
    logTestCode(l) {
      buf[0] = '\0';
      if(f) fgets(buf,120,f);
      p=buf;
      while (*p && *p != '\n') p++;
      *p='\0';
    }
    logEQint(l,0,strcmp(buf+20,"LOG ADDEDTO \"test.log\""));
    logTestFailNote(l,"Second line: [%s]",buf);
    
    if(f) fclose(f);
    
    logClockStart(l) {
      for (k=0;k<1000000;k++) (utlZero <<= 1);
      logAlert(l,"Counted!");
    } logClockStop;
    
    k = 0;
    logTest(l, k++ == ret0());
    logTest(l, k == !ret0());
       
  }
  return 0;
}
