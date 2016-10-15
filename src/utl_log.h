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

#define utl_log_trc(c,t,...)  (void)(utl_log_time(), fprintf(utl_log_file,c" [%s] ",t),\
                                               fprintf(utl_log_file,__VA_ARGS__), \
                                               fprintf(utl_log_file," %s:%d\n",__FILE__,__LINE__), \
                                               fflush(utl_log_file))

#define logprintf(...) (utl_log_time(),fprintf(utl_log_file,__VA_ARGS__), fputc('\n',utl_log_file),fflush(utl_log_file))
#define logopen(f,m)   utl_log_open(f,m)
#define logclose()     utl_log_close("LOG STOP")
                       
#ifndef NDEBUG
#define logcheck(e)    utl_log_check(!!(e),#e,__FILE__,__LINE__)
#define logassert(e)   utl_log_assert(!!(e),#e,__FILE__,__LINE__)
#define logdebug(...)  utl_log_trc("TRC","DBG",__VA_ARGS__)
#define logclock       for(clock_t utl_log_clk = clock();\
                               utl_log_clk != (clock_t)-1; \
                                   logprintf("CLK %ld (s/%ld) %s:%d",(clock()-utl_log_clk), (long int)CLOCKS_PER_SEC,__FILE__,(int)__LINE__),\
                                   utl_log_clk = (clock_t)-1 )
#else
#define logcheck(e)    utl_ret(1)
#define logassert(e)   ((void)0)
#define logdebug(...)  utl_ret(0)
#define logclock
#define UTL_NOTRACE
#endif

#ifndef UTL_NOTRACE
#define logtrace(t,...)       utl_log_trc("TRC",t,__VA_ARGS__)
#define logtracecheck(t,...)  utl_log_trc("TCK",t,__VA_ARGS__)
#else
#define logtrace(...)
#define logchecktrace(...)
#endif

#define _logprintf(...) utl_ret(0)
#define _logdebug(...)  utl_ret(0)
#define _logcheck(...)  utl_ret(1)
#define _logassert(...) ((void)0)
#define _logopen(f,m)   utl_retptr(NULL)
#define _logclose()     utl_ret(0)
#define _logclock
#define _logtrace(...)       utl_ret(0)
#define _logchecktrace(...)  utl_ret(0)

extern FILE *utl_log_file;

FILE *utl_log_open(const char *fname, const char *mode);
int   utl_log_close(const char *msg);
int   utl_log_check(int res, const char *test, const char *file, int32_t line);
void  utl_log_assert(int res, const char *test, const char *file, int32_t line);
int   utl_log_time(void);

#endif
//>>>//
