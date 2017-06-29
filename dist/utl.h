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

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
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

#define utl_arg0(x0,...)              x0
#define utl_arg1(x0,x1,...)           x1
#define utl_arg2(x0,x1,x2,...)        x2
#define utl_arg3(x0,x1,x2,x3,...)     x3
#define utl_arg4(x0,x1,x2,x3,x4,...)  x4

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


typedef struct utl_mpl_node_s {
  struct utl_mpl_node_s *next;
  uint32_t               size;
  struct utl_mpl_node_s *blk;
} utl_mpl_node_s;


typedef struct {
  utl_mpl_node_s used;
  utl_mpl_node_s unused;
} mpl_s, *mpl_t;

mpl_t utl_mpl_new(void);
void *utl_mpl_malloc(mpl_t mp, uint32_t sz);
void *utl_mpl_calloc(mpl_t mp, uint32_t sz);
void *utl_mpl_realloc(mpl_t mp, uint32_t sz);
void *utl_mpl_strdup(mpl_t mp, char *s);
void *utl_mpl_free(mpl_t mp, void *e,int clean);

#define mplnew()        utl_mpl_new()
#define mplmalloc(m,sz) utl_mpl_malloc(m,sz)
#define mplfree(...)    utl_mpl_free(\
                             (void *)utl_expand(utl_arg0(__VA_ARGS__,0,0,0)), \
                             (void *)utl_expand(utl_arg1(__VA_ARGS__,0,0,0)), \
                             (int)   utl_expand(utl_arg2(__VA_ARGS__,0,0,0))  ) 


#define mplclean(m)     utl_mpl_free(m,NULL,1);


#endif /* UTL_NOMEM */
#line 18 "src/utl_vec.h"
#ifndef UTL_NOVEC

#define vec_MIN_ELEM 16
#define vec_MAX_ELEM (UINT32_MAX-1)
#define vec_MAX_CNT  UINT32_MAX


typedef int (*utl_cmp_t)(void *, void *, void *);
typedef uint32_t (*utl_hsh_t)(void *, void *);

typedef struct vec_s {
  uint32_t   fst; 
  uint32_t   lst; 
  uint32_t   cur; 
  uint32_t   max;
  uint32_t   cnt;
  uint16_t   esz;  /* each element max size: 64K */
  uint16_t   flg;
  utl_cmp_t  cmp;
  utl_hsh_t  hsh;
  uint8_t   *vec;
  void      *aux;
  void      *elm; // this will always point to eld
  uint8_t    eld[4];
} vec_s, *vec_t;

#define vecDO(type,v,i,e,x)  do {vec_t v_=v;  *((type *)(v_->elm)) = (e); x(v_,i);} while (0)

// Array
#define vecset(type,v,i,e)   vecDO(type,v,i,e,utl_vec_set)
#define vecins(type,v,i,e)   vecDO(type,v,i,e,utl_vec_ins)
#define vecget(type,v,i,d)   (*((type *)((v)->elm))=(d),utl_vec_get((v),i),*((type *)((v)->elm)))

#define vecsetptr(v,i,p)     (memcpy((v)->elm,p,(v)->esz),utl_vec_set(v,i))
#define vecinsptr(v,i,p)     (memcpy((v)->elm,p,(v)->esz),utl_vec_ins(v,i))
#define vecgetptr(v,i)       utl_vec_get(v,i)

#define vecalloc(...)  utl_vec_alloc utl_expand((utl_arg0(__VA_ARGS__,NULL), \
                                                 utl_arg1(__VA_ARGS__,vec_MAX_CNT,vec_MAX_CNT)))

#define vecdel(v,i)          utl_vec_del(v,i)

// Set (sorted or unsorted) 
#define vecadd(type,v,e)     vecDO(type,v,vec_MAX_CNT,e,utl_vec_add)
#define vecaddptr(v,p)       vecinsptr(v,vec_MAX_CNT,p)

#define vecsearch(type,v,e)  (*((type *)(v->elm)) = (e), (type *)utl_vec_search(v, 0))
#define vecremove(type,v,e)  utl_vec_remove(v, (*((type *)(v->elm)) = (e), 0))  

#define vecsort(...)  utl_vec_sort utl_expand((utl_arg0(__VA_ARGS__,NULL), utl_arg1(__VA_ARGS__,utl_vec_nullcmp,NULL)))

#define vecSORTED 0x0001
#define vecHASHED 0x0020

#define vecissorted(v) ((v)->flg &   vecSORTED)
#define vecsorted(v)   ((v)->flg |=  vecSORTED)
#define vecunsorted(v) ((v)->flg &= ~vecSORTED)

// Stack
#define vecpush(type,v,e)    vecins(type,v,vec_MAX_CNT,e)
#define utl_vec_drop(v,n)    do { \
                               vec_t v_=v; \
                               if (v_) { \
                                 if (v_->cnt > n) v_->cnt -= n; \
                                 else v_->cnt=0; \
                               } \
                             } while (0)
#define vecdrop(...)         utl_vec_drop(utl_expand(utl_arg0(__VA_ARGS__,NULL)), \
                                          utl_expand(utl_arg1(__VA_ARGS__,1,1)))
#define vectop(type,v,d)     vecget(type,v,vec_MAX_CNT,d)    
#define vectopptr(v)         vecgetptr(v,vec_MAX_CNT)    

// Queue
#define vecenq(type,v,e)     vecDO(type,v,0,e,utl_vec_enq)
#define vecdeq(v)            utl_vec_deq(v)

// I/O
#define vecread(v,i,n,f)  utl_vec_read(v,i,n,f)
#define vecwrite(v,i,n,f) utl_vec_write(v,i,n,f)

#define vecfreeze(f,v)    utl_frz(f,v,utl_vec_freeze)
#define vecunfreeze(...)  \
          utl_unfrz utl_expand((utl_arg0(__VA_ARGS__,NULL),\
                                    utl_arg1(__VA_ARGS__,NULL,NULL), \
                                    utl_arg2(__VA_ARGS__,NULL,NULL,NULL),utl_vec_unfreeze))
                                    
// Info
#define vecnew(...)       utl_vec_new utl_expand((sizeof(utl_arg0(__VA_ARGS__,int)),\
                                      utl_arg1(__VA_ARGS__,NULL,NULL), \
                                      utl_arg2(__VA_ARGS__,NULL,NULL,NULL)))

#define vecfree(v)        utl_vec_free(v)
#define veccount(v)       ((v)->cnt)
#define vecmax(v)         ((v)->max)
#define vecaux(v)         ((v)->aux)
#define vecisempty(v)     ((v)->cnt == 0)
#define vec(type,v)       ((type *)((v)->vec))
#define vecclear(v)       ((v)->cnt = 0)

#define vecfirstptr(v)    utl_vec_first(v)
#define vecnextptr(v)     utl_vec_next(v)
#define vecprevptr(v)     utl_vec_prev(v)
#define veclastptr(v)     utl_vec_last(v)

#define vecfirst(type,v,d)  (utl_vec_first(v)? *((type *)((v)->elm)):d)
#define vecnext(type,v,d)   (utl_vec_next(v)? *((type *)((v)->elm)):d)

#define vecprev(type,v,d)   (utl_vec_prev(v)? *((type *)((v)->elm)):d)
#define veclast(type,v,d)   (utl_vec_last(v)? *((type *)((v)->elm)):d)

// Protypes
void *utl_vec_alloc(vec_t v, uint32_t i);
void *utl_vec_set(vec_t v, uint32_t i);
void *utl_vec_ins(vec_t v, uint32_t i);
void *utl_vec_add(vec_t v, uint32_t i);
void *utl_vec_get(vec_t v, uint32_t i);

void *utl_vec_first(vec_t v);
void *utl_vec_next(vec_t v) ;
void *utl_vec_prev(vec_t v) ;
void *utl_vec_last(vec_t v) ;

int16_t utl_vec_del(vec_t v,  uint32_t i);
int16_t utl_vec_delrange(vec_t v, uint32_t i,  uint32_t j);

vec_t utl_vec_new(uint16_t esz, utl_cmp_t cmp, utl_hsh_t hsh);
vec_t utl_vec_free(vec_t v);

size_t utl_vec_read(vec_t v,uint32_t i, size_t n,FILE *f);
size_t utl_vec_write(vec_t v, uint32_t i, size_t n, FILE *f);

vec_t utl_unfrz(char *fname, utl_cmp_t cmp, utl_hsh_t hsh, 
                           vec_t (*fun)(FILE *, utl_cmp_t, utl_hsh_t));
int utl_frz(char *fname, vec_t t, int (*fun)(FILE *, vec_t));

int utl_vec_freeze(FILE *f,vec_t v);
vec_t utl_vec_unfreeze(FILE *f, utl_cmp_t cmp, utl_hsh_t hsh);

void utl_vec_sort(vec_t v, utl_cmp_t cmp);
void *utl_vec_search(vec_t v,int x);
int utl_vec_remove(vec_t v, int x);

int utl_vec_nullcmp(void *a, void *b, void *aux);

void *utl_vec_enq(vec_t v, uint32_t i);
void utl_vec_deq(vec_t v);

// Character buffer
#define buf_t                 vec_t
#define bufnew()              vecnew(char)
#define buffreeze(f,b)        vecfreeze(f,b)
#define bufunfreeze(f)        vecunfreeze(f)
#define buffree(b)            vecfree(b)
#define bufaddc(b,c)          utl_buf_addc(b,c)
#define bufsetc(b,i,c)        vecset(char,b,i,c)
#define bufinsc(b,i,c)        utl_buf_insc(b,i,c)
#define bufinss(b,i,s)        utl_buf_inss(b,i,s)
#define bufsets(b,i,s)        utl_buf_sets(b,i,s)
#define bufadds(b,s)          utl_buf_sets(b,vec_MAX_CNT,s)

#define bufgetc(b,i)          utl_buf_get(b,i)
#define bufdel(b,i,j)         utl_buf_del(b,i,j)
#define bufread(b,i,n,f)      utl_buf_read(b,i,n,f)
#define bufreadln(b,i,f)      utl_buf_readln(b,i,f)
#define bufreadall(b,i,f)     utl_buf_readall(b,i,f)
#define bufwrite(b,i,n,f)     vecwrite(b,i,n,f)
#define buf(b)                vec(char, (vec_t)b)
#define buflen(b)             veccount(b)

#define bufaux(b)             vecaux(b)
#define bufcur(b)            ((b)->cur)
#define buffmt(b,i,f,...)     utl_buf_fmt(b,i,f,__VA_ARGS__)
#define bufsetf(b,f,...)      utl_buf_fmt(b,0,f,__VA_ARGS__)
#define bufaddf(b,f,...)      utl_buf_fmt(b,vec_MAX_CNT,f,__VA_ARGS__)

char utl_buf_get(buf_t b, uint32_t n);
size_t  utl_buf_readall(buf_t b, uint32_t i, FILE *f);
size_t  utl_buf_read(buf_t b, uint32_t i, uint32_t n, FILE *f) ;
char   *utl_buf_readln(buf_t b, uint32_t i, FILE *f);
char   *utl_buf_sets(buf_t b, uint32_t i, const char *s);
char   *utl_buf_inss(buf_t b, uint32_t i, const char *s);
char   *utl_buf_insc(buf_t b, uint32_t i, char c);
char   *utl_buf_addc(buf_t b, char c);
int16_t utl_buf_del(buf_t b, uint32_t i,  uint32_t j);
int     utl_buf_fmt(buf_t b, uint32_t i, const char *fmt, ...);

void utl_dpqsort(void *base, uint32_t nel, uint32_t esz, utl_cmp_t cmp, void *aux);
#define utlqsort(b,n,s,c,x) utl_dpqsort(b,n,s,c,x)

#define utlqseacrch(b)

#define sym_t vec_t

#define symNULL vec_MAX_CNT

#define symnew()           utl_sym_new()
#define symfree(t)         utl_sym_free(t)
#define symadd(t,s)        utl_sym_add(t,s)
#define symdel(t,i)        utl_sym_del(t,i)
#define symget(t,i)        utl_sym_get(t,i)
#define symsearch(t,s)     utl_sym_search(t,s)
#define symcount(t)        veccount(t)
#define symsetdata(t,i,n)  utl_sym_setdata(t,i,n)
#define symgetdata(t,i)    utl_sym_getdata(t,i)
#define symfirst(t)        vecfirst(uint32_t,t,symNULL)
#define symnext(t)         vecnext(uint32_t,t,symNULL)
#define symfreeze(f,t)     utl_frz(f,t,utl_sym_freeze)
#define symunfreeze(f)     utl_unfrz(f,NULL,NULL,utl_sym_unfreeze)

sym_t    utl_sym_unfreeze(FILE *f, utl_cmp_t cmp, utl_hsh_t hsh);
int      utl_sym_freeze(FILE *f, sym_t t);
int16_t  utl_sym_del(sym_t t, const char *sym);
uint32_t utl_sym_search(sym_t t, const char *sym);
uint32_t utl_sym_add(sym_t t, const char *sym);
char    *utl_sym_get(sym_t t,uint32_t id);
sym_t    utl_sym_free(sym_t t);
sym_t    utl_sym_new(void);
int32_t  utl_sym_getdata(sym_t t,uint32_t id);
int16_t  utl_sym_setdata(sym_t t,uint32_t id, int32_t val);

#endif 
#line 18 "src/utl_pmx.h"
#ifndef UTL_NOPMX

#define utl_pmx_MAXCAPT 16

#define pmxaction_t utl_txt_action_t

typedef struct {
  const char *pat;
  const char *txt;
  int32_t min_n;
  int32_t max_n;
  int32_t n;
  int16_t inv;
  int16_t cap; 
} utl_pmx_state_s;

typedef struct {
const char       *start;
const char       *capt[utl_pmx_MAXCAPT][2];
const char       *error;
uint16_t          capnum;
uint16_t          utf8;
uint16_t          csens;
utl_pmx_state_s   stack[utl_pmx_MAXCAPT];
uint16_t          stack_ptr;
} pmx_t;


extern int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch);

//extern const char  *utl_pmx_capt[utl_pmx_MAXCAPT][2];
//extern uint8_t      utl_pmx_capnum                  ;
//extern const char  *utl_pmx_error                   ;

extern pmx_t utl_pmx_ ;


#define pmxsearch(r,t)  utl_pmx_search(r,t,0)
#define pmxmatch(r,t)   utl_pmx_search(r,t,1)
#define pmxstart(n)    (utl_pmx_.capt[n][0])
#define pmxend(n)      (utl_pmx_.capt[n][1])
#define pmxcount()     (utl_pmx_.capnum)
#define pmxlen(n)       utl_pmx_len(n)
#define pmxerror()     (utl_pmx_.error?utl_pmx_.error:utl_emptystring)
#define pmxextend(f)   (void)(utl_pmx_ext = f)
#define pmxscan(r,t,f,a) utl_pmx_scan(r,t,f,a)

#define pmxclear(p_)    utl_pmx_save(p_)
#define pmxrestore(p_) utl_pmx_restore(p_)

void        utl_pmx_save(pmx_t *);
void        utl_pmx_restore(pmx_t *);

const char *utl_pmx_search(const char *pat, const char *txt, int fromstart);
int         utl_pmx_len(uint8_t n);
void        utl_pmx_extend(int(*ext)(const char *, const char *,int, int32_t));
const char *utl_pmx_scan(const char *pat, const char *txt, pmxaction_t func, void *aux);



#endif
#line 25 "src/utl_peg.h"
#ifndef UTL_NOPEG

struct peg_s;
typedef struct peg_s *peg_t;
typedef void (*pegrule_t)(peg_t,const char *);

struct peg_s {
  int32_t      fail;
  int32_t      auxi;
  void        *aux;
  const char  *start;
  const char  *pos;
  vec_t        defer;
  vec_t        mmz;
  const char  *errpos;
  const char  *errrule;
  const char  *errptr;
  const char  *errmsg;
  const char  *errmsgtmp;
  int32_t      errln;
  int32_t      errcn;
};

typedef struct  {
  int min;
  int max;
  int rpt;
  const char *pos;
  const char *rpos; 
  int dcnt;
  int rdcnt;
  int rlen;
} pegsave_t;
 
// As defined in utl_hdr.h:
//  void (*)(const char *, const char *, void *);
#define pegaction_t utl_txt_action_t
 
typedef struct {
  pegaction_t func;
  const char *from;
  const char *to;
} pegdefer_t;

typedef struct utl_peg_mmz_s {
  int32_t     fail;
  const char *startpos;
  const char *endpos;
  vec_t       defer;
} utl_peg_mmz_t;
 
int utl_peg_lower(const char *str);
int utl_peg_oneof(const char *pat, const char *str);
int utl_peg_str(const char *pat, const char *str);
int utl_peg_eol(const char *str);
int utl_peg_wspace(const char *str);
int utl_peg_vspace(const char *str);

void utl_peg_ref(peg_t, const char *, pegrule_t, utl_peg_mmz_t *);

int utl_peg_parse(peg_t, pegrule_t, utl_peg_mmz_t *, const char *, const char *, void *);

#define peg_parse(p,f,s,a) utl_peg_parse(p,PeG_##f,PeG_mmz_##f,s,#f,a)
#define pegparse(p,f,...) \
           peg_parse(p, f, \
                     utl_expand(utl_arg0(__VA_ARGS__,utl_emptystring)), \
                     utl_expand(utl_arg1(__VA_ARGS__,NULL,NULL))  ) 

const char *utl_peg_back(peg_t ,const char *, const char *,int32_t);

peg_t utl_peg_free(peg_t peg);
peg_t utl_peg_new(void);

#define pegnew() utl_peg_new()
#define pegfree(p) utl_peg_free(p)

#define PEG_FAIL     peg_->fail
#define PEG_POS      peg_->pos
#define PEG_AUX      peg_->aux
#define PEG_DCNT     veccount(peg_->defer)

#define PEG_BACK(p_,n_) utl_peg_back(peg_,pegr_,p_,n_)

#define utl_peg_rec(r_) if (!PEG_FAIL) {\
                          PEG_FAIL = r_;\
                          if (PEG_FAIL > 0) { \
                            PEG_POS += PEG_FAIL; \
                            PEG_FAIL = 0;\
                          } else {PEG_FAIL = -PEG_FAIL;} \
                          if (PEG_FAIL) {PEG_BACK(PEG_POS,PEG_DCNT);}\
                        } else (void)0

#define pegis(s_)       int s_(const char *, void*); utl_peg_rec(s_(PEG_POS,PEG_AUX))
#define pegstr(s_)      utl_peg_rec(utl_peg_str(s_,PEG_POS))
#define pegoneof(s_)    utl_peg_rec(utl_peg_oneof(s_,PEG_POS))
#define peglower        utl_peg_rec((islower((int)(*PEG_POS))?1:-1))
#define pegupper        utl_peg_rec((isupper((int)(*PEG_POS))?1:-1))
#define pegdigit        utl_peg_rec((isdigit((int)(*PEG_POS))?1:-1))
#define pegspace        utl_peg_rec((isspace((int)(*PEG_POS))?1:-1))
#define pegwspace       utl_peg_rec(utl_peg_wspace(PEG_POS))
#define pegvspace       utl_peg_rec(utl_peg_vspace(PEG_POS))

#define pegany          utl_peg_rec(((*PEG_POS)?1:-1))
#define pegeot          utl_peg_rec(((*PEG_POS)?-1:0))
#define pegeol          utl_peg_rec((utl_peg_eol(PEG_POS)))
#define pegsol          utl_peg_rec(((PEG_POS == peg_->start \
                                      || PEG_POS[-1] == '\n' \
                                      || PEG_POS[-1] == '\r')?0:-1))

#define pegpmx(s_)      utl_peg_rec((pmxmatch(s_,PEG_POS)?pmxlen(0):-1))
                                       
// PEG_BACK() is needed to ensure that errpos is updated (if it needs to be)
#define pegfail        (PEG_FAIL=!PEG_BACK(PEG_POS,PEG_DCNT))
#define pegempty       (PEG_FAIL=!PEG_POS)
#define pegerror       (PEG_FAIL=-(!PEG_BACK(PEG_POS,PEG_DCNT)))
#define pegfailed()    (peg_->fail)

#define utl_peg_rule(x_) void PeG_##x_(peg_t peg_, const char *pegr_)
#define pegrule(x_)      utl_peg_mmz_t PeG_mmz_##x_[2] = {{0}}; utl_peg_rule(x_)

#define pegref(x_)     do { \
                         extern utl_peg_mmz_t PeG_mmz_##x_[2]; \
                         utl_peg_rule(x_); \
                         utl_peg_ref(peg_,#x_, PeG_##x_, PeG_mmz_##x_); \
                       } while (0)
                              
const char *utl_peg_defer(peg_t, pegaction_t, const char *, const char *);

#define pegaction(f_)   int f_(const char *, const char *, void *); \
                        for(const char *tmp=PEG_POS; \
                            !PEG_FAIL && tmp; \
                              tmp=utl_peg_defer(peg_,f_,tmp,PEG_POS))

#define pegcheck(f_)    int f_(const char *, const char *,void *); \
                        for (pegsave_t peg_save={.pos=PEG_POS, .dcnt=PEG_DCNT};\
                             !PEG_FAIL && peg_save.pos; \
                                peg_save.pos = (PEG_FAIL || !f_(peg_save.pos,PEG_POS,PEG_AUX)) \
                                               ? PEG_BACK(peg_save.pos,peg_save.dcnt) \
                                               : NULL )


#define pegswitch pegalt
#define pegcase   pegor

#define pegalt  for (pegsave_t peg_save={.pos=PEG_POS, .dcnt=PEG_DCNT};\
                     !PEG_FAIL && peg_save.pos && (PEG_FAIL=1); \
                        peg_save.pos=PEG_FAIL \
                                     ? PEG_BACK(peg_save.pos,peg_save.dcnt) \
                                     : NULL)

#define pegchoice  pegalt
#define pegeither  pegor
#define pegor   if (PEG_FAIL > 0 && !(PEG_FAIL=0) && \
                    !PEG_BACK(peg_save.pos,peg_save.dcnt))
      
#define pegnot  for (pegsave_t peg_save={.pos=PEG_POS, .dcnt=PEG_DCNT}; \
                     !PEG_FAIL && peg_save.pos; \
                        PEG_FAIL=!PEG_FAIL, peg_save.pos= PEG_BACK(peg_save.pos,peg_save.dcnt))
                           
#define pegand  for (pegsave_t peg_save={.pos=PEG_POS, .dcnt=PEG_DCNT}; \
                      !PEG_FAIL && peg_save.pos;\
                       peg_save.pos=PEG_BACK(peg_save.pos,peg_save.dcnt))

#define PEG_RPT_SAVE(m_,M_) { .min=m_, .max=M_, .rpt=0, \
                              .pos=PEG_POS, .rpos=PEG_POS, \
                              .dcnt=PEG_DCNT, .rdcnt=PEG_DCNT }

void utl_peg_repeat(peg_t peg_, const char *pegr_, pegsave_t *peg_save);
#define pegrpt(m_,M_) \
  if (PEG_FAIL) (void)0; else \
  for(pegsave_t peg_save = PEG_RPT_SAVE(m_,M_); \
        peg_save.max > 0; \
          utl_peg_repeat(peg_, pegr_, &peg_save)) 
          
#define pegopt   pegrpt(0,1)
#define pegstar  pegrpt(0,INT_MAX)
#define pegmore  pegrpt(1,INT_MAX)
#define pegplus  pegrpt(1,INT_MAX)

#define pegsetaux(p_,a_) (p_->aux = (void *)(a_))

#define pegpos(p_)            ((p_)->pos)
#define pegstartpos(p_)       ((p_)->start)
#define pegfailpos(p_)        ((p_)->errpos)
#define pegfailrule(p_)       ((p_)->errrule)
#define pegfaillinenum(p_)    ((p_)->errln)
#define pegfailline(p_)       ((p_)->errptr)
#define pegfailcolumn(p_)     ((p_)->errcn)
#define pegfailsetmessage(s_) if (PEG_FAIL) (void)0; else (peg_->errmsgtmp = (s_))
#define pegfailmessage(p_)    (((p_)->errmsg) ? ((p_)->errmsg) :  ((p_)->errrule))
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
                              logwarning("Exception: %d (%d)",utl_unpow2(x),y);\
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

