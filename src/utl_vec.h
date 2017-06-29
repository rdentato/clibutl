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

//<<<//
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
//>>>//
