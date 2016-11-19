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

#define utl_log_trc(c,...)  do { \
                              utl_log_time();\
                              fputs(c,utl_log_file);\
                              snprintf(utl_log_buf,UTL_LOG_BUF_SIZE,__VA_ARGS__);\
                              fputs(utl_log_buf,utl_log_file);\
                              fprintf(utl_log_file," %s:%d\n",__FILE__,__LINE__);\
                              utl_log_trc_check(utl_log_buf,utl_log_watch,__FILE__,__LINE__);\
                              fflush(utl_log_file);\
                            } while(0)
                              
//                              
//

#define logtracewatch(...)  for (char *utl_log_watch[UTL_LOG_WATCH_SIZE] = {__VA_ARGS__,""}; \
                                                      utl_log_watch[0] != NULL; \
                                                      utl_log_watch[0] = NULL, utl_log_trc_check_last(utl_log_watch,__FILE__,__LINE__))
							 
#define utl_log_prt(...) (utl_log_time(), \
                          fprintf(utl_log_file,__VA_ARGS__),\
                          fputc('\n',utl_log_file),\
                          fflush(utl_log_file))

#define logprintf(...) (void)utl_log_prt(__VA_ARGS__)
#define logopen(f,m)   utl_log_open(f,m)
#define logclose()     utl_log_close("LOG STOP")
                       
#ifndef NDEBUG
#define logcheck(e)    utl_log_check(!!(e),#e,__FILE__,__LINE__)
#define logassert(e)   utl_log_assert(!!(e),#e,__FILE__,__LINE__)
#define logclock       for(clock_t utl_log_clk = clock();\
                               utl_log_clk != (clock_t)-1; \
                                   utl_log_prt("CLK %ld/%ld sec. %s:%d",(clock()-utl_log_clk), (long int)CLOCKS_PER_SEC,__FILE__,__LINE__),\
                                   utl_log_clk = (clock_t)-1 )
#define logdebug       utl_log_trc("DBG ",__VA_ARGS__)

#else
#define logcheck(e)    utl_ret(1)
#define logassert(e)   
#define logclock
#define logdebug(...)
#define logtracewatch  
#define UTL_NOTRACE
#endif

#ifndef UTL_NOTRACE
#define logtrace(...)       utl_log_trc("TRC ",__VA_ARGS__)
#else
#define logtrace(...)
#endif

#define _logprintf(...)     
#define _logcheck(...)      utl_ret(1)
#define _logassert(...)     
#define _logopen(f,m)       utl_retptr(NULL)
#define _logclose()         utl_ret(0)
#define _logclock
#define _logtrace(...)  
#define _logdebug(...)       
#define _logtracewatch

extern FILE *utl_log_file;
extern uint32_t utl_log_check_num;
extern uint32_t utl_log_check_fail;
extern char *utl_log_watch[];
extern char utl_log_buf[UTL_LOG_BUF_SIZE];

FILE *utl_log_open(const char *fname, const char *mode);
int   utl_log_close(const char *msg);
int   utl_log_check(int res, const char *test, const char *file, int32_t line);
void  utl_log_assert(int res, const char *test, const char *file, int32_t line);
int   utl_log_time(void);
void  utl_log_trc_check(char *buf, char *watch[], const char *file, int32_t line);
void  utl_log_trc_check_last(char *watch[], const char *file, int32_t line);

#endif
//>>>//
