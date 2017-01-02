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

typedef struct vec_s {
  uint32_t  fst; 
  uint32_t  lst; 
  uint32_t  max;
  uint32_t  cnt;
  uint16_t  esz;  /* each element max size: 64K */
  uint16_t  flg;
  int     (*cmp)(void *, void *);
  int     (*hsh)(void *);
  uint8_t  *vec;
  void     *elm; // this will always point to eld
  uint8_t   eld[4];
} vec_s, *vec_t;

#define vecDO(type,v,i,e,x)  do {vec_t v_=v;  *((type *)(v_->elm)) = (e); x(v_,i);} while (0)

// Random access
#define vecset(type,v,i,e)   vecDO(type,v,i,e,utl_vec_set)
#define vecins(type,v,i,e)   vecDO(type,v,i,e,utl_vec_ins)
#define vecget(type,v,i)     (type *)utl_vec_get(v,i)
#define vecdel(v,i)          utl_vec_del(v,i)

// Set (sorted or unsorted) 
#define vecadd(type,v,e)     vecDO(type,v,vec_MAX_CNT,e,utl_vec_add)
#define vecsearch(type,v,e)  (type *)utl_vec_search(v, (*((type *)(v->elm)) = (e), 0))  

#define vecsort(...)  utl_vec_sort(utl_arg0(__VA_ARGS__,NULL), utl_arg1(__VA_ARGS__,utl_vec_nullcmp,NULL))
                                                                                                                                                  
#define vecSORTED 0x0001
#define vecHASHED 0x0020

#define vecissorted(v) ((v)->flg &   vecSORTED)
#define vecsorted(v)   ((v)->flg |=  vecSORTED)
#define vecunsorted(v) ((v)->flg &= ~vecSORTED)

// Stack policy
#define vecpush(type,v,e)    vecins(type,v,vec_MAX_CNT,e)
#define vecdrop(v)           do {vec_t v_=v; if (v_->cnt) v_->cnt--;} while (0)
#define vectop(type,v)       vecget(type,v,vec_MAX_CNT)    

// Queue
#define vecenq(type,v,e)
#define vecdeq(v)

// I/O
#define vecread(v,i,n,f)  utl_vec_read(v,i,n,f)
#define vecwrite(v,i,n,f) utl_vec_write(v,i,n,f)

// Info
#define vecnew(type)   utl_vec_new(sizeof(type))
#define vecfree(v)     utl_vec_free(v)
#define veccount(v)    ((v)->cnt)
#define vecmax(v)      ((v)->max)
#define vecisempty(v)  ((v)->cnt == 0)
#define vec(type,v)    ((type *)((v)->vec))

// Protypes
void *utl_vec_set(vec_t v, uint32_t i);
void *utl_vec_ins(vec_t v, uint32_t i);
void *utl_vec_add(vec_t v, uint32_t i);
void *utl_vec_get(vec_t v, uint32_t i);

int16_t utl_vec_del(vec_t v,  uint32_t i);
int16_t utl_vec_delrange(vec_t v, uint32_t i,  uint32_t j);

vec_t utl_vec_new(uint16_t esz);
vec_t utl_vec_free(vec_t v);

size_t utl_vec_read(vec_t v,uint32_t i, size_t n,FILE *f);
size_t utl_vec_write(vec_t v, uint32_t i, size_t n, FILE *f);

void utl_vec_sort(vec_t v, int (*cmp)(void *, void *));
void *utl_vec_search(vec_t v,int x);

int utl_vec_nullcmp(void *a, void *b);

// Character buffer
#define buf_t                 vec_t
#define bufnew()              vecnew(char)
#define bufaddc(b,c)          vecpush(char,b,c)
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
//>>>//
