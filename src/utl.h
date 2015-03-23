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
**                        __/  /_ /  )
**                ___  __(_   ___)  /
**               /  / /  )/  /  /  /
**              /  /_/  //  (__/  /
**             (____,__/(_____(__/
** ..
**
**   This file ('|utl.h|) provide the following basic elements:
**
**  .[Logging]        To print logging traces during program execution.
**                    It offers multilevel logging similar to '|log4j|
**                    but limited to files.
**                    Included are functions to implement unit tests.
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
**   [Text buffers]
**   [Queues]
**   [Stacks]
**   [Sorted Array]
**   [String match]
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
**  and add it to your project.
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
int utlEmptyFun(void) {return 0;}
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
  
utl_extern(int utlZero, = 0);

typedef int (*utl_cmp_t)(const void *a, const void *b);
typedef int (*utl_del_t)(const void *a);


/* .% Assumptions (static assertions)
** ==================================
**
** http://www.drdobbs.com/compile-time-assertions/184401873
*/

#define utl_assum1(e,l) void utl_assumption_##l ( char utl_assumption[(e)?1:-1]);
#define utl_assum0(e,l) utl_assum1(e,l)
#define utlAssume(e)    utl_assum0(e,__LINE__)
#define utlAssert       assert


utlAssume(sizeof(uint32_t) == 4);


#ifndef UTL_NOTRYCATCH

/* .% Exceptions
** ============
**   Exceptions can be very useful when dealing with error conditions is so
** complicate that the logic of errors handling obscures the logic of the 
** program itself.
**   If an error happens in the '|try| section, you can '|throw()| an exception
** (an error condition) and the control goes last to the '|catch| section
** where the proper actions can be taken.
**
**   Simple implementation of try/catch.
** .{{ c
**   tryenv env = NULL;  // define an "environment" for trying and set to NULL
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
**  This comes useful when you throw an exception from a called function.
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
**   })
**
** .}}
*/ 


typedef struct utl_env_s { 
  uint32_t flags;
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
                } while(0);

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
**                   fsmExit;  // exit from the FSM
**        }
**
**        case y : { ...
**                   if (c == 1) fsmGoto(x);
**                   fsmGoto(z);
**        }
**      })
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
                        default: utl_fsm_next = fsmEND; \
                }} while (utlZero);
                         
#define fsmGoto(x)  if (!utlZero) {utl_fsm_next = (x); break;} else (utlZero<<=1)
#define fsmRestart  fsmGoto(fsmSTART)
#define fsmExit     fsmGoto(fsmEND)

#endif

/* .% Logging
** ==========
**
*/

#define log_D 8
#define log_I 7
#define log_M 6
#define log_W 5
#define log_E 4
#define log_C 3
#define log_A 2
#define log_F 1
#define log_T 0


#define log_X (log_D + 1)
#define log_L (log_D + 2)


/* Logging functions are available unless the symbol '{=UTL_NOLOGGING}
** has been defined before including '|utl.h|.
*/

#ifndef UTL_NOLOGGING

#define UTL_LOG_OUT 0x80    /* use stdout */
#define UTL_LOG_ERR 0x40    /* use stderr */

#define UTL_LOG_SKIP 0x01   /* skip tests */
#define UTL_LOG_SKP0 0x02
#define UTL_LOG_RES  0x04

typedef struct {
  FILE          *file;
  unsigned short rot;
  unsigned char  level;
  unsigned char  last;
  unsigned char  flags;
  unsigned char  ok;
  unsigned char  ko;
  unsigned char  skp;
} utl_log_s, *log_t;

#define utl_log_stdout_init {NULL, 0, log_W, log_W, UTL_LOG_OUT,0,0,0}
utl_extern(utl_log_s utl_log_stdout , = utl_log_stdout_init);
utl_extern(log_t logStdout, = &utl_log_stdout);

#define utl_log_stderr_init {NULL, 0, log_W, log_W, UTL_LOG_ERR,0,0,0}
utl_extern(utl_log_s utl_log_stderr , = utl_log_stderr_init);
utl_extern(log_t logStderr, = &utl_log_stderr);

#define logNull NULL

utl_extern(log_t utlLog , = &utl_log_stderr);

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

                                        /* 0   1   2   3   4   5   6   7   8   9   10 */
                                        /* 0   4   8   12  16  20  24  28  32  36  40 */
utl_extern(char const utl_log_abbrev[], = "TST FTL ALT CRT ERR WRN MSG INF DBG OFF LOG ");

#ifdef UTL_LIB
/* Assume that log_L is the last level in utl_log_abbrev */
utlAssume( (log_L +1) == ((sizeof(utl_log_abbrev)-1)>>2));
#endif

int   utl_log_level(log_t lg);
int   utl_log_chrlevel(char *l);
int   logLevel(log_t lg, char *lv); 
int   logLevelEnv(log_t lg, char *var, char *level);

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
**    Log files can be opened in "write" or "append" mode as any normal file 
** using the '{=logOpen()} function.
** For example:
** .v  
**   log_t lgr = NULL;
**   lgr=logOpen(lgr,"file1.log","w") // Delete old log file and create a new one
**   ...
**   lgr=logClose(lgr);
**   ...
**   lgr=logOpen(lgr,"file1.log","a") // Append to previous log file
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

log_t utl_logOpen(char *fname, char *mode);
log_t utl_logClose(log_t lg);
void utl_log_write(log_t lg,int lv, int tstamp, char *format, ...);

#define logFile(l) utl_logFile(l)
#define logLevel(lg,lv)      utl_logLevel(lg,lv)
#define logLevelEnv(lg,v,l)  utl_logLevelEnv(lg,v,l)

#define logIf(lg,lc) utl_log_if(lg,utl_log_chrlevel(lc))

#define utl_log_if(lg,lv) if ((lv) > utl_log_level(lg)) {} else
          
#define logDebug(lg, ...)      utl_log_write(lg, log_D, 1, __VA_ARGS__)
#define logInfo(lg, ...)       utl_log_write(lg, log_I, 1, __VA_ARGS__)
#define logMessage(lg, ...)    utl_log_write(lg, log_M, 1, __VA_ARGS__)
#define logWarn(lg, ...)       utl_log_write(lg, log_W, 1, __VA_ARGS__)
#define logError(lg, ...)      utl_log_write(lg, log_E, 1, __VA_ARGS__)
#define logCritical(lg, ...)   utl_log_write(lg, log_C, 1, __VA_ARGS__)
#define logAlert(lg, ...)      utl_log_write(lg, log_A, 1, __VA_ARGS__)
#define logFatal(lg, ...)      utl_log_write(lg, log_F, 1, __VA_ARGS__)

#define logdbg(...)  logDebug(logStderr,__VA_ARGS__)
#define logNdbg(...) 

#define logClock(lg,x)         logClockStart(lg); \
                                 x \
                               logClockStop(lg); 

#define logClockStart(lg)      do { \
                                 clock_t utl_clk = clock(); \
                                 log_t utl_l=lg;
                                 
#define logClockStop             logAlert(utl_l,"CLK  %ld (s/%ld) %s:%d",clock()-utl_clk, CLOCKS_PER_SEC,__FILE__,__LINE__);\
                               } while (utlZero)


#define logContinue(lg, ...)   utl_log_write(lg, -1, 0, __VA_ARGS__)

#define logAssert(lg,e)        utl_log_assert(lg, e, #e, __FILE__, __LINE__)



#define logTest(lg,e)          utl_log_test(lg, e, #e, __FILE__, __LINE__); 

#define logTestPlan(lg, s)      for((lg? utl_log_write(lg, log_T, 1, "PLN  %s (%s:%d)",s, __FILE__, __LINE__),\
                                    lg->ok=lg->ko=lg->skp =0, lg->flags |= UTL_LOG_SKP0\
                                  : 1); lg->flags & UTL_LOG_SKP0;lg->flags &= ~UTL_LOG_SKP0,logTestStat(lg)) 

#define logTestNote(lg, ...)   utl_log_write(lg, log_T, 0, "     "__VA_ARGS__)

#define logTestFailNote(lg, ...)  (lg && !(lg->flags & UTL_LOG_RES)? logTestNote(lg, __VA_ARGS__):0)

#define logTestCode(lg)         if (!(lg && !(lg->flags & UTL_LOG_SKIP))) {} else 

#define logTestStat(lg)        (lg? utl_log_write(lg, log_T, 1, "RES  KO:%3d  OK:%3d  SKIP:%3d  TOT:%3d  (%s:%d)", \
                                    lg->ko, lg->ok, lg->skp, lg->ko + lg->ok + lg->skp,__FILE__, __LINE__) \
                                  : 0)

#define logTestSkip(lg,s,e)    for ( utl_log_testskip_init(lg, e, s,  __LINE__) ; \
                                     utl_log_testskip_check(lg) ;\
                                     utl_log_testskip_end(lg,  __LINE__) )

#define log_testxxx(t1,t2,lg,s,e,r,o,t)  \
      if (lg && !(lg->flags & UTL_LOG_SKIP)) { \
        t1 utl_exp = (e); t1 utl_ret = (r); \
        if (!utl_log_test(lg, t , s, __FILE__, __LINE__))  \
          logTestNote(lg,"Expected (" #t1 ") " #o " "#t2" got "#t2,utl_exp,utl_ret); \
      } \
      else utl_log_test(lg, 1, s, __FILE__, __LINE__)

#define log_testint(lg,s,e,r,o)    log_testxxx(int   ,%d,lg,s,e,r,o,(utl_ret o utl_exp))
#define log_testptr(lg,s,e,r,o)    log_testxxx(void *,%p,lg,s,e,r,o,(utl_ret o utl_exp))
#define log_testdbl(lg,s,e,r,o)    log_testxxx(double,%p,lg,s,e,r,o,(utl_ret o utl_exp))
#define log_teststr(lg,x,e,r,o,m)  log_testxxx(char *,%s,lg,x,e,r,o,(1 << (1+strcmp(utl_ret, utl_exp))) & m)
#define log_teststrn(lg,x,e,r,o,m,n) log_testxxx(char *,%s,lg,x,e,r,o,(1 << (1+strncmp(utl_ret, utl_exp,n))) & m)

#define logEQint(lg,e,r)       log_testint(lg,#e" == "#r,e,r, == )
#define logNEint(lg,e,r)       log_testint(lg,#e" != "#r,e,r, != )
#define logGTint(lg,e,r)       log_testint(lg,#e" >  "#r,e,r, >  )
#define logGEint(lg,e,r)       log_testint(lg,#e" >= "#r,e,r, >= )
#define logLTint(lg,e,r)       log_testint(lg,#e" <  "#r,e,r, <  )
#define logLEint(lg,e,r)       log_testint(lg,#e" <= "#r,e,r, <= )

#define logEQdbl(lg,e,r)       log_testdbl(lg,#e" == "#r,e,r, == )
#define logNEdbl(lg,e,r)       log_testdbl(lg,#e" != "#r,e,r, != )
#define logGTdbl(lg,e,r)       log_testdbl(lg,#e" >  "#r,e,r, >  )
#define logGEdbl(lg,e,r)       log_testdbl(lg,#e" >= "#r,e,r, >= )
#define logLTdbl(lg,e,r)       log_testdbl(lg,#e" <  "#r,e,r, <  )
#define logLEdbl(lg,e,r)       log_testdbl(lg,#e" <= "#r,e,r, <= )
//#define logEPSdbl(lg,s,d,e,r)    logLTdbl(lg,s,d,fabs((e)-(r)))

#define logEQptr(lg,e,r)       log_testptr(lg,#e" == "#r,e,r, == )
#define logNEptr(lg,e,r)       log_testptr(lg,#e" != "#r,e,r, != )
#define logGTptr(lg,e,r)       log_testptr(lg,#e" >  "#r,e,r, >  )
#define logGEptr(lg,e,r)       log_testptr(lg,#e" >= "#r,e,r, >= )
#define logLTptr(lg,e,r)       log_testptr(lg,#e" <  "#r,e,r, <  )
#define logLEptr(lg,e,r)       log_testptr(lg,#e" <= "#r,e,r, <= )

#define logNULL(lg,r)          log_testptr(lg, #r" is NULL",NULL,r, == )
#define logNNULL(lg,r)         log_testptr(lg, #r" is not NULL",NULL,r, != )

#define logEQstr(lg,e,r)       log_teststr(lg,#e" == "#r,e,r, ==, 2 )
#define logNEstr(lg,e,r)       log_teststr(lg,#e" != "#r,e,r, !=, 5 )
#define logGTstr(lg,e,r)       log_teststr(lg,#e" >  "#r,e,r, > , 4 )
#define logGEstr(lg,e,r)       log_teststr(lg,#e" >= "#r,e,r, >=, 6 )
#define logLTstr(lg,e,r)       log_teststr(lg,#e" <  "#r,e,r, < , 1 )
#define logLEstr(lg,e,r)       log_teststr(lg,#e" <= "#r,e,r, <=, 3 )

#define logEQstrn(lg,e,r,n)    log_teststrn(lg,#e" == "#r,e,r, ==, 2 ,n)
#define logNEstrn(lg,e,r,n)    log_teststrn(lg,#e" != "#r,e,r, !=, 5 ,n)
#define logGTstrn(lg,e,r,n)    log_teststrn(lg,#e" >  "#r,e,r, > , 4 ,n)
#define logGEstrn(lg,e,r,n)    log_teststrn(lg,#e" >= "#r,e,r, >=, 6 ,n)
#define logLTstrn(lg,e,r,n)    log_teststrn(lg,#e" <  "#r,e,r, < , 1 ,n)
#define logLEstrn(lg,e,r,n)    log_teststrn(lg,#e" <= "#r,e,r, <=, 3 ,n)


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
int   utl_log_level(log_t lg) { return (int)(lg ? lg->level : log_X) ; }

FILE *utl_logFile(log_t lg)
{
  if (!lg) return NULL;
  if (lg->flags & UTL_LOG_ERR) return stderr;
  if (lg->flags & UTL_LOG_OUT) return stdout;
  return lg->file;
}

int   utl_log_chrlevel(char *l) {
  int i=0;
  char c = (l && *l)? toupper(l[0]) : 'W';
  
  while (utl_log_abbrev[i] != ' ' && utl_log_abbrev[i] != c) i+=4;
  i = (i <= 4*log_D) ? (i>> 2) : log_W;
  return i;
}


int utl_logLevel(log_t lg, char *lv) 
{
  if (!lg) return log_X;
  
  if (lv && lv[0] && lv[0] != '?')
      lg->level = utl_log_chrlevel(lv);
  return utl_log_level(lg);  
}

int utl_logLevelEnv(log_t lg, char *var, char *level)
{
  char *lvl_str;
  
  lvl_str=getenv(var);
  if (!lvl_str) lvl_str = level;
  return utl_logLevel(lg,lvl_str);
}

log_t utl_logOpen(char *fname, char *mode)
{
  char md[4];
  log_t lg = logStderr;
  FILE *f = NULL;
  
  if (fname) {
    md[0] = mode[0]; md[1] = '+'; md[2] = '\0';
    if (md[0] != 'a' && md[0] != 'w') md[0] = 'a'; 
    f = fopen(fname,md);
  }

  if (f) {
    lg = malloc(sizeof(utl_log_s));
    if (lg) { 
      lg->flags = 0;   lg->rot = 0;
      lg->file = f;    lg->level = log_L;
      utl_log_write(lg,log_L, 1, "%s \"%s\"", (md[0] == 'a') ? "ADDEDTO" : "CREATED",fname); 
      lg->level = log_W;
    }
  }
  if (f && !lg) fclose(f);
  return lg;
}

log_t utl_log_close(log_t lg)
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
static void utl_log_rotate(log_t lg)
{
   /* TODO:
    */ 
}

void utl_log_write(log_t lg, int lv, int tstamp, char *format, ...)
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
  
  if (lv == -1) lv = lg->last;
  
  lg_lv = lg->level;
  lv = lv & 0x0F;
  lg->last = lv;
  if( lv <= lg_lv) {
    if (tstamp) {
      time(&t);
      strftime(tstr,32,"%Y-%m-%d %X",localtime(&t));
    } else {
      strcpy(tstr,"                   ");          
    }
    fprintf(f, "%s %.4s", tstr, utl_log_abbrev+(lv<<2));
    va_start(args, format);  vfprintf(f,format, args);  va_end(args);
    fputc('\n',f);
    fflush(f);
    if (lg->rot >0) utl_log_rotate(lg);
  }    
}

void utl_log_assert(log_t lg,int e,char *estr, char *file,int line)
{ 
  if (!e) {
    logFatal(lg,"Assertion failed:  %s, file %s, line %d", estr, file, line);
#ifndef NDEBUG
  abort();
#endif
  }
}

int utl_log_test(log_t lg,int e, char *s, char *file, int line)
{ 
  char *msg = utlEmptyString;
  int skip = 0;
  if (lg) {
    lg->flags &= ~UTL_LOG_RES;
    skip = lg->flags & UTL_LOG_SKIP;
    if (!skip) {
      if (e) {msg = "OK  "; lg->ok++; }
      else   {msg = "KO  "; lg->ko++; }
    }
    else  {msg = "SKP "; lg->skp++;}
    
    if (e) lg->flags |=  UTL_LOG_RES;
    
    utl_log_write(lg, log_T, 1, "%s %s (:%d)",msg,s, line);
  }
  return e;
}

void utl_log_testskip_init(log_t lg, int e, char *estr,int line)
{
  if (lg) {
    if (e) {
      utl_log_write(lg, log_T, 1, "SKP  REASON: \"%s\" (:%d)",estr, line);
      lg->flags |= UTL_LOG_SKIP;
    }
    lg->flags |= UTL_LOG_SKP0;
  }
}

int utl_log_testskip_check(log_t lg)
{
  return lg && (lg->flags & UTL_LOG_SKP0);
}

void utl_log_testskip_end(log_t lg, int line)
{
  if (lg) {
    if (lg->flags & UTL_LOG_SKIP)
      utl_log_write(lg, log_T, 1, "SKP  END (:%d)", line);
    lg->flags &= ~(UTL_LOG_SKP0|UTL_LOG_SKIP);
  }
}

                 
#endif  /*- UTL_LIB */

#else   /*- UTL_NOLOGGING */

#define logLevel(lg,lv)       log_W
#define logLevelEnv(lg,v,l)   log_W     
#define logDebug(lg, ...)     (utlZero<<=1)
#define logInfo(lg, ...)      (utlZero<<=1)
#define logMessage(lg, ...)   (utlZero<<=1)
#define logWarn(lg, ...)      (utlZero<<=1)
#define logError(lg, ...)     (utlZero<<=1)
#define logCritical(lg, ...)  (utlZero<<=1)
#define logAlarm(lg, ...)     (utlZero<<=1)
#define logFatal(lg, ...)     (utlZero<<=1)

#define logNdbg(...)           (utlZero<<=1)
#define logContinue(lg, ...)  (utlZero<<=1)

#define logAssert(lg,e)       (utlZero<<=1)

#define logIf(lg,lv) if (!utlZero) (utlZero<<=1) ; else

#define logOpen(f,m)    NULL
#define logClose(lg)    NULL

typedef void *log_t;

#define logFile(x) NULL
#define logStdout  NULL
#define logStderr  NULL

#define logClock(lg,x)         x

#define logTest(lg,e)           (utlZero<<=1)
#define logTestPlan(lg, s)       
#define logTestNote(lg, ...)     (utlZero<<=1)
#define logTestFailNote(lg, ...) (utlZero<<=1) 
#define logTestCode(lg)       
#define logTestStat(lg)          (utlZero<<=1)
#define logTestSkip(lg,s,e)    

#define log_testint(lg,s,e,r,o) (utlZero<<=1)
#define log_testptr(lg,s,e,r,o) (utlZero<<=1)

#define logEQInt(lg,s,e,r)  (utlZero<<=1)
#define logNEInt(lg,s,e,r)  (utlZero<<=1)
#define logGTInt(lg,s,e,r)  (utlZero<<=1)
#define logGEInt(lg,s,e,r)  (utlZero<<=1)
#define logLTInt(lg,s,e,r)  (utlZero<<=1)
#define logLEInt(lg,s,e,r)  (utlZero<<=1)
                               
#define logEQPtr(lg,s,e,r)  (utlZero<<=1)
#define logNEPtr(lg,s,e,r)  (utlZero<<=1)
#define logGTPtr(lg,s,e,r)  (utlZero<<=1)
#define logGEPtr(lg,s,e,r)  (utlZero<<=1)
#define logLTPtr(lg,s,e,r)  (utlZero<<=1)
#define logLEPtr(lg,s,e,r)  (utlZero<<=1)
                                
#define logNULL(lg,s,e)     (utlZero<<=1)
#define logNNULL(lg,s,e)    (utlZero<<=1)

#endif /*- UTL_NOLOGGING */

#ifdef NDEBUG
#undef logDebug
#define logDebug(lg,...) (utlZero<<=1)
#endif  /*- NDEBUG */

#define logNDebug(lg,...) (utlZero<<=1)

#ifdef UTL_NOLOGCLOCK
#undef  logClock
#define logClock(lg,x)         x
#endif
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
    logError(utlLog,"Invalid or double freed %p (%u %s:%d)",p->data, \
                                               utl_mem_allocated, file, line);     
    return utlMemInvalid; 
  }
  if (memcmp(p->data+p->size,END_CHK,4)) {
    logError(utlLog,"Boundary overflow detected %p [%d] (%u %s:%d)", \
                              p->data, p->size, utl_mem_allocated, file, line); 
    return utlMemOverflow;
  }
  logInfo(utlLog,"Valid pointer %p (%u %s:%d)",ptr, utl_mem_allocated, file, line); 
  return utlMemValid; 
}

void *utl_malloc(size_t size, char *file, int line )
{
  utl_mem_t *p;
  
  if (size == 0) logWarn(utlLog,"Shouldn't allocate 0 bytes (%u %s:%d)", \
                                                utl_mem_allocated, file, line);
  p = malloc(sizeof(utl_mem_t) +size);
  if (p == NULL) {
    logCritical(utlLog,"Out of Memory (%u %s:%d)",utl_mem_allocated, file, line);
    return NULL;
  }
  p->size = size;
  memcpy(p->chk,BEG_CHK,4);
  memcpy(p->data+p->size,END_CHK,4);
  utl_mem_allocated += size;
  logInfo(utlLog,"alloc %p [%d] (%u %s:%d)",p->data,size,utl_mem_allocated,file,line);
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
    case utlMemNull  :    logWarn(utlLog,"free NULL (%u %s:%d)", 
                                                utl_mem_allocated, file, line);
                          break;
                          
    case utlMemOverflow : logWarn(utlLog, "Freeing an overflown block  (%u %s:%d)", 
                                                           utl_mem_allocated, file, line);
    case utlMemValid :    p = utl_mem(ptr); 
                          memcpy(p->chk,CLR_CHK,4);
                          utl_mem_allocated -= p->size;
                          if (p->size == 0)
                            logWarn(utlLog,"Freeing a block of 0 bytes (%u %s:%d)", 
                                                utl_mem_allocated, file, line);

                          logInfo(utlLog,"free %p [%d] (%u %s %d)", ptr, 
                                    p?p->size:0,utl_mem_allocated, file, line);
                          free(p);
                          break;
                          
    case utlMemInvalid :  logError(utlLog,"free an invalid pointer! (%u %s:%d)", \
                                                utl_mem_allocated, file, line);
                          break;
  }
}

void *utl_realloc(void *ptr, size_t size, char *file, int line)
{
  utl_mem_t *p;
  
  if (size == 0) {
    logWarn(utlLog,"realloc() used as free() %p -> [0] (%u %s:%d)",ptr,utl_mem_allocated, file, line);
    utl_free(ptr,file,line); 
  } 
  else {
    switch (utl_check(ptr,file,line)) {
      case utlMemNull   : logWarn(utlLog,"realloc() used as malloc() (%u %s:%d)", \
                                             utl_mem_allocated, file, line);
                          return utl_malloc(size,file,line);
                        
      case utlMemValid  : p = utl_mem(ptr); 
                          p = realloc(p,sizeof(utl_mem_t) + size); 
                          if (p == NULL) {
                            logCritical(utlLog,"Out of Memory (%u %s:%d)", \
                                             utl_mem_allocated, file, line);
                            return NULL;
                          }
                          utl_mem_allocated -= p->size;
                          utl_mem_allocated += size; 
                          logInfo(utlLog,"realloc %p [%d] -> %p [%d] (%u %s:%d)", \
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
    logWarn(utlLog,"strdup NULL (%u %s:%d)", utl_mem_allocated, file, line);
    return NULL;
  }
  size = strlen(ptr)+1;

  dest = utl_malloc(size,file,line);
  if (dest) memcpy(dest,ptr,size);
  logInfo(utlLog,"strdup %p [%d] -> %p (%u %s:%d)", ptr, size, dest, \
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

#ifdef strdup
#undef strdup
#endif
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

#define utlMemCheck(p)     utlMemValid
#define utlMemAllocated    0
#define utlMemValidate(p) (p)

#endif /* UTL_MEMCHECK */

#ifndef UTL_NOADT

/** VECTORS **/
typedef struct vec_s {
  uint32_t  max;
  uint32_t  cnt;
  uint32_t  esz;
  uint32_t  first;
  uint32_t  last; 
  uint32_t  flg; 
  utl_cmp_t cmp;
  utl_del_t del;
  void     *vec;
  void     *val; /* will point to the elm field */
  uint32_t  elm; /* just to get the pointer */
} *vec_t;

#define UTL_VEC_SORTED  1UL

vec_t utl_vec_new(int esz, utl_cmp_t , utl_del_t);
#define vecNew(...) utl_vec_new(utl_vec_new_ARGS(__VA_ARGS__))

#define utl_vec_new_ARGS(...)    sizeof(utl_ARGS0(__VA_ARGS__, NULL, NULL, NULL)),\
                                utl_ARGS1(__VA_ARGS__, NULL, NULL, NULL), \
                                utl_ARGS2(__VA_ARGS__, NULL, NULL, NULL)
#define utl_ARGS0(x, ...) x
#define utl_ARGS1(x, y, ...) y
#define utl_ARGS2(x, y, z, ...) z

vec_t utl_vec_free(vec_t v);
#define vecFree utl_vec_free

void *utl_vec_set(vec_t v, uint32_t i, void *e);
#define vecSetPtr utl_vec_set
#define vecSet(ty,v,i,e)   (*((ty *)(v)->val) = (e), utl_vec_set(v,i,(v)->val))

void *utl_vec_fill(vec_t v, uint32_t j, uint32_t i, void *e);
#define vecFillPtr utl_vec_fill
#define vecFill(ty,v,j,i,e)  (*((ty *)(v)->val) = (e), utl_vec_fill(v,j,i,(v)->val))

void *utl_vec_add(vec_t v, void *e); 
#define vecAddPtr  utl_vec_add
#define vecAdd(ty,v,e)  (*((ty *)(v)->val) = (e), utl_vec_add(v,(v)->val))

void *utl_vec_ins(vec_t v, uint32_t n, uint32_t l,void *e);
#define vecInsPtr(v,i,e) utl_vec_ins(v,i,1,e);
#define vecIns(ty,v,i,e) (*((ty *)(v)->val) = (e), utl_vec_ins(v,i,1,(v)->val))
#define vecInsGap(v,i,n) utl_vec_ins(v,i,n,NULL)

int utl_vec_del(vec_t v, uint32_t i, uint32_t l);
#define vecDel(v,i) utl_vec_del(v,i,1)

#define utl_vec_valid_ndx(v, i)  ((v) && (v)->vec && ((i) < v->cnt))
#define utl_vec_expand(v, i) ((utl_vec_valid_ndx(v,i)) || (utl_vec_resize(v,i) >= 0))
#define utl_vec_getptr(v,i)  (((char *)((v)->vec)) + ((i)*((v)->esz)))
#define utl_vec_cpy(v,i,e)   memcpy(utl_vec_getptr(v,i), e, (v)->esz)

void *utl_vec_get(vec_t v, uint32_t  i);
#define vecGetPtr utl_vec_get
#define vecGet(ty,v,i,d) (utl_vec_valid_ndx(v,i) ? *((ty *)(utl_vec_getptr(v,i))) : d)

int utl_vec_resize(vec_t v, uint32_t n);
#define vecResize utl_vec_resize

uint32_t utl_vec_count(vec_t v);
#define vecCount     utl_vec_count

uint32_t utl_vec_max(vec_t v);
#define vecMax     utl_vec_max

uint32_t utl_vec_esz(vec_t v);
#define vecEsz     utl_vec_esz

void  *utl_vec_vec(vec_t v);
#define vec(ty, v) ((ty *)utl_vec_vec(v))

int utl_vec_sort(vec_t v, utl_cmp_t c) ;
#define vecSorted(...) utl_vec_sort(utl_vecSorted_ARGS(__VA_ARGS__)) 
#define utl_vecSorted_ARGS(...) utl_ARGS0(__VA_ARGS__, NULL), utl_ARGS1(__VA_ARGS__, NULL)

#define vecUnsorted(v) if (!v) {} else {v->cmp = NULL; v->flg &= ~UTL_VEC_SORTED;}

void *utl_vec_search(vec_t v, void *e);
#define vecSearchPtr(v,e) utl_vec_search(v,e)
#define vecSearch(ty,v,e)   (*((ty *)(v)->val) = (e), utl_vec_search(v,(v)->val))

#ifdef UTL_LIB

#if 0
/*
** Integer log base 2 of 32 bits integer values.
**   llog2(0) == llog2(1) == 0
*/
static short utl_llog2(uint32_t n)
{
#if defined(__GNUC__)
  return (31 - __builtin_clzl(n));
#else
  register uint32_t l = 0;
  register uint32_t x = n;
  if (x & 0xFFFF0000) {l |= 16; x >>= 16;} /* 11111111111111110000000000000000 */
  if (x & 0xFF00)     {l |= 8;  x >>= 8 ;} /* 1111111100000000*/
  if (x & 0xF0)       {l |= 4;  x >>= 4 ;} /* 11110000*/
  if (x & 0xC)        {l |= 2;  x >>= 2 ;} /* 1100 */
  if (x & 2)          {l |= 1;  }          /* 10 */
  return l;
#endif
}
#endif

vec_t utl_vec_new(int esz, utl_cmp_t cmp, utl_del_t del)
{
  vec_t v;
  v = malloc(sizeof(struct vec_s)+(esz-sizeof(uint32_t)));
  if (v) {
    v->max = 0;    v->cnt = 0;
    v->first = 0;  v->last = 0;
    v->esz = esz;  v->vec = NULL; v->flg = 0;
    v->cmp = cmp;  v->del = del;
    v->val = &(v->elm);
  }
  return v;
}

vec_t utl_vec_free(vec_t v)
{
  if (v) {
    if (v->vec) free(v->vec);
    v->max = 0;    v->cnt = 0;
    v->first = 0;  v->last = 0;
    v->esz = 0;    v->vec = NULL;
    free(v);
  }
  return NULL;
}

uint32_t utl_vec_count(vec_t v) { return v? v->cnt : 0; }
uint32_t utl_vec_max(vec_t v)   { return v? v->max : 0; }
uint32_t utl_vec_esz(vec_t v)   { return v? v->esz : 0; }
void    *utl_vec_vec(vec_t v)   { return v? v->vec : NULL; } 

int utl_vec_resize(vec_t v, uint32_t n)
{
  uint32_t new_max = 1;
  char *new_vec = NULL;
  if (!v) return 0;

  while (new_max <= n) new_max *= 2;
  
  if (new_max != v->max) {
    new_vec = realloc(v->vec, new_max * v->esz);
    if (!new_vec) return -1;
    v->vec = new_vec;
    v->max = new_max;
    if (v->cnt > v->max) v->cnt = v->max; /* in case of shrinking */
  }
  return v->cnt;
}

void *utl_vec_set(vec_t v, uint32_t  i, void *e)
{
  void *p = NULL; 
  if (utl_vec_expand(v,i)) {
    p = utl_vec_getptr(v,i);
    if (e) memcpy(p, e, v->esz);
    if (i >= v->cnt) v->cnt = i+1;
    v->flg &= ~UTL_VEC_SORTED;
  }
  return p;
}

void *utl_vec_fill(vec_t v, uint32_t j, uint32_t i, void *e)
{
  char *p = NULL;
  void *pos = NULL;
  uint32_t sz;
  
  if ((j > i) || !utl_vec_expand(v,i)) return NULL;
  sz = v->esz;
  pos = utl_vec_getptr(v,j);
  p = pos;  
  while ( j<=i) {
    memcpy(p,e, sz);
    j++, p += sz;
  }
  if (i >= v->cnt) v->cnt = i+1;
  v->flg &= ~UTL_VEC_SORTED;
  return pos;
}

void *utl_vec_get(vec_t v, uint32_t i)
{
  if (!utl_vec_valid_ndx(v,i)) return NULL;
  return utl_vec_getptr(v,i);
}

void *utl_vec_top(vec_t v)
{
  if (!v || !v->cnt) return NULL;
  return ((char *)(v->vec)) + ((v->cnt-1)*v->esz);
}

void utl_vec_pop(vec_t v) { if (v && v->cnt) v->cnt -= 1; }

void *utl_vec_add(vec_t v, void *e) 
{
  void *pos = NULL;
  
  if (!v) return 0;
  if (v->cmp) {
    pos = vecSearchPtr(v,e);
    logNDebug(logStderr,"Found? %p n:%d c:%d",pos, v->first, v->cnt);
    if (pos) {
      memcpy(pos,e,v->esz);
    }
    else {
      pos = utl_vec_getptr(v,v->first);
      vecInsPtr(v,v->first,e);
    }
    v->flg |= UTL_VEC_SORTED;
  }
  else  {
    pos = utl_vec_getptr(v,v->cnt);
    utl_vec_set(v,v->cnt,e);
  }
  return pos;
}

void *utl_vec_ins(vec_t v, uint32_t i, uint32_t l,void *e)
{
  char *b = NULL;
  uint32_t sz;
  
  if (!v) return b;
  if (l == 0) return b;
  if (i > v->cnt) i = v->cnt;

  logNDebug(logStderr,"Inserting before %d count:%d len:%d",i,v->cnt,l);
  if (utl_vec_expand(v,v->cnt+l+1)) {
    sz = v->esz;
    b = v->vec+i*sz;
    memmove(b+(l*sz), b, (v->cnt-i)*sz);
    if (e)  memcpy(b,e,(l*sz));
    else memset(b,0x00,l*sz);
    v->cnt += l;
    v->flg &= ~UTL_VEC_SORTED;
  }
  logNDebug(logStderr,"Inserting after %d count:%d len:%d",i,v->cnt,l);
  return b;
}

int utl_vec_del(vec_t v, uint32_t i,uint32_t l)
{
  uint32_t cnt;
  char *b;
  uint32_t sz;
  
  cnt = vecCount(v);
  if (i < cnt) {
  
    if (i+l > cnt) l = cnt-i-1;
  
    if (i < cnt-l) {
      sz = v->esz;
      b = v->vec+i*sz;
      memmove(b,b+l*sz,(cnt-i-l)*sz);
    }
    v->cnt -= l;
  }
  return v->cnt;
}

int utl_vec_sort(vec_t v, utl_cmp_t c) 
{
  if (c) {
    v->cmp = c;
    qsort(v->vec, v->cnt, v->esz, v->cmp);
    v->flg |= UTL_VEC_SORTED;
  }
  return v->flg & UTL_VEC_SORTED;
}

void *utl_vec_search(vec_t v, void *e)
{
  uint32_t ndx =0;
  int res = 1;
  void *ret = NULL;
  int a,b;
  
  if (vecCount(v) == 0 || !v->cmp) return NULL;
  
  if (!(v->flg & UTL_VEC_SORTED)) vecSorted(v,v->cmp);
  
  a = 0;
  b = v->cnt-1; 
  while (res && a <= b) {
    ndx = (a + b) / 2;
    ret = vecGetPtr(v,ndx);
    res = v->cmp(ret,e);
    logNDebug(logStderr,"ndx: %d (cmp=%d) [%d,%d]",ndx,res,a,b);
    if      (res > 0)  b = ndx -1; 
    else if (res < 0)  a = ndx +1;
  } 
  if (res) ret = NULL;
  v->first = ndx + (res < 0);
  return ret;
}

#endif  /* UTL_LIB */

/** STACKS **/
 
#define stk_t             vec_t
#define stkNew(ty)        vecNew(ty)
#define stkFree           vecFree
#define stkCount(s)       vecCount(s)
#define stkEmpty(s)     (!vecCount(s))
#define stkPushPtr(s,e)   vecAddPtr(s,e)
#define stkPush(ty,s,e)   vecAdd(ty,s,e)
#define stkTopPtr(s)      utl_vec_top(s)
#define stkTop(ty,s,d)   (vecCount(s)? *((ty *)utl_vec_top(s)) : d)
#define stkPop(s)         utl_vec_pop(s)

/**  QUEUE **/
#define que_t vec_t

#define queDelFirst(q)  utl_que_del(q, 'F')
#define queDelLast(q)   utl_que_del(q, 'B')
#define queDel          queDelFirst
int utl_que_del(que_t qu, char where);

#define queFirstPtr(q)    utl_que_get(q,'F')
#define queLastPtr(q)     utl_que_get(q,'B')
void *utl_que_get(que_t qu, char where);

#define queFirst(ty,q,d) (!(q) || queCount(q) == 0 ? d : *((ty *)queFirstPtr(q)))
#define queLast(ty,q,d)  (!(q) || queCount(q) == 0 ? d : *((ty *)queLastPtr(q)))

#define queAddFirstPtr(q,e)  utl_que_add_front(q,e)
#define queAddLastPtr(q,e)   utl_que_add_back(q,e)
#define queAddPtr queAddLastPtr

void *utl_que_add_front(que_t qu, void *e);
void *utl_que_add_back(que_t qu, void *e);

#define queAddFirst(ty,q,e)  (*((ty *)(q)->val) = (e), utl_que_add_front(q,(q)->val))
#define queAddLast(ty,q,e)   (*((ty *)(q)->val) = (e), utl_que_add_back(q,(q)->val))
#define queAdd queAddLast

#define queNew(ty) utl_que_new(sizeof(ty))
que_t utl_que_new(uint32_t esz);

#define queEmpty utl_que_empty
int utl_que_empty(que_t qu); 

#define queMax   vecMax
#define queCount vecCount
#define queEsz   vecEsz
#define queFree  vecFree

#define que_first(q) ((q)->first)
#define que_last(q)  ((q)->last)

#ifdef UTL_LIB

que_t utl_que_new(uint32_t esz)
{
  que_t qu = vecNew(esz);
  utlZero *= utl_vec_expand(qu,7); // to avoid warnings
  return qu;
}

int utl_que_empty(que_t qu) { return (qu == NULL || queCount(qu) == 0); }

int utl_que_expand(que_t qu)
{
  uint32_t new_max = 8;
  uint32_t old_max = 0;
  uint32_t n = 0;
  uint32_t sz;

  old_max = queMax(qu);
  if (queCount(qu) < old_max) return 1; /* there's room enough */
  
  new_max = old_max * 2;

  if (!utl_vec_expand(qu,new_max-1)) return 0; /* couldn't expand!! */
  
  /* Rearrange existing elements */
  n = qu->last;
  sz = queEsz(qu);
  if (n < old_max / 2) { /* move elements 0 .. n-1 to the new space */
     if (n > 0) memcpy(utl_vec_getptr(qu, old_max),  utl_vec_getptr(qu, 0),  n * sz);
     qu->last = old_max+n;
  } else { /* move elements n .. old_max-1 to the new space */
     n = old_max-n; /* Elements to move */
     memcpy(utl_vec_getptr(qu, new_max-n),  utl_vec_getptr(qu, qu->first),  n * sz);
     qu->first = new_max-n;
  }
  
  return 1;
}

void *utl_que_add_front(que_t qu, void *e)
{
  void *p = NULL;
  if (utl_que_expand(qu)) {
    qu->first = (qu->first + queMax(qu) - 1) % queMax(qu);
    p = utl_vec_getptr(qu,qu->first);
    memcpy(p,e,qu->esz);
    qu->cnt++;
  }
  return p;
}

void *utl_que_add_back(que_t qu, void *e)
{
  void *p = NULL;
  if (utl_que_expand(qu)) {
    p = utl_vec_getptr(qu,qu->last);
    memcpy(p,e,qu->esz);
    qu->last = (qu->last +1) % queMax(qu);
    qu->cnt++;
  }
  return p;
}

int utl_que_del(que_t qu, char where)
{
  if (!qu) return 0;
  if (queCount(qu) == 0) return 1;
  if (where == 'B') 
    qu->last = (qu->last + queMax(qu) - 1) % queMax(qu);
  else 
    qu->first = (qu->first +1) % queMax(qu);
  qu->cnt--;
  return 1;
}

void *utl_que_get(que_t qu, char where)
{
  uint32_t n;
  if (queCount(qu) == 0) return NULL;
  if (where == 'B') 
    n = (qu->last + queMax(qu) - 1) % queMax(qu);
  else 
    n = qu->first;
    
  return utl_vec_getptr(qu, n);
}

#endif /* UTL_LIB */ 

/**  BUFFERS **/
#define buf_t vec_t

#define bufNew() utl_vec_new(1,NULL,NULL)
#define bufFree  utl_vec_free

char utl_buf_get(buf_t bf, uint32_t i);
#define bufGet   utl_buf_get

int utl_buf_set(buf_t bf, uint32_t i, char c);
#define bufSet   utl_buf_set

int utl_buf_add(buf_t bf, char c);
#define bufAdd   utl_buf_add

int utl_buf_addstr(buf_t bf, char *s);
#define bufAddStr  utl_buf_addstr

int utl_buf_ins(buf_t bf, uint32_t i, char c);
#define bufIns(b,i,c) utl_buf_ins(b,i,c)

int utl_buf_insstr(buf_t bf, uint32_t i, char *s);
#define bufInsStr(b,i,s) utl_buf_insstr(b,i,s)

#define bufResize utl_vec_resize

#define bufClr(bf) utl_buf_set(bf,0,'\0');

int utl_buf_fmt(buf_t bf, char *format, ...);
#define bufFormat utl_buf_fmt

#define bufLen vecCount
#define bufMax vecMax
#define bufStr(b) vec(char,b)

int utl_buf_addline(buf_t bf, FILE *f);
#define bufAddLine   utl_buf_addline
#define bufReadLine  utl_buf_addline

int utl_buf_addfile(buf_t bf, FILE *f);
#define bufAddFile   utl_buf_addfile
#define bufReadFile  utl_buf_addfile

#if !defined(UTL_HAS_SNPRINTF) && defined(_MSC_VER) && (_MSC_VER < 1800)
#define UTL_ADD_SNPRINTF
#define snprintf  c99_snprintf
#define vsnprintf c99_vsnprintf
#endif

#ifdef UTL_LIB
int utl_buf_set(buf_t bf, uint32_t i, char c)
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

char utl_buf_get(buf_t bf, uint32_t i)
{
  if (!bf) return '\0';
  if (i >= bf->cnt) return '\0';
  return ((char*)(bf->vec))[i];
}

int utl_buf_add(buf_t bf, char c)
{  return utl_buf_set(bf,bf->cnt,c); }

int utl_buf_addstr(buf_t bf, char *s)
{
  if (!bf) return 0;
  if (!s || !*s) return 1;
  
  while (*s) if (!utl_buf_set(bf,bf->cnt,*s++)) return 0;
  
  return utl_buf_add(bf,'\0');
}

/* A line in the file can be ended by '\r\n', '\n' or '\r'.
** The NEWLINE characters are discarded.
** The string in the buffer is terminated with '\n\0'. 
*/
int utl_buf_addline(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case '\r' : if ((c = fgetc(f)) != '\n') ungetc(c,f);
      case '\n' :
      case EOF  : c = EOF; utl_buf_add(bf,'\n'); break;
      default   : utl_buf_add(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

int utl_buf_addfile(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case EOF  : c = EOF; break;
      default   : utl_buf_add(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

static int utl_buf_insert(buf_t bf, uint32_t i, uint32_t l, char *s)
{
  int n = 1;
  int k;
  char *b;
  
  if (!bf) return 0;
  if (!s)  return 1;
  if (l == 0) l = strlen(s);
  if (l == 0) return 1;
  if (i > bf->cnt) i = bf->cnt;

  if (!utl_vec_expand(bf,bf->cnt+l+1)) return 0;
  
  b = bf->vec+i;
  k = bf->cnt;
  memmove(b+l, b, k-i);
  memcpy(b,s,l);
  utl_buf_set(bf, k+l, '\0');
  return n;
}

int utl_buf_insstr(buf_t bf, uint32_t i, char *s)
{ return utl_buf_insert(bf,i,0,s); }

int utl_buf_ins(buf_t bf, uint32_t i, char c)
{ return utl_buf_insert(bf,i,1,&c); }


/* {{ code from http://stackoverflow.com/questions/2915672 */
#ifdef UTL_ADD_SNPRINTF

inline int c99_snprintf(char* str, uint32_t size, const char* format, ...)
{
  int count;
  va_list ap;
  va_start(ap, format);
  count = c99_vsnprintf(str, size, format, ap);
  va_end(ap);
  return count;
}

inline int c99_vsnprintf(char* str, uint32_t size, const char* format, va_list ap)
{
  int count = -1;
  if (size != 0)   count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
  if (count == -1) count = _vscprintf(format, ap);
  return count;
}
#endif /* UTL_ADD_SNPRINTF */
/* }} */

int utl_buf_fmt(buf_t bf, char *format, ...)
{
  int count;
  int count2;
  va_list ap;

  if (!bf) return -1;
  
  va_start(ap, format);
  count = vsnprintf(NULL,0,format, ap);
  va_end(ap);
  utl_buf_set(bf,count,'\0'); /* ensure there's enough room */
  va_start(ap, format);
  count2 = vsnprintf(bufStr(bf),count+1,format, ap);
  va_end(ap);
  
  utlAssert(count == count2);
  
  return count2;
}
#endif /* UTL_LIB */

/****************************************/
#ifndef UTL_NOREX
/***
  expr -> term+ '|' term+
 term -> '!'? fact [?*+]? 
 fact -> '(' expr  ')' | '[' '^'? range+ ']' | '%' any | any
 range -> any '-' any | '\' any | any
*/

#define UTL_MAX_CAPT 10

int utl_match(char *pat, char *str, vec_t v);
int utl_match_len(vec_t v, int n);
char *utl_match_capt(vec_t v, int n);
int utl_match_tok(vec_t v);

#define utlMatch     utl_match
#define utlMatchLen  utl_match_len
#define utlMatchSub  utl_match_capt
#define utlMatchTok  utl_match_tok

#ifdef UTL_LIB 


#define utl_eox(c) ((c) <2)
#define utl_endpat(c) (utl_eox(c) || ((c) == '|') || ((c) == ')'))
static int utl_expr(char **ppat, char **pstr, vec_t v);

utlAssume(sizeof(uint32_t) == 2 * sizeof(uint16_t));

/* us a uin32_t as a stack */
static int utl_match_push(vec_t v, int n)
{
  int sp = -1;
  uint32_t stk;
  uint16_t *ndx;
  
  if (v) {
    ndx = (uint16_t *)&(v->last);
    sp = ndx[0]; stk = v->first;
    logNdbg("push stk: %08X sp: %d val: %X",v->first,ndx[0],n);
    n &= 0x0F; /* the numbers in the stack will be in the range 0-15 */
    if (sp < 8) {
      stk &= ~(0x0F << (4*sp));  /* clear old value */
      stk |=  (n    << (4*sp));  /* set new value */
      sp++;
      ndx[0] = sp; v->first = stk;
    }
  }  
  return sp;
}

static int utl_match_pop(vec_t v)
{
  int sp;
  uint32_t stk;
  uint16_t *ndx;
  int n = -1;  
  
  if (v) {
    ndx = (uint16_t *)&(v->last);
    sp = ndx[0]; stk = v->first;
    if (sp > 0) {
      sp--;
      n = (stk >> (4*sp)) & 0x0F;  /* isolate top value */
      ndx[0] = sp;
    }
    logNdbg("pop from stk: %08X sp: %d val: %X",v->first,ndx[0], n);
  }  
  return n;
}

static int utl_match_open(vec_t v,char *s)
{
  uint16_t *ndx;
  int n = -1;
  if (v) {
    ndx = (uint16_t *)&(v->last);
    n = ndx[1];
    logNdbg("expr( %p %d",v,n);
    if (n < UTL_MAX_CAPT) {
      vecSet(char *,v,n*2, s);
      vecSet(char *,v,n*2+1, s);
      utl_match_push(v,n);
      ndx[1] = ++n;
    }
  }
  return n;
}

static int utl_match_close(vec_t v, char *s)
{
  int n = -1;

  if (v) {
    n = utl_match_pop(v);
    logNdbg("expr) %p %d",v,n);
    if (n >= 0) vecSet(char *,v,n*2+1, s);
  }
  return n;
}


static int utl_uchr(char **s)
{ /* one day will do UTF-8 */
  int c = 0;
  if (s && *s) {
    c = **s; if (c) *s += 1;
  }
  return c;
}

static int utl_ccls(char **ppat, int c)
{
  int ret = 0;
  char *p = *ppat;
  int inv = 0;
  int cp = 0;
  int cp2 = 0;
  
  
  if (*p == '^') {inv = 1; p++; }

  while (!utl_eox(*p) && (*p != ']')) {
    logNdbg("ccls {%s}[%c]",p,c);
    cp = utl_uchr(&p);
    if (p[1] && (p[1] != ']')) {
      if (cp == '%') cp = utl_uchr(&p);
      if (*p == '-') {
        p++; cp2 = utl_uchr(&p);
        if (cp == '%' && p[1] && (p[1] != ']')) cp2 = utl_uchr(&p);
        logNdbg("range: [%d][%d]",cp,cp2);
      }
    }
    if (cp2) {
      if (cp2 < cp) {ret = cp; cp = cp2; cp2 = ret; ret = 0;}
      if (cp <= c && c <= cp2) {ret = 1; break; }
    }
    else if (c == cp) { ret = 1; break; }
    cp2 = 0;
  }
  
  while (!utl_eox(*p) && *p != ']') p++;
  if (*p == ']') p++;
  
  *ppat = p;
  ret = (!ret == inv);

  return ret;
}

static int utl_fact(char **ppat, char **pstr, vec_t v)
{
  int ret = 0;
  char *p = *ppat;
  char *s = *pstr;
  char *q = NULL;
  int c,cp;
  int inv = 0;
  int max = 1;
  
  p = *ppat;
  cp = *p;
  
  logNdbg("fact: [%s] [%s]",p,s);

  if (cp == '@') {max = 0x7FFFFFFF; cp = '%';}
  
  #define utl_cls(x,y,z) case x : inv = 1; case y : utl_isa(z)
  #define utl_isa(z)     while (c && (ret < max) && (!(z) == inv)) { \
                          ret++; q = s; c = utl_uchr(&s); \
                         } s = q;
  if (cp == '%' && !utl_eox(p[1])) {
    p++; 
    c = utl_uchr(&s);
    q = s;
    switch (*p) {
      utl_cls('A','a',isalpha(c));  p++; break;
      utl_cls('L','l',islower(c));  p++; break;
      utl_cls('D','d',isdigit(c));  p++; break;
	    utl_cls('M','m',isalnum(c));  p++; break;   
	    utl_cls('B','b',isblank(c));  p++; break;
	    utl_cls('C','c',iscntrl(c));  p++; break;
	    utl_cls('G','g',isgraph(c));  p++; break;
	    utl_cls('N','n',isprint(c));  p++; break;
	    utl_cls('P','p',ispunct(c));  p++; break;
	    utl_cls('S','s',isspace(c));  p++; break;
	    utl_cls('U','u',isupper(c));  p++; break;
 	    utl_cls('X','x',isxdigit(c)); p++; break;
	    utl_cls('I','i',isascii(c));  p++; break;
      
      case '.' : ret = 1; break;
      
      default: cp = utl_uchr(&p); utl_isa(cp == c); 
    }
  }
  else if (*p == '[') {
    p++; c = utl_uchr(&s); ret = utl_ccls(&p,c);
  }
  else if (*p == '(') {
    p++; ret = utl_expr(&p,&s,v); 
  }
  else {
    cp = utl_uchr(&p);  c = utl_uchr(&s);
    if (c) ret = ( cp == c);
    logNdbg("fact: '%c' == '%c' (%d)",cp,c,ret);
  }  

  *ppat = p;           /* Pattern is always consumed */
  if (ret) *pstr = s;  /* input text only if it matches */
  return ret;
}

static void utl_resetv(vec_t v)
{
  int k;
  if (v) {
    for (k = 2*UTL_MAX_CAPT+1; k >= 0; k--) {
      logNdbg("%d: NULL",ret);
      vecSet(char *,v,k, NULL);
    }
    v->first = 0;  v->last = 0;  
  } 
}

static int utl_term(char **ppat, char **pstr, vec_t v)
{
  char *p = *ppat;
  char *s = *pstr;

  int nmatch = 0;
  int ret = 0;
  int inv = 0;
  
  if (*p == '!') {inv = 1; p++ ;} 
  while(!utl_endpat(*p)) {  /* to handle * and + */
    *ppat = p;
    logNdbg("term: [%s] [%s]",p,s);
    if (p[0] == '$' && !(p[1] < 2)) {
      logNdbg("End set to [%s]",s);
      vecSet(char *,v,2 * UTL_MAX_CAPT,s);
      vecSet(char *,v,2 * UTL_MAX_CAPT+1,p+1);
      p += 2;
      continue;
    }
    ret = utl_fact(&p,&s,v);
    if (ret) {
      nmatch++;
      if (*p == '+' || *p == '*') { /* let's try again (if there's any text left) */
        p = *s? *ppat : p+1; 
      }   
      else if (*p == '?') { p++; break; }          /* got it once, ok! */
    } else {
      if (*p == '?' || *p == '*') { ret = 1; p++; } /* no match, but it's ok */
      else  if (*p == '+') { p++; ret = (nmatch > 0); }  /* did it already match once? */
      break;
    }
  }
  logNdbg("termret: %d",ret);
  *ppat = p;              /* Pattern is always consumed! */
  if (inv) return !ret;   /* The ! operator does not consume input */
  if (ret) *pstr = s;     /* Only on match we consume input */
  
  return ret;
}
  
int utl_match_tok(vec_t v)
{
  int ret = 0;
  char *p;
  p = vecGet(char *,v,2 * UTL_MAX_CAPT+1,NULL);
  if (p) ret = *p;
  return ret;
}

int utl_match_len(vec_t v, int n)
{
  int ret = 0;
  if (n < UTL_MAX_CAPT+1)
    ret = vecGet(char *,v,n*2+1,NULL) - vecGet(char *,v,n*2,NULL);
  return ret;
}

char *utl_match_capt(vec_t v, int n)
{
  char *ret = NULL;
  if (n < UTL_MAX_CAPT)
    ret = vecGet(char *,v,n*2,NULL);
  return ret;
}

static char *utl_consume(char *p)
{
  int k = 0; /* consume untried pattern */
  while (*p) {
    logNdbg("consuming: '%c'",*p);
    switch (*p) {
      case '(' : k++; break;

      case ')' : if (k == 0) return p;
                 k--;
                 break;

      case '|' : if (k == 0) return p;
                 break;

      case '\1': return p;

    }
    p++;
  }
  return p;
}

static int utl_expr(char **ppat, char **pstr, vec_t v)
{
  int ret = 0;
  char *p = *ppat;
  char *s = *pstr;
  char *q = NULL;

  logNdbg("expr: [%s][%s]",p,s);
  utl_match_open(v,*pstr);
  
  for(;;) { /* to handle alternatives */
    while (!utl_endpat(*p) && (ret = utl_term(&p,&s,v))) ;
    logNdbg("alt: [%s][%s] ret: %d",p,s,ret);
    if (ret) break;    
    p = utl_consume(p);
    if (*p != '|') break;
    p++; s=*pstr;  /* try next alternative */
  }
  logNdbg("exprend (before): [%s]",p);
  
  for (;;) {
    p = utl_consume(p); /* consume untried pattern */
    if (*p != '|') break;
    p++;
  } 
  
  if (!utl_eox(*p)) p++;
  
  *ppat = p;
  logNdbg("exprend (after): [%s] ret:%d",p, ret);
  
  if (ret) {
    if ((q=vecGet(char *,v, 2* UTL_MAX_CAPT,NULL))) s = q;
    *pstr = s;
  }

  utl_match_close(v,*pstr);
  
  return ret;
}

int utl_match(char *pat, char *str, vec_t v)
{
  char *p = pat;
  char *s = str;
  int ret = 0;
  
  if (p && *p) {
    for (; ret==0 ; p++) { /* handle multiple expressions */
      s=str;
      utl_resetv(v);
      logNdbg("match ret: %d",ret);
      ret = utl_expr(&p, &s,v);
      ret = s - str;
      logNdbg("consumed: %d",ret);
      logNdbg("pattern ends on %d", *p);
      if (*p == 0) break;
    } 
  }
  return ret; /* the amount of input consumed */
}


#endif /* UTL_LIB */

#endif /* UTL_NOREX */

#endif /* UTL_NOADT */

#endif /* UTL_H */

/**************************************************************************/

