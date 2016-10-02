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

#define logprintf(...) utl_log_printf(NULL,NULL,0,__VA_ARGS__)
#define logopen(f,m)   utl_log_open(f,m)
#define logclose()     utl_log_close("LOG STOP")
                       
#ifndef NDEBUG
#define logcheck(e)    utl_log_check(!!(e),#e,__FILE__,__LINE__)
#define logassert(e)   utl_log_assert(!!(e),#e,__FILE__,__LINE__)
#define logdebug(...)  utl_log_printf(utl_log_DBG,__FILE__,__LINE__,__VA_ARGS__)
#define logclock       for(int utl_log_clk_stop = 0, utl_log_clk = clock();\
                           utl_log_clk_stop++ == 0; \
                           logprintf("CLK  %ld (s/%ld) %s:%d",clock()-utl_log_clk, CLOCKS_PER_SEC,__FILE__,__LINE__))
#else
#define logcheck(e)    utl_ret(1)
#define logassert(e)   ((void)0)
#define logdebug(...)  utl_ret(0)
#define logclock
#define UTL_NOTRACE
#endif

#ifndef UTL_NOTRACE
#define logtrace(t,...)      utl_log_printf(utl_log_TRC,__FILE__,__LINE__,__VA_ARGS__)
#define logchecktrace(t,...) utl_log_printf(utl_log_TCK,__FILE__,__LINE__,__VA_ARGS__)
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

extern clock_t utl_log_clk;
extern const char *utl_log_TRC;
extern const char *utl_log_TCK;
extern const char *utl_log_DBG;

int utl_log_printf(const char *categ, const char *fname, int32_t line,const char *format, ...);
FILE *utl_log_open(const char *fname, const char *mode);
int   utl_log_close(const char *msg);
int   utl_log_check(int res, const char *test, const char *file, int32_t line);
void  utl_log_assert(int res, const char *test, const char *file, int32_t line);

#endif
//>>>//
