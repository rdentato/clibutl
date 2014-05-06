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

int main (int argc, char *argv[])
{
  TSTPLAN("utl test: logging") {
    
    TSTSECTION("Enabled") {
        #ifdef UTL_NOLOGGING
          enabled = 0;
          TST("Compiled with logging disabled!",1);
        #endif
    }
    
    TSTSKIP(!enabled,"Logging disabled") {
      TSTSECTION("NULL logger") {
        lg = NULL;
        TSTNULL("logger is NULL",  lg);
        TSTEQPTR("log file is NULL", NULL,logFile(lg));
        TSTEQINT("log is disabled", log_X , logLevel(lg,"?") );
        TSTCODE {
          logLevel(lg,"MSG");
        } TSTEQINT("NULL logger level can't be changed", log_X, logLevel(lg,"") );
      }
      
      TSTSECTION("stdout logger") {
      
        TSTCODE {
          lg = logStdout;
        } 
        TSTNNULL("logger is not NULL",lg);
        TSTEQPTR("log file is stdout", stdout,logFile(lg));
        TSTEQINT("logLevel is Warn", log_W, logLevel(lg,"") );
      
        TSTCODE {
          logLevel(lg,"Msg");
        } TSTEQINT("Log level changed", log_M, logLevel(lg,"?") );
        
        TSTCODE {
          k = 0;
          logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
          logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
          logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
        } TSTEQINT("logLevel() and logIf()", 110, k );
        
        TSTCODE {
          lg = logClose(lg);
        }
        TSTEQPTR("logger is NULL", NULL,lg);
      }
      
      TSTSECTION("stderr logger") {
      
        TSTCODE {
          lg = logStderr;
        } 
        TSTNNULL("logger is not NULL",lg);
        #ifndef UTL_NOLOGGING
        TSTFAILNOTE("&log_stderr = %p logStderr = %p",&utl_log_stderr,logStderr);
        #endif
        TSTEQPTR("log file is stderr", stderr,logFile(lg));
        TSTEQINT("logLevel is Warn", log_W, logLevel(lg,"") );
      
        TSTCODE {
          logLevel(lg,"Msg");
        } TSTEQINT("Log level changed", log_M, logLevel(lg,"?") );
        
        TSTCODE {
          k = 0;
          logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
          logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
          logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
        } TSTEQINT("logLevel() and logIf()", 110, k );
        
        TSTCODE {
          lg = logClose(lg);
        }
        TSTEQPTR("logger is NULL", NULL,lg);
      }
      
      TSTSECTION("test.log") {
      
        TSTCODE {
          lg = logOpen("test.log","w"); 
        } 
        TSTNNULL("logger is not NULL", lg);
        TSTNNULL("log file is not NULL", logFile(lg));
        TSTEQINT("logLevel is Warn", log_W, logLevel(lg,"") );
      
        TSTCODE {
          logLevel(lg,"Msg");
        } TSTEQINT("Log level changed", log_M, logLevel(lg,"") );
        
        TSTCODE {
          k = 0;
          logIf(lg,"Info")  { k+=1; } /* If level is at least INFO  */
          logIf(lg,"Warn")  { k+=10; } /* If level is at least WARN  */
          logIf(lg,"Error") { k+=100; } /* If level is at least ERROR */
        } TSTEQINT("logLevel() and logIf()", 110, k );
        
        TSTCODE {
          lg = logClose(lg);
        }
        TSTNULL("logger is NULL",lg);
        
        TSTCODE {
          f = fopen("test.log","r");
        }
        TSTNNULL("file created",f);
        if(f) fclose(f);
      
        TSTCODE {
          lg = logOpen("test.log","a"); 
          lg = logClose(lg);
        } 
        
        TSTCODE {
          f = fopen("test.log","r");
        }
        TSTNNULL("file still there",f);
        
        TSTCODE {
          buf[0] = '\0';
          if(f) fgets(buf,120,f);
          p=buf;
          while (*p && *p != '\n') p++; *p='\0';
        }
        TSTEQINT("Log creating ok",0,strcmp(buf+20,"LOG CREATED \"test.log\""));
        TSTFAILNOTE("First line: [%s]",buf);
        
        TSTCODE {
          buf[0] = '\0';
          if(f) fgets(buf,120,f);
          p=buf;
          while (*p && *p != '\n') p++;
          *p='\0';
        }
        TSTEQINT("Log appending ok",0,strcmp(buf+20,"LOG ADDEDTO \"test.log\""));
        TSTFAILNOTE("Second line: [%s]",buf);
        
        if(f) fclose(f);
      }
    }
  }
}
