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


**          __
**         /  )
**        /  /______   ______
**       /  //  __  \ /  __  \
**      /  //  (_/  //  (_/  /
**     (__/ \______/ \___   /
**                    __/  /
**                   (____/
*/

//<<<//
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
                                 (( utl_log_dbglvl <= UTL_LOG_D) && utl_log_prt("WCH END\x09:%s:%d\x09",__FILE__,__LINE__)),utl_log_watch_.flg = 0)
							 
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
//>>>//
