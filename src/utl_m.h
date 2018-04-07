#line 2 "src/utl_hdr.h"
/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                        ___   __
**                     __/  /_ /  )
**             ___  __(_   ___)  /
**            /  / /  )/  /  /  /
**           /  (_/  //  (__/  / 
**          (____,__/(_____(__/
**    https://github.com/rdentato/clibutl
**
*/    
#ifndef UTL_H
#define UTL_H

#ifdef __cplusplus
extern "C" {
#define UTL_NOTRY
#endif

#ifdef UTL_NET
#ifndef _WIN32
#define _POSIX_C_SOURCE 201112L
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <setjmp.h>
#include <limits.h>


#ifdef NDEBUG

/* Give NDEBUG precedence over DEBUG */
#ifdef DEBUG
#undef DEBUG
#endif

/* Disable memory logging if NDEBUG is defined */
#ifdef UTL_MEMCHECK
#undef UTL_MEMCHECK
#endif

#endif /* NDEBUG */

#ifdef UTL_MEMCHECK
#ifdef UTL_NOLOG
#undef UTL_NOLOG
#endif
#endif

int   utl_ret(int x);
void *utl_retptr(void *x);

uint32_t utl_hash_string(void *key);
uint32_t utl_hash_int32(void *key);

#define utlhashstring  utl_hash_string
#define utlhashint32   utl_hash_int32

extern const char *utl_emptystring;
#define utlemptystring utl_emptystring;

// utl_expand() is just to please Microsoft C whose preprocessor
// behaves differently from the other compilers (up to VS2015, at least)
#define utl_expand(x) x

#define utl_arg0(_x0,...)                                     _x0
#define utl_arg1(_x0,_x1,...)                                 _x1
#define utl_arg2(_x0,_x1,_x2,...)                             _x2
#define utl_arg3(_x0,_x1,_x2,_x3,...)                         _x3
#define utl_arg4(_x0,_x1,_x2,_x3,_x4,...)                     _x4
#define utl_arg5(_x0,_x1,_x2,_x3,_x4,_x5,...)                 _x5
#define utl_arg6(_x0,_x1,_x2,_x3,_x4,_x5,_x6,...)             _x6
#define utl_arg7(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,...)         _x7
#define utl_arg8(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,_x8,...)     _x8
#define utl_arg9(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,_x8,_x9,...) _x9

int utl_unpow2(int n);

//uint32_t utl_rnd();

// Extended to include the full Latin-1 characters
int utlisdigit(int ch);
int utlisalpha(int ch);
int utlisalnum(int ch);
int utlislower(int ch);
int utlisupper(int ch);
int utlisblank(int ch);
int utlisspace(int ch);
int utliscntrl(int ch);
int utlisgraph(int ch);
int utlispunct(int ch);
int utlisprint(int ch);
int utlisxdigit(int ch);

int utlfoldchar(int ch);
  
int utl_next_utf8(const char *txt, int32_t *ch);
#define utlnextutf8(...) \
                 utl_next_utf8(utl_expand(utl_arg0(__VA_ARGS__,NULL)),\
                               utl_expand(utl_arg1(__VA_ARGS__,NULL,NULL)))

typedef int (*utl_txt_action_t)(const char *, const char *, void *);

#ifdef __cplusplus
}
#endif
                               
#line 28 "src/utl_log.h"
#ifndef UTL_NOLOG

#define UTL_LOG_BUF_SIZE 256
#define UTL_LOG_WATCH_SIZE 16

#define UTL_LOG_N  5
#define UTL_LOG_E  4
#define UTL_LOG_W  3
#define UTL_LOG_I  2
#define UTL_LOG_D  1
#define UTL_LOG_T  0

#define utl_log_trc(l,c,...)  if (l) { \
                                char utl_log_buf[UTL_LOG_BUF_SIZE];\
                                utl_log_time();\
                                memcpy(utl_log_buf,c,4);\
                                snprintf(utl_log_buf+4,UTL_LOG_BUF_SIZE-4,__VA_ARGS__);\
                                fputs(utl_log_buf,utl_log_file);\
                                fprintf(utl_log_file,"\x09:%s:%d\x09\n",__FILE__,__LINE__);\
                                utl_log_watch_check(utl_log_buf+4,utl_log_watch,__FILE__,__LINE__);\
                                fflush(utl_log_file);\
                              } else (void)0

#define utl_log_prt(...) (utl_log_time(), \
                          fprintf(utl_log_file,__VA_ARGS__),\
                          fputc('\n',utl_log_file),\
                          fflush(utl_log_file))

extern const char *utl_log_w;

#define logprintf(...) (void)utl_log_prt(__VA_ARGS__)
//#define logopen(f,m)    utl_log_open(f,m)
#define logopen(...)    utl_log_open utl_expand((utl_arg0(__VA_ARGS__,utl_emptystring), \
                                                 utl_arg1(__VA_ARGS__,utl_log_w,utl_log_w)))


#define logclose()      utl_log_close("LOG STOP")
#define loglevel(s)     utl_log_setlevel(s)
#define loginfo(...)
#define logwarning(...)
#define logerror(...)

#ifndef UTL_LOGLVL_NONE
#undef  logerror
#define logerror(...)   utl_log_trc(utl_log_prdlvl <= UTL_LOG_E,"ERR ",__VA_ARGS__)
#ifndef UTL_LOGLVL_ERR
#undef  logwarning
#define logwarning(...) utl_log_trc(utl_log_prdlvl <= UTL_LOG_W,"WRN ",__VA_ARGS__)
#ifndef UTL_LOGLVL_WRN
#undef  loginfo
#define loginfo(...)    utl_log_trc(utl_log_prdlvl <= UTL_LOG_I,"INF ",__VA_ARGS__)
#endif
#endif
#else // ULT_LOGLVL_NONE
#define UTL_NDEBUG
#endif

#ifdef NDEBUG
#define UTL_NDEBUG
#endif

#ifndef UTL_NDEBUG
#define logcheck(e)       utl_log_check(!!(e),#e,__FILE__,__LINE__)
#define logassert(e)      utl_log_assert(!!(e),#e,__FILE__,__LINE__)
#define logexpect(e,...) (utl_log_check(!!(e),#e,__FILE__,__LINE__)? 1 :(logprintf(__VA_ARGS__),0))

#define logclock          for (clock_t utl_log_clk = clock();\
                               utl_log_clk != (clock_t)-1; \
                               (utl_log_dbglvl <= UTL_LOG_D)?utl_log_prt("CLK %ld/%ld sec.\x09:%s:%d\x09",(clock()-utl_log_clk), (long int)CLOCKS_PER_SEC,__FILE__,__LINE__):1,\
                                     utl_log_clk = (clock_t)-1 )

#define logdebug(...)  utl_log_trc(utl_log_dbglvl <= UTL_LOG_D,"DBG ",__VA_ARGS__)


struct log_watch_s {
  struct log_watch_s *prev;
  char *watch[UTL_LOG_WATCH_SIZE] ;
  int  flg;
};

typedef struct log_watch_s log_watch_t;

#define logwatch(...)  for (log_watch_t utl_log_watch_= {.prev = utl_log_watch, .watch = {__VA_ARGS__,"\1"}, .flg = 1}; \
                                 (utl_log_watch = &utl_log_watch_), \
                                   (utl_log_watch_.flg) ? ((utl_log_dbglvl <= UTL_LOG_D) ? utl_log_prt("WCH START\x09:%s:%d\x09",__FILE__,__LINE__):1)\
                                                        : ((utl_log_watch = utl_log_watch->prev), 1), \
                                   utl_log_watch_.flg; \
                                 utl_log_watch_last(utl_log_watch,__FILE__,__LINE__),\
                                 (void)(( utl_log_dbglvl <= UTL_LOG_D) && utl_log_prt("WCH END\x09:%s:%d\x09",__FILE__,__LINE__)),utl_log_watch_.flg = 0)
							 
#define logifdebug       if (utl_log_dbglvl > UTL_LOG_D) ; else

#else
#define logcheck(e)    utl_ret(1)
#define logexpect(e)   utl_ret(1)
#define logassert(e)   
#define logclock
#define logdebug(...)
#define logwatch(...)
#define logifdebug       if (1) ; else
#define UTL_NOTRACE
#endif

#ifndef UTL_NOTRACE
#define logtrace(...)       utl_log_trc(utl_log_dbglvl <= UTL_LOG_T,"TRC ",__VA_ARGS__)
#else
#define logtrace(...)
#endif

#define _logprintf(...)     
#define _logcheck(...)      utl_ret(1)
#define _logexpect(...)     utl_ret(1)
#define _logassert(...)     
#define _logopen(f,m)       utl_retptr(NULL)
#define _logclose()         utl_ret(0)
#define _logclock
#define _logdebug(...)
#define _logifdebug       if (1) ; else
#define _loginfo(...)
#define _logwarning(...)
#define _logerror(...)
#define _logtrace(...)  
#define _logwatch(...)
   

extern FILE *utl_log_file;
extern uint32_t utl_log_check_num;
extern uint32_t utl_log_check_fail;
extern log_watch_t *utl_log_watch;
extern int16_t utl_log_dbglvl;
extern int16_t utl_log_prdlvl;

FILE *utl_log_open(const char *fname, const char *mode);
int   utl_log_close(const char *msg);
int   utl_log_check(int res, const char *test, const char *file, int32_t line);
void  utl_log_assert(int res, const char *test, const char *file, int32_t line);
int   utl_log_time(void);
void  utl_log_watch_check(char *buf, log_watch_t *lwatch, const char *file, int32_t line);
void  utl_log_watch_last(log_watch_t *lwatch, const char *file, int32_t line);
void  utl_log_setlevel(const char *lvl);

#endif
#line 105 "src/utl_fsm.h"

#ifndef UTL_NOFSM

#define fsm           
#define fsmGOTO(x)    goto fsm_state_##x
#define fsmSTATE(x)   fsm_state_##x :
#define fsmSTART      

#endif
#line 48 "src/utl_try.h"

#ifndef UTL_NOTRY

typedef struct utl_jb_s {
  jmp_buf           jmp;
  struct utl_jb_s  *prv;
  const char       *fn;
  int               ln;
  int               ex;
  int16_t           flg;
  int16_t           id;
} utl_jb_t;

extern utl_jb_t *utl_jmp_list; // Defined in utl_hdr.c

#define UTL_TRY_INIT  {.flg=0, \
                       .prv=utl_jmp_list, \
                       .fn=utl_emptystring, \
                       .ln=0, \
                       .id=0}

#define try  for ( utl_jb_t utl_jb = UTL_TRY_INIT; \
                  !utl_jb.flg && (utl_jmp_list=&utl_jb); \
                   utl_jmp_list=utl_jb.prv, utl_jb.flg=1) \
              if ((utl_jb.ex = setjmp(utl_jb.jmp)) == 0)

#define UTL_CATCH_TEST(x,y,w,z) ((1<<(x))|(1<<(y))|(1<<(w))|(1<<(z)))

#define utl_catch(x,y,w,z) else if ( (utl_jb.ex  & UTL_CATCH_TEST(x,y,w,z)) \
                                  && (utl_jmp_list=utl_jb.prv, utl_jb.flg=1)) 

#define catch(...) utl_catch(utl_expand(utl_arg0(__VA_ARGS__,16)),\
                             utl_expand(utl_arg1(__VA_ARGS__,16,16)),\
                             utl_expand(utl_arg2(__VA_ARGS__,16,16,16)),\
                             utl_expand(utl_arg3(__VA_ARGS__,16,16,16,16)))

#define catchall else for ( utl_jmp_list=utl_jb.prv; \
                           !utl_jb.flg; \
                            utl_jb.flg=1) 

#define utl_throw(x,y)    do { \
                            int ex_ = x; \
                            if (ex_ > 0 && utl_jmp_list) {\
                              /*logwarning("Exception: %d (%d)",utl_unpow2(x),y);*/\
                              utl_jmp_list->fn = __FILE__; \
                              utl_jmp_list->ln = __LINE__;\
                              utl_jmp_list->id = y;\
                              longjmp(utl_jmp_list->jmp,ex_); \
                            }\
                          } while (0)

#define throw(...) utl_throw(1<<(utl_expand(utl_arg0(__VA_ARGS__,0)) & 0xF),\
                                 utl_expand(utl_arg1(__VA_ARGS__,0,0)))

#define rethrow()    utl_throw(utl_jb.ex,utl_jb.id)

#define thrown()     utl_unpow2(utl_jb.ex)

#define thrownid()   utl_jb.id
#define thrownfile() utl_jb.fn
#define thrownline() utl_jb.ln

#endif
#line 17 "src/utl_end.h"

#ifdef __cplusplus
}
#endif

#endif /* UTL_H */

