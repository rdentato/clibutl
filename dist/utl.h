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
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>

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

extern const char *utl_emptystring;


#line 28 "src/utl_log.h"
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
int   utl_log_time();

#endif
#line 24 "src/utl_mem.h"
#ifndef UTL_NOMEM

#ifndef memINVALID
#define memINVALID    -2
#define memOVERFLOW   -1
#define memVALID       0
#define memNULL        1
#endif

void  *utl_malloc   (size_t size, const char *file, int32_t line );
void  *utl_calloc   (size_t num, size_t size, const char *file, int32_t line);
void  *utl_realloc  (void *ptr, size_t size, const char *file, int32_t line);
void   utl_free     (void *ptr, const char *file, int32_t line );
void  *utl_strdup   (const char *ptr, const char *file, int32_t line);
                    
int    utl_check    (void *ptr, const char *file, int32_t line);
size_t utl_mem_used (void);


#ifdef UTL_MEMCHECK

#ifndef UTL_MEM
#define malloc(n)     utl_malloc(n,__FILE__,__LINE__)
#define calloc(n,s)   utl_calloc(n,s,__FILE__,__LINE__)
#define realloc(p,n)  utl_realloc(p,n,__FILE__,__LINE__)
#define free(p)       utl_free(p,__FILE__,__LINE__)

#ifdef strdup
#undef strdup
#endif
#define strdup(p)     utl_strdup(p,__FILE__,__LINE__)

#define memcheck(p)   utl_check(p,__FILE__, __LINE__)
#define memused()     utl_mem_used()
#endif /* UTL_MEM */

#else /* UTL_MEMCHECK */

#define memcheck(p)     memVALID
#define memused()       0

#endif /* UTL_MEMCHECK */
#endif /* UTL_NOMEM */
#line 18 "src/utl_vec.h"
#ifndef UTL_NOVEC

#define vec_MIN_ELEM 16
#define vec_MAX_ELEM (UINT32_MAX-1)
#define vec_MAX_CNT  UINT32_MAX

typedef struct vec_s {
  uint32_t  fst; 
  uint32_t  lst; 
  uint32_t  max;
  uint32_t  cnt;
  uint16_t  esz;  /* each element max size: 64K */
  uint16_t  flg;
  int     (*cmp)(void *, void *);
  uint8_t  *vec;
  void     *elm; // this will always point to eld
  uint8_t   eld[4];
} vec_s, *vec_t;

#define vecset(type,v,i,e)   vecDO(type,v,i,e,utl_vec_set)
#define vecins(type,v,i,e)   vecDO(type,v,i,e,utl_vec_ins)
#define vecDO(type,v,i,e,x)  do {vec_t v_=v;  *((type *)(v_->elm)) = (e); x(v_,i);} while (0)

 
#define vecadd(type,v,e)     vecins(type,v,vec_MAX_CNT,e)
#define vecpush(type,v,e)    vecins(type,v,vec_MAX_CNT,e)

#define vecdrop(v)           do {vec_t v_=v; if (v_->cnt) v_->cnt--;} while (0)

#define vecget(type,v,i)     (type *)utl_vec_get(v,i)
#define vectop(type,v)       vecget(type,v,vec_MAX_CNT)    


#define vec(type,v)          ((type *)((v)->vec))

#define vecnew(type)         utl_vec_new(sizeof(type))
#define vecfree(v)           utl_vec_free(v)
#define vecdel(v,i)          utl_vec_del(v,i)

#define veccount(v) ((v)->cnt)
#define vecmax(v)   ((v)->max)

#define vecread(v,i,n,f)  utl_vec_read(v,i,n,f)
#define vecwrite(v,i,n,f) utl_vec_write(v,i,n,f)

#define vecsort(v,c)           utl_vec_sort(v,c)

#define vecsearch(type,v,e)    (type *)utl_vec_search(v, (*((type *)(v->elm)) = (e), 0))  

#define vecSORTED 0x0001

#define vecisempty(v)  ((v)->cnt == 0)
#define vecissorted(v) ((v)->flg &   vecSORTED)
#define vecsorted(v)   ((v)->flg |=  vecSORTED)
#define vecunsorted(v) ((v)->flg &= ~vecSORTED)

void *utl_vec_set(vec_t v, uint32_t i);
void *utl_vec_ins(vec_t v, uint32_t i);
void *utl_vec_get(vec_t v, uint32_t i);

int16_t utl_vec_del(vec_t v,  uint32_t i);
int16_t utl_vec_delrange(vec_t v, uint32_t i,  uint32_t j);

vec_t utl_vec_new(uint16_t esz);
vec_t utl_vec_free(vec_t v);

size_t utl_vec_read(vec_t v,uint32_t i, size_t n,FILE *f);
size_t utl_vec_write(vec_t v, uint32_t i, size_t n, FILE *f);

void utl_vec_sort(vec_t v, int (*cmp)(void *, void *));
void *utl_vec_search(vec_t v,int x);


#define que_t                 vec_t

#define buf_t                 vec_t
#define bufnew()              vecnew(char)
#define bufaddc(b,c)          vecadd(char,b,c)
#define bufsetc(b,i,c)        vecset(char,b,i,c)
#define bufinsc(b,i,c)        utl_buf_insc(b,i,c)
#define bufinss(b,i,s)        utl_buf_inss(b,i,s)
#define bufsets(b,i,s)        utl_buf_sets(b,i,s)

#define bufgetc(b,i)          utl_buf_get(b,i)
#define bufdel(b,i,j)         utl_buf_del(b,i,j)
#define bufread(b,i,n,f)      utl_buf_read(b,i,n,f)
#define bufreadln(b,i,f)      utl_buf_readln(b,i,f)
#define bufreadall(b,i,f)     utl_buf_readall(b,i,f)
#define bufwrite(b,i,n,f)     vecwrite(b,i,n,f)
#define buf(b)                vec(char, b)
#define buflen(b)             veccount(b)

char utl_buf_get(buf_t b, uint32_t n);
size_t utl_buf_readall(buf_t b, uint32_t i, FILE *f);
size_t utl_buf_read(buf_t b, uint32_t i, uint32_t n, FILE *f) ;
char *utl_buf_readln(buf_t b, uint32_t i, FILE *f);
char *utl_buf_sets(buf_t b, uint32_t i, const char *s);
char *utl_buf_inss(buf_t b, uint32_t i, const char *s);
char *utl_buf_insc(buf_t b, uint32_t i, char c);
int16_t utl_buf_del(buf_t b, uint32_t i,  uint32_t j);

#endif 
#line 20 "src/utl_pmx.h"
#ifndef UTL_NOPMX

#define utl_pmx_MAXCAPT 16

extern int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch);

extern const char  *utl_pmx_capt[utl_pmx_MAXCAPT][2];
extern uint8_t      utl_pmx_capnum                  ;
extern const char  *utl_pmx_error                   ;

#define pmxsearch(r,t)  utl_pmx_search(r,t)
#define pmxstart(n)    (utl_pmx_capt[n][0])
#define pmxend(n)      (utl_pmx_capt[n][1])
#define pmxcount()     (utl_pmx_capnum)
#define pmxlen(n)       utl_pmx_len(n)
#define pmxerror()     (utl_pmx_error?utl_pmx_error:utl_emptystring)
#define pmxextend(f)   (void)(utl_pmx_ext = f)

const char *utl_pmx_search(const char *pat, const char *txt);
int    utl_pmx_len(uint8_t n);
void   utl_pmx_extend(int(*ext)(const char *, const char *,int, int32_t));

#endif
#line 105 "src/utl_fsm.h"

#ifndef UTL_NOFSM

#define fsm           
#define fsmGOTO(x)    goto fsm_state_##x
#define fsmSTATE(x)   fsm_state_##x :
#define fsmSTART      

#endif
#line 17 "src/utl_end.h"

#ifdef __cplusplus
}
#endif

#endif /* UTL_H */

