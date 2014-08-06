/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**
*/

#ifndef UTL_H
#define UTL_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include <string.h>
#include <stdarg.h>
#include <stddef.h>

#include <setjmp.h>
#include <assert.h>


/* .% Overview
**
** =========== 
** .v
**                           ___   __
**                       ___/  (_ /  )
**                ___  _(__   ___)  /
**               /  / /  )/  /  /  /
**              /  /_/  //  (__/  /
**             (____,__/(_____(__/
** ..
**
**   This file ('|utl.h|) provide the following basic elements:
**
**  .[Unit Testing]   A simple framework to create unit tests. Tests output
**                    is compliant with the TAP '(Test Anything Protocol)
**                    standard.
**
**   [Logging]        To print logging traces during program execution.
**                    It offers multilevel logging similar to '|log4j|
**                    but limited to files.
**
**   [Finite State Machine]
**                    Macros to use FSM as if they were a native C control
**                    structure (similar to switch).
**
**   [Exceptions]     A simple implementation of try/catch. Similar to C++.
**
**   [Guarded memory allocation]
**                    Replacement for malloc(), calloc(), realloc() and free()
**                    that account and report about misuse of memory.
**
**   [Dynamic arrays]
**   [Text buffers}
**  ..
**
*/

/*
** .% How to use '|utl|
** ====================
**
**  To access '|utl| functions you simply:
**
**  .# #include '|utl.h| in each source file
**   # in one (and only one) of the source files #define the symbol
**     '|UTL_LIB| before including '|utl.h| (a good place is the
**     file where  your '|main()| function is defined)
**  ..
**
**    As an alternative to the second step above, you can create a source
**  file (say '|utl.c|) with only the following lines:
**  .{{ C
**       #define  UTL_C
**       #include "utl.h"
**  .}}
**  and link it to your project.
**
*/

#ifdef UTL_C
#ifndef UTL_LIB
#define UTL_LIB
#endif
#endif

/* .%% Enable/disable utl features
** -------------------------------
**
*/

#ifdef NDEBUG
#ifdef DEBUG
#undef DEBUG
#endif
#endif

#ifdef UTL_NOMEMCHECK
#ifdef UTL_MEMCHECK
#undef UTL_MEMCHECK
#endif
#endif

#ifdef UTL_MEMCHECK
#ifdef UTL_NOLOGGING
#undef UTL_NOLOGGING
#endif
#endif

#ifdef UTL_UNITTEST
#ifndef DEBUG
#define DEBUG
#endif
#endif


/* .% Globals
** ==========
** 
**    There are some symbols (costants or variables) that need to be shared by
**  all the modules that include '|utl.h|.  They are defined with the aid of
**  the macro '{utl_extern} which takes care of their definition and initialization
**  in the '/main/ code and of their declaration as '|extern| in the rest of the code.
*/

#ifdef UTL_LIB
#define utl_extern(n,v) n v
#else
#define utl_extern(n,v) extern n
#endif

#define utl_initvoid ;

#define UTL_VERSION 0x0003

utl_extern(unsigned short utlVersion, = UTL_VERSION);

/* .%% Constants
** -------------
**
**  A set of constants for generic use. Provided by for convenience.
**
**  .[utlEmptyFun]  A pointer to a do-nothing function that can be used
**                  as a generic placeholder (or NULL indicator) for
**                  function pointers. This can be useful as the C standard
**                  doesn't guarantee that one could mix pointers to objects
**                  with pointers to function (even if in reality this is
**                  practically always the case).  
**  ..
*/

int utlEmptyFun(void); 
#ifdef UTL_LIB
int   utlEmptyFun(void) {return 0;}
#endif

/*  .[utlEmptyString]  A pointer to the empty string "" that provides unique
**                     representation for the empty string.  
**  ..
*/

utl_extern(char *utlEmptyString, = "");

/*   .[utlZero]  Is a constant whose value is 0 and that is to be used in
**               idioms like '|do { ... } while (utlZero)|.
**               This is useful to avoid compilers warning about testing
**               constant values.
**   ..
*/
  
#ifdef __GNUC__
#define utlZero 0
#else
utl_extern(const int utlZero, = 0);
#endif


/* .% Assumptions (static assertions)
** ==================================
**
** http://www.drdobbs.com/compile-time-assertions/184401873
*/

#define utl_assum1(e,l) typedef struct {int utl_assumption[(e)?1:-1];} utl_assumption_##l
#define utl_assum0(e,l) utl_assum1(e,l)
#define utlAssume(e)    utl_assum0(e,__LINE__)


#ifndef UTL_NOTRYCATCH

/* .% Exceptions
** ============
**   Exceptions can be very useful when dealing with error conditions is so
** complicate that the logic of errors handling obscures the logic of the 
** program itself.
**   If an error happens in the '|try| section, you can '|throw()| an exception
** (an error condition) and the control goes back to the '|catch| section
** where the proper actions can be taken.
**
**   Simple implementation of try/catch.
** .{{ c
**   tryenv env = NULL;
**   try(env) {
**      ... code ...
**      if (something_failed) throw(env, ERR_OUTOFMEM)  // must be > 0 
**      some_other_func(env); // you can trhow exceptions from other functions 
**      ... code ...          // as long as you pass it the try environment.
**   }  
**   catch({                  // NOTE the catch part is enclosed in special 
*       case ERR_OUTOFMEM :   // braces: '|({ ... })| and is the body of a
**      ... code ...          // '|switch()| statement (including fallthrough!)
**   });
** .}}
**
**  This comes useful when you throw an exception form a called function.
**  The example below, handles the "out of mem" condition in the same place
**  regardless of where the exception was raised.
**
** .{{ C
**
**   #define ERR_OUTOFMEM 0xF0CA
**   tryenv env = NULL; // Remember to set to NULL initally!
**   char *f1(tryenv ee, int x)   { ... throw(ee, ERR_OUTOFMEM} ... }
**   void *f2(tryenv ee, char *x) { ... throw(ee, ERR_OUTOFMEM} ... }
**   try(env) {
**      ... code ...
**      f1(env,3); 
**      ... code ...
**      if ... throw(env,ERR_OUTFOMEM) 
**      ... code ...
**      f2(env,"Hello"); 
**      ... code ...
**   }  
**   catch({                    // Note the use of '|({| and '|})| !!
**      case  ERR_OUTOFMEM : 
**                 ... code ... // Handle all your cleanup here!
**                 break;
**   });
**
** .}}
*/ 


typedef struct utl_env_s { 
  jmp_buf jb;
  struct utl_env_s volatile *prev;
  struct utl_env_s volatile **orig;
} *tryenv; 

#define try(utl_env) \
            do { struct utl_env_s utl_cur_env; volatile int utl_err; \
                 utl_cur_env.prev = (void *)utl_env; \
                 utl_cur_env.orig = (void *)(&utl_env); \
                 utl_env = &utl_cur_env; \
                 if ((utl_err = setjmp(utl_cur_env.jb))==0)
                 
#define catch(y) if (utl_err) switch (utl_err) { \
                      y \
                   } \
                   utl_err = 0;\
                  *utl_cur_env.orig = utl_cur_env.prev; \
                } while(0)

#define throw(env,err) (env? longjmp(((tryenv)env)->jb, err): exit(err))
#define rethrow        (*utl_cur_env.orig = utl_cur_env.prev,throw(*utl_cur_env.orig,utl_err))

#endif

#ifndef UTL_NOFSM
/*  .% Finite state machine
**  =======================
**
**    A Finite State Machine (FSM) is very common paradigm for software that respond to 
**  external events.  There are many ways to code a FSM, this one has the advantage to 
**  closely mimic the graphical representation of a FSM.
**
** .v
**      fsm ({            // Note the use of '|({| and '|})| !!
**
**        case fsmSTART: { ...
**                   if (c == 0) fsmGoto(z);
**                   fsmGoto(y);
**        }
**
**        case z : { ...
**                   break;  // exit from the FSM
**        }
**
**        case y : { ...
**                   if (c == 1) fsmGoto(x);
**                   fsmGoto(z);
**        }
**      });
** ..
**
**   It's a good practice to include a drawing of the FSM in the technical
** documentation (e.g including the GraphViz description in comments).
*/

#define fsmSTART -1
#define fsmEND   -2

#define fsm(x)  do { int utl_fsm_next , utl_fsm_state; \
                      for (utl_fsm_next=fsmSTART; utl_fsm_next>fsmEND;) \
                        switch((utl_fsm_state=utl_fsm_next, utl_fsm_next=fsmEND, utl_fsm_state)) { \
                        x \
                }} while (utlZero)
                         
#define fsmGoto(x)  utl_fsm_next = (x); break
#define fsmRestart  utl_fsm_next = fsmSTART; break
#define fsmExit     utl_fsm_next = fsmEND; break

#endif

#ifdef UTL_UNITTEST

/* .% UnitTest
** ===========
**
**   These macros will help you writing unit tests.  The log produced
** is '<TAP 1.3=http://testanything.org> compatible and also contains
** all the information about passing/failing.
**
**   They are available only if the symbol '|UTL_UNITTEST| is defined before
** including the '|utl.h| header.
** 
**   '{UTL_UNITTEST} implies '{DEBUG}
*/

utl_extern(FILE *TST_FILE, = NULL);
#define TSTFILE (TST_FILE?TST_FILE:stderr)

/* Output is flushed every time to avoid we lose a message in case of
** abnormal exit. 
*/
#define TSTWRITE(...) (fprintf(TSTFILE,__VA_ARGS__),fflush(TSTFILE))

#define TSTTITLE(s) TSTWRITE("TAP version 13\n#\n# ** %s - (%s)\n",s,__FILE__)

#define TST_INIT0 (TSTRES=TSTNUM=TSTGRP=TSTSEC=TSTTOT= \
                               TSTGTT=TSTGPAS=TSTPASS=TSTNSK=TSTNTD=0)
                                 
#define TSTPLAN(s) for (TSTPASSED = TST_INIT0 + 1, TSTTITLE(s); \
                                             TSTPASSED; TSTDONE(),TSTPASSED=0)
                       
/* Tests are divided in sections introduced by '{=TSTSECTION(title)} macro.
** The macro reset the appropriate counters and prints the section header 
*/
#define TSTSECTION(s)  if ((TSTSTAT(), TSTGRP = 0, TSTSEC++, \
                             TSTWRITE("#\n# * %d. %s (%s:%d)\n", \
                             TSTSEC, s, __FILE__, __LINE__), TSTPASS=0)) ((void)0); \
                       else                                               

/* to disable an entire test section, just prepend '|_| or '|X|*/
 
#define XTSTSECTION(s) if (!utlZero) ((void)0); else 
#define _TSTSECTION(s) XTSTSECTION(s)

/* In each section, tests can be logically grouped so that different aspects
** of related functions can be tested.
*/
#define TSTGROUP(s) \
    if ((TSTWRITE("#\n# *   %d.%d %s\n",TSTSEC,++TSTGRP,s),TSTNUM=0)) ((void)0); else
                     
/* to disable a n entire test group , just prepend '|_| or '|X| */
#define XTSTGROUP(s) if (!utlZero) ((void)0); else  
#define _TSTGROUP(s) XTSTGROUP(s)

/* Test code will be skipped if needed */
#define TSTCODE   if (TSTSKP && *TSTSKP)   ((void)0); else  
#define XTSTCODE  if (!utlZero) ((void)0); else
#define _TSTCODE  XTSTCODE
                     
/* The single test is defined  with the '|TST(s,x)| macro.
**   .['|s|] is a short string that identifies the test
**    ['|x|] an assertion that has to be true for the test to succeed.
**   ..
*/
#define XTST(s,x)

#define TST(s,x) (TST_DO(s,((TSTSKP && *TSTSKP)?1:(x))),TST_WRDIR, TSTWRITE("\n"), TSTRES)

#define TST_DO(s,x) (TSTRES = (x), TSTGTT++, TSTTOT++, TSTNUM++, \
                     TSTPASSED = (TSTPASSED && (TSTRES || TSTTD)), \
                     TSTWRITE("%s %4d - %s (:%d)", \
                              (TSTRES? (TSTGPAS++,TSTPASS++,TSTOK) : TSTKO), \
                               TSTGTT, s, __LINE__))

#define TST_WRDIR \
           ((TSTSKP && *TSTSKP)? (TSTNSK++, TSTWRITE(" # SKIP %s",TSTSKP)) \
                   : (TSTTD ? (TSTNTD++, (TSTWRITE(" # TODO %s%s",TSTTD, \
                                        (TSTRES?TSTWRN:utlEmptyString)))) : 0))

#define TSTFAILED  (!TSTRES)

/* You can skip a set of tests giving a reason.
** Nested skips are not supported!
*/
#define TSTSKIP(x,r) for (TSTSKP=(x)?r:utlEmptyString; TSTSKP; TSTSKP=NULL)

#define TSTTODO(r)   for (TSTTD=r; TSTTD; TSTTD=NULL)

#define TSTNOTE(...) \
                 (TSTWRITE("#      "),TSTWRITE(__VA_ARGS__), TSTWRITE("\n"))
                                            
#define TSTFAILNOTE(...) (TSTRES? 0 : (TSTNOTE(__VA_ARGS__)))

#define TSTEXPECTED(f1,v1,f2,v2) \
                             (TSTRES? 0 : (TSTNOTE("Expected "f1" got "f2,v1,v2)))

#define TST_INT(s,e,r,o) do { int utl_exp = (e); int utl_ret = (r);\
                             TST(s,utl_exp o utl_ret);\
                             TSTEXPECTED("(int) "#o" %d",utl_exp,"%d",utl_ret); \
                           } while (utlZero)

                             
#define TSTEQINT(s,e,r)  TST_INT(s,e,r, == )
#define TSTNEQINT(s,e,r) TST_INT(s,e,r, != )
#define TSTLTINT(s,e,r)  TST_INT(s,e,r, < )
#define TSTLEINT(s,e,r)  TST_INT(s,e,r, <= )
#define TSTGTINT(s,e,r)  TST_INT(s,e,r, > )
#define TSTGEINT(s,e,r)  TST_INT(s,e,r, >= )

#define TST_PTR(s,e,r,o)  do { void *utl_exp = (e); void *utl_ret = (r); \
                              TST(s,utl_exp o utl_ret) ; \
                              TSTEXPECTED("(ptr) "#o" 0x%p",utl_exp,"0x%p",utl_ret); \
                            }  while (utlZero)

                           
#define TSTEQPTR(s,e,r)  TST_PTR(s,e,r, == )
#define TSTNEQPTR(s,e,r) TST_PTR(s,e,r, != )
#define TSTNULL(s,r)     TSTEQPTR(s,NULL,r)
#define TSTNNULL(s,r)    TSTNEQPTR(s,NULL,r)

              
#define TSTBAILOUT(r) \
          if (!(r)) ((void)0); else {TSTWRITE("Bail out! %s\n",r); TSTDONE(); exit(0);}

/* At the end of a section, the accumulated stats can be printed out */
#define TSTSTAT() \
          (TSTTOT == 0 ? 0 : ( \
           TSTWRITE("#\n# SECTION %d OK: %d/%d\n",TSTSEC,TSTPASS,TSTTOT), \
           TSTTOT = 0))

/* At the end of all the tests, the accumulated stats can be printed out */
#define TSTDONE() \
  (TSTGTT <= 0 ? 0 : ( TSTSTAT(),  \
  TSTWRITE("#\n# TOTAL OK: %d/%d SKIP: %d TODO: %d\n",TSTGPAS,TSTGTT, \
                                                              TSTNSK,TSTNTD), \
  TSTWRITE("#\n# TEST PLAN: %s \n",TSTPASSED ? "PASSED" : "FAILED"), \
  TSTWRITE("#\n1..%d\n",TSTGTT),fflush(TSTFILE)) )

/* Execute a statement if a test succeeded */
#define TSTIF_OK  if (TSTRES)

/* Execute a statement if a test failed */
#define TSTIF_NOTOK if (!TSTRES)

static int    TSTRES    = 0;  /* Result of the last performed '|TST()| */
static int    TSTNUM    = 0;  /* Last test number */
static int    TSTGRP    = 0;  /* Current test group */
static int    TSTSEC    = 0;  /* Current test SECTION*/
static int    TSTTOT    = 0;  /* Number of tests executed */
static int    TSTGTT    = 0;  /* Number of tests executed (Grand Total) */
static int    TSTGPAS   = 0;  /* Number of tests passed (Grand Total) */
static int    TSTPASS   = 0;  
static int    TSTPASSED = 1;  
static int    TSTNSK    = 0;  
static int    TSTNTD    = 0;  
static char  *TSTSKP    = NULL;
static char  *TSTTD     = NULL;

static const char *TSTOK  = "ok    ";
static const char *TSTKO  = "not ok";
static const char *TSTWRN = " (passed unexpectedly!)";


#endif /* UTL_UNITTEST */

/* .% Logging
** ==========
**
*/

#define log_D 7
#define log_I 6
#define log_M 5
#define log_W 4
#define log_E 3
#define log_C 2
#define log_A 1
#define log_F 0


#define log_X (log_D + 1)
#define log_L (log_D + 2)


/* Logging functions are available unless the symbol '{=UTL_NOLOGGING}
** has been defined before including '|utl.h|.
*/

#ifndef UTL_NOLOGGING

#define UTL_LOG_NEW 0x00    
#define UTL_LOG_ADD 0x01    /* append to existing file */
#define UTL_LOG_ERR 0x02    /* use stderr */
#define UTL_LOG_OUT 0x04    /* use stdout */

typedef struct {
  FILE          *file;
  unsigned char  level;
  unsigned char  flags;
  unsigned short rot;
  char          *pre;
} utl_log_s, *utlLogger;

#define utl_log_stdout_init {NULL, log_W, UTL_LOG_OUT,0,NULL}
utl_extern(utl_log_s utl_log_stdout , = utl_log_stdout_init);
#define logStdout (&utl_log_stdout)

#define utl_log_stderr_init {NULL, log_W, UTL_LOG_ERR,0,NULL}
utl_extern(utl_log_s utl_log_stderr , = utl_log_stderr_init);
#define logStderr (&utl_log_stderr)

#define logNull NULL

utl_extern(utlLogger utl_logger , = logNull);

#include <time.h>
#include <ctype.h>

/* .%% Logging levels
** ~~~~~~~~~~~~~~~~~~
**
**   Logging levels are hierarchical and structured. Default log level is WARN.
**
**   Use '{=logLevel()}    to set the desired level of logging.
**   Use '{=logLevelEnv()} to set the desired level of logging based on an
**                         enviroment variable.
*/

                                    /* 0   1   2   3   4   5   6   7   8   9    */
                                    /* 0   4   8   12  16  20  24  28  32  36   */
utl_extern(char const utl_log_abbrev[], = "FTL ALT CRT ERR WRN MSG INF DBG OFF LOG ");

int   utl_log_level(utlLogger lg);
int   utl_log_chrlevel(char *l);
int   logLevel(utlLogger lg, char *lv); 
int   logLevelEnv(utlLogger lg, char *var, char *level);

/*
** The table below shows whether a message of a certain level will be
** printed (Y) or not (N) given the current level of logging.
** .v
**                          message level 
**                    DBG INF MSG WRN ERR CRT ALT FTL
**               DBG   Y   Y   Y   Y   Y   Y   Y   Y
**               INF   N   Y   Y   Y   Y   Y   Y   Y
**               MSG   N   N   Y   Y   Y   Y   Y   Y
**      current  WRN   N   N   N   Y   Y   Y   Y   Y
**      logging  ERR   N   N   N   N   Y   Y   Y   Y
**       level   CRT   N   N   N   N   N   Y   Y   Y
**               ALT   N   N   N   N   N   N   Y   Y
**               FTL   N   N   N   N   N   N   N   Y
**               OFF   N   N   N   N   N   N   N   N
** ..
*/

/* .%% Logging file rotate
** ~~~~~~~~~~~~~~~~~~~~~~~
**
** For long running programs (servers, daemons, ...) it is important to rotate 
** the log files from time to time so that they won't become too big.
** The function logRotateOn() will check the current size of the log file and 
** if it's bigger than the threshold specified, will close it and open a new
** one with the same name and numbering it.
** Then new file will be renamed _1, _2, etc.
**
**   logRotate(lg,n)
**
*/


/* .%% Logging format
** ~~~~~~~~~~~~~~~~~~
** 
** Log files have the following format:
** .v
**     <date> <time> <level> <message>
** ..
**
**  For example:
** .v
**     2009-01-29 13:46:02 ERR An error!
**     2009-01-29 13:46:02 FTL An unrecoverable error
** ..
**
*/

/* .%% Loggers
** ~~~~~~~~~~~
/*    Log files can be opened in "write" or "append" mode as any normal file 
** using the '{=logOpen()} function.
** For example:
** .v  
**   utlLogger lgr = NULL;
**   logOpen(lgr,"file1.log","w") // Delete old log file and create a new one
**   ...
**   logOpen(lgr,"file1.log","a") // Append to previous log file
** .. 
**
**   There are three predefined loggers:
**   .[{logNull}]    A null logger that won't output any message
**    [{logStdout}]  A logger that will output on stdout
**    [{logStderr}]  A logger that will output on stderr
**   ..
** They are '{logClose()} safe, i.e. you can pass them to logClose() and nothing
** bad will happen.
*/

#define logOpen(f,m)   utl_logOpen(f,m)
#define logClose(l)    utl_log_close(l)

#define logPre(l,s)      ((l)->pre = s)

utlLogger utl_logOpen(char *fname, char *mode);
utlLogger utl_logClose(utlLogger lg);
void utl_log_write(utlLogger lg,int lv, int tstamp, char *format, ...);

#define logFile(l) utl_logFile(l)
#define logLevel(lg,lv)      utl_logLevel(lg,lv)
#define logLevelEnv(lg,v,l)  utl_logLevelEnv(lg,v,l)

#define logIf(lg,lc) utl_log_if(lg,utl_log_chrlevel(lc))

#define utl_log_if(lg,lv) if ((lv) > utl_log_level(lg)) ((void)0) ; else
          
#define logDebug(lg, ...)      utl_log_write(lg, log_D, 1, __VA_ARGS__)
#define logInfo(lg, ...)       utl_log_write(lg, log_I, 1, __VA_ARGS__)
#define logMessage(lg, ...)    utl_log_write(lg, log_M, 1, __VA_ARGS__)
#define logWarn(lg, ...)       utl_log_write(lg, log_W, 1, __VA_ARGS__)
#define logError(lg, ...)      utl_log_write(lg, log_E, 1, __VA_ARGS__)
#define logCritical(lg, ...)   utl_log_write(lg, log_C, 1, __VA_ARGS__)
#define logAlarm(lg, ...)      utl_log_write(lg, log_A, 1, __VA_ARGS__)
#define logFatal(lg, ...)      utl_log_write(lg, log_F, 1, __VA_ARGS__)

#define logDContinue(lg, ...)  utl_log_write(lg, log_D, 0, __VA_ARGS__)
#define logIContinue(lg, ...)  utl_log_write(lg, log_I, 0, __VA_ARGS__)
#define logMContinue(lg, ...)  utl_log_write(lg, log_M, 0, __VA_ARGS__)
#define logWContinue(lg, ...)  utl_log_write(lg, log_W, 0, __VA_ARGS__)
#define logEContinue(lg, ...)  utl_log_write(lg, log_E, 0, __VA_ARGS__)
#define logCContinue(lg, ...)  utl_log_write(lg, log_C, 0, __VA_ARGS__)
#define logAContinue(lg, ...)  utl_log_write(lg, log_A, 0, __VA_ARGS__)
#define logFContinue(lg, ...)  utl_log_write(lg, log_F, 0, __VA_ARGS__)

#define logAssert(lg,e)        utl_log_assert(lg, e, #e, __FILE__, __LINE__)

/*
** .v
**   logError("Too many items at counter %d (%d)",numcounter,numitems);
**   logContinue("Occured %d times",times++);
** ..
** will produce:
** .v
**     2009-01-29 13:46:02 ERR Too many items at counter 9 (5)
**                             Occured 3 times
** ..
*/

#ifdef UTL_LIB
int   utl_log_level(utlLogger lg) { return (int)(lg ? lg->level : log_X) ; }

FILE *utl_logFile(utlLogger lg)
{
  if (!lg) return NULL;
  if (lg->flags & UTL_LOG_ERR) return stderr;
  if (lg->flags & UTL_LOG_OUT) return stdout;
  return lg->file;
}

int   utl_log_chrlevel(char *l) {
  int i=0;
  char c = l ? toupper(l[0]) : 'W';
  
  while (utl_log_abbrev[i] != ' ' && utl_log_abbrev[i] != c) i+=4;
  i = (i <= 4*7) ? (i>> 2) : log_W;
  return i;
}


int utl_logLevel(utlLogger lg, char *lv) 
{
  if (!lg) return log_X;
  
  if (lv && lv[0] && lv[0] != '?')
      lg->level = utl_log_chrlevel(lv);
  return utl_log_level(lg);  
}

int utl_logLevelEnv(utlLogger lg, char *var, char *level)
{
  char *lvl_str;
  
  lvl_str=getenv(var);
  if (!lvl_str) lvl_str = level;
  return utl_logLevel(lg,lvl_str);
}

utlLogger utl_logOpen(char *fname, char *mode)
{
  char md[4];
  utlLogger lg = logStderr;
  FILE *f = NULL;
  
  if (fname) {
    md[0] = mode[0]; md[1] = '+'; md[2] = '\0';
    if (md[0] != 'a' && md[0] != 'w') md[0] = 'a'; 
    f = fopen(fname,md);
  }
  
  if (f) {
    lg = malloc(sizeof(utl_log_s));
    if (lg) { 
      lg->flags = 0;
      lg->rot = 0;
    lg->pre = NULL;
      lg->file = f;
    /* Assume that log_L is the last level in utl_log_abbrev */
    utlAssume( (log_L +1) == ((sizeof(utl_log_abbrev)-1)>>2));
      lg->level = log_L;
      utl_log_write(lg,log_L, 1, "%s \"%s\"", (md[0] == 'a') ? "ADDEDTO" : "CREATED",fname); 
      
      lg->level = log_W;
    }
  }
  if (f && !lg) fclose(f);
  return lg;
}

utlLogger utl_log_close(utlLogger lg)
{
  if (lg && lg != logStdout && lg != logStderr) {
    if (lg->file) fclose(lg->file);
    lg->file = NULL;
    free(lg);
  }
  return NULL;
}

/* 
** if limit reached, close the log and open a new one incrementing
** the name.
**      mylog.log
**      mylog_001.log
**      mylog_002.log
**       etc...
*/
static void utl_log_rotate(utlLogger lg)
{
  // TODO:
}

void utl_log_write(utlLogger lg, int lv, int tstamp, char *format, ...)
{
  va_list args;
  char tstr[32];
  time_t t;
  FILE *f = stderr;
  int lg_lv = log_W;
  
  if (!lg) return; 
  
  if (lg->flags & UTL_LOG_OUT) f = stdout;
  else if (lg->flags & UTL_LOG_ERR) f = stderr;
  else f = lg->file;
  
  lg_lv = lg->level;
  lv = lv & 0x0F;
  if( lv <= lg_lv) {
    if (tstamp) {
      time(&t);
      strftime(tstr,32,"%Y-%m-%d %X",localtime(&t));
    } else {
      strcpy(tstr,"                   ");          
    }
    if (lg->pre) fprintf(f, "%s ",lg->pre);
    fprintf(f, "%s %.4s", tstr, utl_log_abbrev+(lv<<2));
    va_start(args, format);  vfprintf(f,format, args);  va_end(args);
    fputc('\n',f);
    fflush(f);
    if (lg->rot >0) utl_log_rotate(lg);
  }    
}

void utl_log_assert(utlLogger lg,int e,char *estr, char *file,int line)
{ 
  if (!e) {
    logFatal(lg,"Assertion failed:  %s, file %s, line %d", estr, file, line);
#ifndef NDEBUG
  abort();
#endif
  }
}
                 
#endif  /*- UTL_LIB */

#else   /*- UTL_NOLOGGING */

#define logLevel(lg,lv)       log_W
#define logLevelEnv(lg,v,l)   log_W     
#define logDebug(lg, ...)     ((void)0)
#define logInfo(lg, ...)      ((void)0)
#define logMessage(lg, ...)   ((void)0)
#define logWarn(lg, ...)      ((void)0)
#define logError(lg, ...)     ((void)0)
#define logCritical(lg, ...)  ((void)0)
#define logAlarm(lg, ...)     ((void)0)
#define logFatal(lg, ...)     ((void)0)

#define logDContinue(lg, ...) ((void)0)
#define logIContinue(lg, ...) ((void)0)
#define logMContinue(lg, ...) ((void)0)
#define logWContinue(lg, ...) ((void)0)
#define logEContinue(lg, ...) ((void)0)
#define logCContinue(lg, ...) ((void)0)
#define logAContinue(lg, ...) ((void)0)
#define logFContinue(lg, ...) ((void)0)

#define logAssert(lg,e)       ((void)0)

#define logIf(lg,lv) if (!utlZero) (void)0 ; else

#define logOpen(f,m)    NULL
#define logClose(lg)    NULL

typedef void *utlLogger;

#define logFile(x) NULL
#define logStdout  NULL
#define logStderr  NULL

#endif /*- UTL_NOLOGGING */

#ifdef NDEBUG
#undef logDebug
#define logDebug(lg,...) ((void)0)
#endif  /*- NDEBUG */

#define logNDebug(lg,...) ((void)0)

/* ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~*/

/*  .% Traced memory
**  ================
*/
#define utlMemInvalid    -2
#define utlMemOverflow   -1
#define utlMemValid       0
#define utlMemNull        1

#ifdef UTL_MEMCHECK
void *utl_malloc  (size_t size, char *file, int line );
void *utl_calloc  (size_t num, size_t size, char *file, int line);
void *utl_realloc (void *ptr, size_t size, char *file, int line);
void  utl_free    (void *ptr, char *file, int line );
void *utl_strdup  (void *ptr, char *file, int line);

int utl_check(void *ptr,char *file, int line);

utl_extern(utlLogger utlMemLog , = &utl_log_stderr);

#ifdef UTL_LIB
/*************************************/

static char *BEG_CHK = "\xBE\xEF\xF0\x0D";
static char *END_CHK = "\xDE\xAD\xC0\xDA";
static char *CLR_CHK = "\xDE\xFA\xCE\xD0";

static size_t utl_mem_allocated = 0;

typedef struct {
   size_t size;
   char   chk[4];
   char   data[4];
} utl_mem_t;

#define utl_mem(x) ((utl_mem_t *)((char *)(x) -  offsetof(utl_mem_t, data)))

int utl_check(void *ptr,char *file, int line)
{
  utl_mem_t *p;
  
  if (ptr == NULL) return utlMemNull;
  p = utl_mem(ptr);
  if (memcmp(p->chk,BEG_CHK,4)) { 
    logError(utlMemLog,"Invalid or double freed %p (%u %s %d)",p->data, \
                                               utl_mem_allocated, file, line);     
    return utlMemInvalid; 
  }
  if (memcmp(p->data+p->size,END_CHK,4)) {
    logError(utlMemLog,"Boundary overflow detected %p [%d] (%u %s %d)", \
                              p->data, p->size, utl_mem_allocated, file, line); 
    return utlMemOverflow;
  }
  logInfo(utlMemLog,"Valid pointer %p (%u %s %d)",ptr, utl_mem_allocated, file, line); 
  return utlMemValid; 
}

void *utl_malloc(size_t size, char *file, int line )
{
  utl_mem_t *p;
  
  if (size == 0) logWarn(utlMemLog,"Shouldn't allocate 0 bytes (%u %s %d)", \
                                                utl_mem_allocated, file, line);
  p = malloc(sizeof(utl_mem_t) +size);
  if (p == NULL) {
    logCritical(utlMemLog,"Out of Memory (%u %s %d)",utl_mem_allocated, file, line);
    return NULL;
  }
  p->size = size;
  memcpy(p->chk,BEG_CHK,4);
  memcpy(p->data+p->size,END_CHK,4);
  utl_mem_allocated += size;
  logInfo(utlMemLog,"alloc %p [%d] (%u %s %d)",p->data,size,utl_mem_allocated,file,line);
  return p->data;
};

void *utl_calloc(size_t num, size_t size, char *file, int line)
{
  void *ptr;
  
  size = num * size;
  ptr = utl_malloc(size,file,line);
  if (ptr)  memset(ptr,0x00,size);
  return ptr;
};

void utl_free(void *ptr, char *file, int line)
{
  utl_mem_t *p=NULL;
  
  switch (utl_check(ptr,file,line)) {
    case utlMemNull  :    logWarn(utlMemLog,"free NULL (%u %s %d)", 
                                                utl_mem_allocated, file, line);
                          break;
                          
    case utlMemOverflow : logWarn(utlMemLog, "Freeing an overflown block  (%u %s %d)", 
                                                           utl_mem_allocated, file, line);
    case utlMemValid :    p = utl_mem(ptr); 
                          memcpy(p->chk,CLR_CHK,4);
                          utl_mem_allocated -= p->size;
                          if (p->size == 0)
                            logWarn(utlMemLog,"Freeing a block of 0 bytes (%u %s %d)", 
                                                utl_mem_allocated, file, line);

                          logInfo(utlMemLog,"free %p [%d] (%u %s %d)", ptr, 
                                    p?p->size:0,utl_mem_allocated, file, line);
                          free(p);
                          break;
                          
    case utlMemInvalid :  logError(utlMemLog,"free an invalid pointer! (%u %s %d)", \
                                                utl_mem_allocated, file, line);
                          break;
  }
}

void *utl_realloc(void *ptr, size_t size, char *file, int line)
{
  utl_mem_t *p;
  
  if (size == 0) {
    logWarn(utlMemLog,"realloc() used as free() %p -> [0] (%u %s %d)",ptr,utl_mem_allocated, file, line);
    utl_free(ptr,file,line); 
  } 
  else {
    switch (utl_check(ptr,file,line)) {
      case utlMemNull   : logWarn(utlMemLog,"realloc() used as malloc() (%u %s %d)", \
                                             utl_mem_allocated, file, line);
                          return utl_malloc(size,file,line);
                        
      case utlMemValid  : p = utl_mem(ptr); 
                          p = realloc(p,sizeof(utl_mem_t) + size); 
                          if (p == NULL) {
                            logCritical(utlMemLog,"Out of Memory (%u %s %d)", \
                                             utl_mem_allocated, file, line);
                            return NULL;
                          }
                          utl_mem_allocated -= p->size;
                          utl_mem_allocated += size; 
                          logInfo(utlMemLog,"realloc %p [%d] -> %p [%d] (%u %s %d)", \
                                          ptr, p->size, p->data, size, \
                                          utl_mem_allocated, file, line);
                          p->size = size;
                          memcpy(p->chk,BEG_CHK,4);
                          memcpy(p->data+p->size,END_CHK,4);
                          ptr = p->data;
                          break;
    }
  }
  return ptr;
}

void *utl_strdup(void *ptr, char *file, int line)
{
  char *dest;
  size_t size;
  
  if (ptr == NULL) {
    logWarn(utlMemLog,"strdup NULL (%u %s %d)", utl_mem_allocated, file, line);
    return NULL;
  }
  size = strlen(ptr)+1;

  dest = utl_malloc(size,file,line);
  if (dest) memcpy(dest,ptr,size);
  logInfo(utlMemLog,"strdup %p [%d] -> %p (%u %s %d)", ptr, size, dest, \
                                                utl_mem_allocated, file, line);
  return dest;
}
#undef utl_mem

/*************************************/
#endif

#define malloc(n)     utl_malloc(n,__FILE__,__LINE__)
#define calloc(n,s)   utl_calloc(n,s,__FILE__,__LINE__)
#define realloc(p,n)  utl_realloc(p,n,__FILE__,__LINE__)
#define free(p)       utl_free(p,__FILE__,__LINE__)
#define strdup(p)     utl_strdup(p,__FILE__,__LINE__)

#define utlMemCheck(p)    utl_check(p,__FILE__, __LINE__)
#define utlMemAllocated   utl_mem_allocated
#define utlMemValidate(p) utl_mem_validate(p)

#define utlMalloc(n)     utl_malloc(n,__FILE__,__LINE__)
#define utlCalloc(n,s)   utl_calloc(n,s,__FILE__,__LINE__)
#define utlRealloc(p,n)  utl_realloc(p,n,__FILE__,__LINE__)
#define utlFree(p)       utl_free(p,__FILE__,__LINE__)
#define utlStrdup(p)     utl_strdup(p,__FILE__,__LINE__)

#else /* UTL_MEMCHECK */

#define utlMemCheck(p) utlMemValid
#define utlMemAllocated 0
#define utlMemValidate(p) (p)

#endif /* UTL_MEMCHECK */

#ifndef UTL_NOADT

typedef struct vec_s {
  size_t  max;
  size_t  cnt;
  size_t  esz;
  void   *vec;
} *vec_t;

vec_t utl_vecNew(size_t esz);
#define vecNew(ty) utl_vecNew(sizeof(ty))

vec_t utl_vecFree(vec_t v);
#define vecFree utl_vecFree

int utl_vecSet(vec_t v, size_t i, void *e);
#define vecSet utl_vecSet

int utl_vecAdd(vec_t v, void *e);
#define vecAdd  utl_vecAdd

void *utl_vecGet(vec_t v, size_t  i);
#define vecGet utl_vecGet

int utl_vecResize(vec_t v, size_t n);
#define vecResize utl_vecResize

size_t utl_vecCount(vec_t v);
#define vecCount     utl_vecCount

size_t utl_vecMax(vec_t v);
#define vecMax     utl_vecMax

void  *utl_vecVec(vec_t v);
#define vec(v,ty)   ((ty *)utl_vecVec(v))

#define buf_t vec_t
int utl_bufSet(buf_t bf, size_t i, char c);

#define bufNew() utl_vecNew(1)
#define bufFree  utl_vecFree

char utl_bufGet(buf_t bf, size_t i);
#define bufGet   utl_bufGet

int utl_bufSet(buf_t bf, size_t i, char c);
#define bufSet   utl_bufSet

int utl_bufAdd(buf_t bf, char c);
#define bufAdd   utl_bufAdd

int utl_bufAddStr(buf_t bf, char *s);
#define bufAddStr  utl_bufAddStr

#define bufResize utl_vecResize

#define bufClr(bf) utl_bufSet(bf,0,'\0');

int utl_bufFormat(buf_t bf, char *format, ...);
#define bufFormat utl_bufFormat

#define bufLen vecCount
#define bufMax vecMax
#define bufStr(b) vec(b,char)

int utl_bufAddLine(buf_t bf, FILE *f);
#define bufAddLine utl_bufAddLine

int utl_bufAddFile(buf_t bf, FILE *f);
#define bufAddFile utl_bufAddFile

#if !defined(UTL_HAS_SNPRINTF) && defined(_MSC_VER) && (_MSC_VER < 1800)
#define UTL_ADD_SNPRINTF
#define snprintf  c99_snprintf
#define vsnprintf c99_vsnprintf
#endif

#ifdef UTL_LIB

vec_t utl_vecNew(size_t esz)
{
  vec_t v;
  v = malloc(sizeof(struct vec_s));
  if (v) {
    v->max = 0;    v->cnt = 0;
    v->esz = esz;  v->vec = NULL;
  }
  return v;
}

vec_t utl_vecFree(vec_t v)
{
  if (v) {
    if (v->vec) free(v->vec);
    v->max = 0;  v->cnt = 0;
    v->esz = 0;  v->vec = NULL;
    free(v);
  }
  return NULL;
}

size_t utl_vecCount(vec_t v) { return v? v->cnt : 0; }
size_t utl_vecMax(vec_t v)   { return v? v->max : 0; }
void  *utl_vecVec(vec_t v)   { return v? v->vec : NULL; } 

static int utl_vec_expand(vec_t v, size_t i)
{
  unsigned long new_max;
  char *new_vec = NULL;
   
  if (!v) return 0;
   
  new_max = v->max;
  
  if (new_max < 8) new_max = 8;

  while (new_max <= i) new_max *= 2; /* double */
   
  if (new_max > v->max) {
    new_vec = realloc(v->vec,new_max * v->esz);
    if (!new_vec) return 0;
    v->vec = new_vec;
    v->max = new_max;
  }
  return 1;
}

int utl_vecSet(vec_t v, size_t  i, void *e)
{
  if (!utl_vec_expand(v,i)) return 0;
  
  memcpy(((char *)(v->vec)) + (i * v->esz),e,v->esz);
  if (i >= v->cnt) v->cnt = i+1;
  return 1;
}

void *utl_vecGet(vec_t v, size_t i)
{
  if (!v) return '\0';
  if (i >= v->cnt) return NULL;
  return ((char *)(v->vec)) + (i*v->esz);
}

int utl_vecAdd(vec_t v, void *e)
{
  return utl_vecSet(v,v->cnt,e);
}

int utl_vecResize(vec_t v, size_t n)
{
  size_t new_max = 1;
  char *new_vec = NULL;
  if (!v) return 0;

  while (new_max <= n) new_max *= 2;
  
  if (new_max != v->max) {
    new_vec = realloc(v->vec,new_max * v->esz);
    if (!new_vec) return 0;
    v->vec = new_vec;
    v->max = new_max;
    if (v->cnt > v->max) v->cnt = v->max;
  }
  
  return 1;
}

int utl_bufSet(buf_t bf, size_t i, char c)
{
  char *s;

  if (!utl_vec_expand(bf,i+1)) return 0;
  s = bf->vec;
  s[i] = c;
  if (c == '\0') {
    bf->cnt = i;
  }
  else if (i >= bf->cnt) {
    s[i+1] = '\0';
    bf->cnt = i+1;
  }
  return 1;
}

char utl_bufGet(buf_t bf, size_t i)
{
  if (!bf) return '\0';
  if (i >= bf->cnt) return '\0';
  return ((char*)(bf->vec))[i];
}

int utl_bufAdd(buf_t bf, char c)
{  return utl_bufSet(bf,bf->cnt,c); }

int utl_bufAddStr(buf_t bf, char *s)
{
  if (!bf) return 0;
  if (!s || !*s) return 1;
  
  while (*s) if (!utl_bufSet(bf,bf->cnt,*s++)) return 0;
  
  return utl_bufAdd(bf,'\0');
}

/* A line in the file can be ended by '\r\n', '\n' or '\r'.
** The NEWLINE characters are discarded.
** The string in the buffer is terminated with '\n\0'. 
*/
int utl_bufAddLine(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case '\r' : if ((c = fgetc(f)) != '\n') ungetc(c,f);
      case '\n' :
      case EOF  : c = EOF; utl_bufAdd(bf,'\n'); break;
      default   : utl_bufAdd(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

int utl_bufAddFile(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case EOF  : c = EOF; break;
      default   : utl_bufAdd(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}


/* {{ code from http://stackoverflow.com/questions/2915672 */
#ifdef UTL_ADD_SNPRINTF

inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
  int count;
  va_list ap;
  va_start(ap, format);
  count = c99_vsnprintf(str, size, format, ap);
  va_end(ap);
  return count;
}

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
  int count = -1;
  if (size != 0)   count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
  if (count == -1) count = _vscprintf(format, ap);
  return count;
}
#endif // UTL_ADD_SNPRINTF
/* }} */

int utl_bufFormat(buf_t bf, char *format, ...)
{
  int count;
  int count2;
  va_list ap;

  if (!bf) return -1;
  
  va_start(ap, format);
  count = vsnprintf(NULL,0,format, ap);
  va_end(ap);
  utl_bufSet(bf,count,'\0'); /* ensure there's enough room */
  va_start(ap, format);
  count2 = vsnprintf(bufStr(bf),count+1,format, ap);
  va_end(ap);
  
  assert(count == count2);
  
  return count2;
}

#endif
#endif /* UTL_NOADT */

#endif /* UTL_H */

/**************************************************************************/

