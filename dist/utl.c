#line 2 "src/utl_hdr.c"
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
*/
#line 92 "src/utl_hdr.c"
#include "utl.h"
#define UTL_MAIN

const char *utl_emptystring = "";

int   utl_ret(int x)      {return x;}
void *utl_retptr(void *x) {return x;}
#line 286 "src/utl_log.c"
#ifndef UTL_NOLOG
#ifdef UTL_MAIN

FILE *utl_log_file = NULL;
uint32_t utl_log_check_num   = 0;
uint32_t utl_log_check_fail  = 0;

char *utl_log_watch[1] = {""};

int utl_log_close(const char *msg)
{
  int ret = 0;
  
  if (utl_log_check_num) {
    logprintf("CHK #KO: %d (of %d)",utl_log_check_fail,utl_log_check_num);
    utl_log_check_fail = 0;
    utl_log_check_num = 0;
  }
  if (msg) logprintf("%s",msg);
  if (utl_log_file && utl_log_file != stderr) ret = fclose(utl_log_file);
  utl_log_file = NULL;
  return ret;
}

FILE *utl_log_open(const char *fname, const char *mode)
{
  char md[4];
  md[0] = (mode && *mode == 'w')? 'w' : 'a';
  md[1] = '+';
  md[2] = '\0';
  utl_log_close(NULL);
  utl_log_file = fopen(fname,md);
  logprintf("LOG START");
  utl_log_check_num = 0;
  utl_log_check_fail = 0;
  return utl_log_file;
}

int utl_log_time(void)
{
  char       log_tstr[32];
  time_t     log_time;
  struct tm *log_time_tm;
  int        ret = 0;
  
  if (!utl_log_file) utl_log_file = stderr;
  if (time(&log_time) == ((time_t)-1)) ret = -1;
  if (ret >= 0 && !(log_time_tm = localtime(&log_time))) ret = -1;
  if (ret >= 0 && !strftime(log_tstr,32,"%Y-%m-%d %H:%M:%S",log_time_tm)) ret =-1;
  if (ret >= 0) ret = fprintf(utl_log_file,"%s ",log_tstr);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  
  return ret;
}

int utl_log_check(int res, const char *test, const char *file, int32_t line)
{
  int ret = 0;
  ret = utl_log_time();
  
  if (ret >= 0) ret = fprintf(utl_log_file,"CHK %s (%s)?\x09:%s:%d\x09\n", (res?"PASS":"FAIL"), test, file, line);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  if (!res) utl_log_check_fail++;
  utl_log_check_num++;
  return res;
}

void utl_log_assert(int res, const char *test, const char *file, int32_t line)
{
  if (!utl_log_check(res,test,file,line)) {
    logprintf("CHK EXITING ON FAIL");
    logclose();
    abort();
  }
}

void utl_log_trc_check(char *buf, char *watch[], const char *file, int32_t line)
{ 
  int k=0;
  char *p;
  int expected = 0;
  int res = 0;
  _logprintf("XXX %s",buf);
  for (k=0; k<UTL_LOG_WATCH_SIZE; k++) {
    if (watch[k]) {
      p = watch[k];
      if (p[0] == '\0') break;
      expected = !((p[0] == '<') && (p[1] == 'n') && (p[2] == 'o') && (p[3] == 't') && (p[4] == '>'));
      if (!expected) p+=5;
    	res = pmxsearch(p,buf) != NULL;
      if (res) {
        utl_log_check(expected,watch[k],file,line);
        if (expected) watch[k] = NULL;
      }
    }
  }
}

void utl_log_trc_check_last(char *watch[], const char *file, int32_t line)
{ 
  /* The  only tests in `watch[]` should be the ones with `<not>` at the beginning */
  int k;
  int expected = 0;
  char *p;
  
  for (k=0; k<UTL_LOG_WATCH_SIZE;k++) {
    if (watch[k]) {
      p = watch[k];
      if (p[0] == '\0') break;
      expected = !((p[0] == '<') && (p[1] == 'n') && (p[2] == 'o') && (p[3] == 't') && (p[4] == '>'));
      utl_log_check(!expected,watch[k],file,line);
    }
  }
}

#endif
#endif
#line 41 "src/utl_mem.c"
#ifndef UTL_NOMEM
#ifdef UTL_MAIN

#ifndef memINVALID
#define memINVALID    -2
#define memOVERFLOW   -1
#define memVALID       0
#define memNULL        1
#endif

static const char  *utl_BEG_CHK = "\xBE\xEF\xF0\x0D";
static const char  *utl_END_CHK = "\xDE\xAD\xC0\xDA";
static const char  *utl_CLR_CHK = "\xDE\xFA\xCE\xD0";
static size_t utl_mem_allocated;

typedef struct {
   size_t size;
   char   chk[4];
   char   blk[4];
} utl_mem_t;

#define utl_mem(x) ((utl_mem_t *)((char *)(x) -  offsetof(utl_mem_t, blk)))

int utl_check(void *ptr,const char *file, int32_t line)
{
  utl_mem_t *p;
  
  if (ptr == NULL) return memNULL;
  p = utl_mem(ptr);
  if (memcmp(p->chk,utl_BEG_CHK,4)) { 
    logprintf("TRC Invalid or double freed %p (%lu)\x09:%s:%d\x09",p->blk,
                                               (unsigned long)utl_mem_allocated, file, line);     
    return memINVALID; 
  }
  if (memcmp(p->blk+p->size,utl_END_CHK,4)) {
    logprintf("TRC Boundary overflow %p [%lu] (%lu)\x09:%s:%d\x09",
                              p->blk, (unsigned long)p->size, (unsigned long)utl_mem_allocated, file, line); 
    return memOVERFLOW;
  }
  logprintf("TRC Valid pointer %p (%lu)\x09:%s:%d\x09",ptr, (unsigned long)utl_mem_allocated, file, line); 
  return memVALID; 
}

void *utl_malloc(size_t size, const char *file, int32_t line )
{
  utl_mem_t *p;
  
  if (size == 0) logprintf("TRC Request for 0 bytes (%lu)\x09:%s:%d\x09",
                                                (unsigned long)utl_mem_allocated, file, line);
  p = (utl_mem_t *)malloc(sizeof(utl_mem_t) +size);
  if (p == NULL) {
    logprintf("TRC Out of Memory (%lu)\x09:%s:%d\x09",(unsigned long)utl_mem_allocated, file, line);
    return NULL;
  }
  p->size = size;
  memcpy(p->chk,utl_BEG_CHK,4);
  memcpy(p->blk+p->size,utl_END_CHK,4);
  utl_mem_allocated += size;
  logprintf("TRC Allocated %p [%lu] (%lu)\x09:%s:%d\x09",p->blk,(unsigned long)size,(unsigned long)utl_mem_allocated,file,line);
  return p->blk;
}

void *utl_calloc(size_t num, size_t size, const char *file, int32_t line)
{
  void *ptr;
  
  size = num * size;
  ptr = utl_malloc(size,file,line);
  if (ptr)  memset(ptr,0x00,size);
  return ptr;
}

void utl_free(void *ptr, const char *file, int32_t line)
{
  utl_mem_t *p=NULL;
  
  switch (utl_check(ptr,file,line)) {
    case memNULL  :    logprintf("TRC free NULL (%lu)\x09:%s:%d\x09", 
                                                (unsigned long)utl_mem_allocated, file, line);
                       break;
                          
    case memOVERFLOW : logprintf("TRC Freeing an overflown block  (%lu)\x09:%s:%d\x09", 
                                                           (unsigned long)utl_mem_allocated, file, line);
    case memVALID :    p = utl_mem(ptr); 
                       memcpy(p->chk,utl_CLR_CHK,4);
                       utl_mem_allocated -= p->size;
                       if (p->size == 0)
                         logprintf("TRC Freeing a block of 0 bytes (%lu)\x09:%s:%d\x09", 
                                             (unsigned long)utl_mem_allocated, file, line);

                       logprintf("TRC free %p [%lu] (%lu)\x09:%s:%d\x09", ptr, 
                                 (unsigned long)(p?p->size:0),(unsigned long)utl_mem_allocated, file, line);
                       free(p);
                       break;
                          
    case memINVALID :  logprintf("TRC free an invalid pointer! (%lu)\x09:%s:%d\x09", 
                                                (unsigned long)utl_mem_allocated, file, line);
                       break;
  }
}

void *utl_realloc(void *ptr, size_t size, const char *file, int32_t line)
{
  utl_mem_t *p;
  
  if (size == 0) {
    logprintf("TRC realloc() used as free() %p -> [0] (%lu)\x09:%s:%d\x09",
                                                      ptr,(unsigned long)utl_mem_allocated, file, line);
    utl_free(ptr,file,line); 
  } 
  else {
    switch (utl_check(ptr,file,line)) {
      case memNULL   : logprintf("TRC realloc() used as malloc() (%lu)\x09:%s:%d\x09", 
                                             (unsigned long)utl_mem_allocated, file, line);
                          return utl_malloc(size,file,line);
                        
      case memVALID  : p = utl_mem(ptr); 
                       p = (utl_mem_t *)realloc(p,sizeof(utl_mem_t) + size); 
                       if (p == NULL) {
                         logprintf("TRC Out of Memory (%lu)\x09:%s:%d\x09", 
                                          (unsigned long)utl_mem_allocated, file, line);
                         return NULL;
                       }
                       utl_mem_allocated -= p->size;
                       utl_mem_allocated += size; 
                       logprintf("TRC realloc %p [%lu] -> %p [%lu] (%lu)\x09:%s:%d\x09", 
                                       ptr, (unsigned long)p->size, p->blk, (unsigned long)size, 
                                       (unsigned long)utl_mem_allocated, file, line);
                       p->size = size;
                       memcpy(p->chk,utl_BEG_CHK,4);
                       memcpy(p->blk+p->size,utl_END_CHK,4);
                       ptr = p->blk;
                       break;
    }
  }
  return ptr;
}

void *utl_strdup(const char *ptr, const char *file, int32_t line)
{
  char *dest;
  size_t size;
  
  if (ptr == NULL) {
    logprintf("TRC strdup NULL (%lu)\x09:%s:%d\x09", (unsigned long)utl_mem_allocated, file, line);
    return NULL;
  }
  size = strlen(ptr)+1;

  dest = (char *)utl_malloc(size,file,line);
  if (dest) memcpy(dest,ptr,size);
  logprintf("TRC strdup %p [%lu] -> %p (%lu)\x09:%s:%d\x09", ptr, (unsigned long)size, dest, 
                                                (unsigned long)utl_mem_allocated, file, line);
  return dest;
}
#undef utl_mem

size_t utl_mem_used(void) {return utl_mem_allocated;}

#endif
#endif
#line 21 "src/utl_vec.c"
#ifndef UTL_NOVEC
#ifdef UTL_MAIN

int utl_vec_nullcmp(void *a, void *b){return 0;}

static int16_t utl_vec_makeroom(vec_t v,uint32_t n)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;

  if (n < v->max) return 1;
  new_max = v->max;
  while (new_max <= n) new_max += (new_max / 2);  /*  (new_max *= 1.5) instead of (new_max *= 2) */
  new_vec = (uint8_t *)realloc(v->vec, new_max * v->esz);
  if (!new_vec) return 0;
  v->vec = new_vec;  v->max = new_max;
  return 1;
}

static int16_t utl_vec_makegap(vec_t v, uint32_t i, uint32_t l)
{
 
  if (!utl_vec_makeroom(v,v->cnt + l)) return 0;
  
  /*
  **                    __l__
  **  ABCDEFGH       ABC-----DEFGH
  **  |  |    |      |  |    |    |
  **  0  i    cnt    0  i   i+l   cnt+l
  */
  
  if (i < v->cnt) {
    memmove(v->vec + (i+l)*v->esz,  v->vec + i*v->esz,  (v->cnt-i)*v->esz);
    v->cnt += l;
  }
  return 1;
}


static int16_t utl_vec_delgap(vec_t v, uint32_t i, uint32_t l)
{
  
  /*
  **       __l__                                
  **    ABCdefghIJKLM              ABCIJKLM     
  **    |  |    |    |             |  |    |    
  **    0  i   i+l   cnt           0  i    cnt-l  
  */
  _logdebug("DELGAP: %d %d",i,l);
  if (i < v->cnt) {
    if (i+l >= v->cnt) v->cnt = i; /* Just drop last elements */
    else {
      memmove(v->vec + i*v->esz,  v->vec + (i+l)*v->esz,  (v->cnt-(i+l))*v->esz);
      v->cnt -= l;
    }
  }
  return 1;
}


vec_t utl_vec_new(uint16_t esz)
{
  vec_t v = NULL;
  uint32_t sz = sizeof(vec_s)+(esz-4); // 4 is the size of the elm array
  
  v = (vec_t)malloc(sz);
  if (v) {
    memset(v,0,sz);
    v->esz = esz;
    v->max = vec_MIN_ELEM;
    v->vec = (uint8_t *)malloc(v->max * esz);
    v->elm = ((char *)v) + offsetof(vec_s,eld);
    vecunsorted(v);
    if (!v->vec) { free(v); v = NULL;}
  }
  return v;
}

vec_t utl_vec_free(vec_t v)
{
  if (v) { 
    if (v->vec) free(v->vec);
    v->vec = NULL;
    free (v);
  }
  return NULL;
}

void *utl_vec_get(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  
  if (i == vec_MAX_CNT) i = v->cnt -1;
  if (i < v->cnt) {
    elm = v->vec + (i*v->esz);
    memcpy(v->elm,elm,v->esz);
  }
  return elm;
}

void *utl_vec_set(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (utl_vec_makeroom(v,i)) {
    elm = v->vec + (i*v->esz);
    memcpy(elm, v->elm, v->esz);
    if (i >= v->cnt) v->cnt = i+1;
    vecunsorted(v);
  }
  return elm;
}

void *utl_vec_ins(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (i == vec_MAX_CNT) i = v->cnt;
  if (utl_vec_makegap(v,i,1))
    elm = (uint8_t *)utl_vec_set(v,i);
  vecunsorted(v);
  return elm;
}

static void *utl_vec_add_sorted(vec_t v)
{
  uint8_t *elm=NULL;
  int32_t mid,lo,hi;
  int ret;
  
  vecsort(v); 
  lo = 0; hi = v->cnt-1;
  while (lo <= hi) {
    mid = lo + (hi-lo)/2;
    elm = v->vec + mid * v->esz;
    ret = v->cmp(v->elm,elm);
    if (ret == 0) {
      memcpy(elm, v->elm, v->esz);
      return elm;
    } 
    if (ret < 0) hi = mid-1;
    else         lo = mid+1;
  }
  // Not found. `lo` points to the first element greater than the one to add
  // (or just past the last element)
  //logtrace("add: %d at %d size: %d max:%d",*((int *)v->elm),lo,v->cnt,v->max);
  return utl_vec_ins(v,lo);
}

void *utl_vec_add(vec_t v, uint32_t i) {
  if (v->cnt > 0) {
    if (v->cmp) return utl_vec_add_sorted(v);
  }
  return utl_vec_set(v,v->cnt);
}

int16_t utl_vec_del(vec_t v, uint32_t i)
{
  return utl_vec_delgap(v,i,1);
}

int16_t utl_vec_delrange(vec_t v, uint32_t i,  uint32_t j)
{
  if (j<i) return 0;
  return utl_vec_delgap(v,i,j-i+1);  
}

size_t utl_vec_read(vec_t v,uint32_t i, size_t n,FILE *f)
{
  if (utl_vec_makeroom(v,i+n+1)) {
    n = fread(v->vec + (i*v->esz), v->esz, n, f);
    if (v->cnt < i+n) v->cnt = i+n;
    vecunsorted(v);
  }
  return 0;
}

size_t utl_vec_write(vec_t v, uint32_t i, size_t n, FILE *f)
{
  if (i+n> v->max) n = v->max - i;
  return fwrite(v->vec+(i*v->esz),v->esz,n,f);
}

void utl_vec_sort(vec_t v, int (*cmp)(void *, void *))
{
  if (cmp != utl_vec_nullcmp && cmp != v->cmp) {
    v->cmp = cmp;
    vecunsorted(v);
  }
  if (v->cmp == NULL) {
    vecunsorted(v);
  }
  else if (!vecissorted(v)) {
    if (v->cnt > 1)
      qsort(v->vec,v->cnt,v->esz,(int (*)(const void *, const void *))(v->cmp));  
    vecsorted(v);
  }
}

void *utl_vec_search(vec_t v, int x)
{
  if (v->cmp) {
    vecsort(v);
    return bsearch(v->elm,v->vec,v->cnt,v->esz,(int (*)(const void *, const void *))(v->cmp));
  }
  return NULL;
}



/* ** BUF ********************* */

char utl_buf_get(buf_t b, uint32_t n)
{
  char *s = vecget(char,b,n);
  return s?*s:'\0';
}

size_t utl_buf_read(buf_t b, uint32_t i, uint32_t n, FILE *f)
{
  size_t r = vecread(b,i,n,f);
  bufsetc(b,i+n,'\0');
  b->cnt = i+n;
  return r;
}

size_t utl_buf_readall(buf_t b, uint32_t i, FILE *f)
{
  uint32_t n,pos;
  size_t ret;
  pos = ftell(f);
  fseek(f,0,SEEK_END);
  n = ftell(f);
  fseek(f,pos,SEEK_SET);
  ret = bufread(b,i,n-pos,f);
  bufsetc(b,i+(n-pos),'\0');
  b->cnt = i+(n-pos);
  return ret;
}

char *utl_buf_readln(buf_t b, uint32_t i, FILE *f)
{
  int c;
  uint32_t n = i;
  
  if (!b || !f || feof(f)) return NULL;
  while ((c=fgetc(f)) != EOF) {
    if (c == '\r') {
      c = fgetc(f);
      if (c != '\n') ungetc(c,f);
      c = '\n';
    }
    bufsetc(b,i++,c);
    if (c=='\n') break;
  }
  if (i==n) return NULL;
  if (bufgetc(b,i-1) != '\n') bufsetc(b,i++,'\n');
  bufsetc(b,i,'\0');
  b->cnt = i;
  return buf(b)+n;
}

char *utl_buf_sets(buf_t b, uint32_t i, const char *s)
{
  char *r = buf(b)+i;
  while (*s) bufsetc(b,i++,*s++);
  bufsetc(b,i,'\0');
  b->cnt=i;
  return r;
}

char *utl_buf_inss(buf_t b, uint32_t i, const char *s)
{
  uint32_t n;
  char *p;
  if (!s || !b) return NULL;
  n = strlen(s);
  
  if (n == 0 || !utl_vec_makegap(b, i, n)) return NULL;
    
  p = buf(b)+i;
  while (*s) *p++ = *s++;

  bufsetc(b,b->cnt,'\0');  b->cnt--;
  return buf(b)+i;
}

char *utl_buf_insc(buf_t b, uint32_t i, char c)
{
  if (!utl_vec_makegap(b, i, 1)) return NULL;
    
  buf(b)[i] = c;
  bufsetc(b,b->cnt,'\0');  b->cnt--;
  return buf(b)+i;
}

int16_t utl_buf_del(buf_t b, uint32_t i,  uint32_t j)
{
  int16_t r;
  _logdebug("len:%d",b->cnt);
  r = utl_vec_delgap(b, i, j-i+1);
  _logdebug("len:%d",b->cnt);
  if (r) {
    bufsetc(b,b->cnt,'\0');
    b->cnt--;
  }
  return r;
}

#endif
#endif
#line 376 "src/utl_pmx.c"
#ifndef UTL_NOPMX
#ifdef UTL_MAIN

int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch) = NULL;

const char *utl_pmx_capt[utl_pmx_MAXCAPT][2] = {{0}} ;
uint8_t     utl_pmx_capnum                   =   0   ;
const char *utl_pmx_error                    = NULL  ;


#define utl_pmx_set_paterror(t) do {if (!utl_pmx_error) {utl_pmx_error = t;}} while (0)

static int utl_pmx_utf8 = 0;
static int utl_pmx_case = 1; // assume case sensitive

#define utl_pmx_FAIL       goto fail

#define utl_pmx_newcap(t) do {                                       \
                            if (utl_pmx_capnum < utl_pmx_MAXCAPT) {  \
                              utl_pmx_capt[utl_pmx_capnum][0] =      \
                              utl_pmx_capt[utl_pmx_capnum][1] = (t); \
                              utl_pmx_capnum++;                      \
                            }                                        \
                          } while(0)

typedef struct {
  const char *pat;
  const char *txt;
  int32_t min_n;
  int32_t max_n;
  int32_t n;
  int16_t inv;
  int16_t cap; 
} utl_pmx_state_s;

utl_pmx_state_s utl_pmx_stack[utl_pmx_MAXCAPT];
uint8_t utl_pmx_stack_ptr = 0;

static void utl_pmx_state_reset(void)
{
  utl_pmx_stack_ptr = 0;
  utl_pmx_capnum = 0;
}

static int utl_pmx_state_push(const char *pat, const char *txt, int32_t min_n, int32_t max_n, int16_t inv)
{
  utl_pmx_state_s *state;
  
  if (utl_pmx_stack_ptr >= utl_pmx_MAXCAPT) return 0;
  
  state = utl_pmx_stack + utl_pmx_stack_ptr;
  
  state->pat   = pat;
  state->txt   = txt;
  state->min_n = min_n;
  state->max_n = max_n;
  state->n     = 0;
  state->inv   = inv;
  state->cap   = utl_pmx_capnum;
  
  utl_pmx_newcap(txt);
  utl_pmx_stack_ptr++;
  
  return 1;
}

static int utl_pmx_state_pop(void)
{
  if (utl_pmx_stack_ptr == 0) return 0;
  utl_pmx_stack_ptr--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top(void)
{
  if (utl_pmx_stack_ptr == 0) return NULL;
  return utl_pmx_stack + (utl_pmx_stack_ptr-1);
}

int utl_pmx_len(uint8_t n) {return (int)(pmxend(n)-pmxstart(n));}

static int utl_pmx_get_utf8(const char *txt, int32_t *ch)
{
  int len;
  uint8_t *s = (uint8_t *)txt;
  uint8_t first = *s;
  int32_t val;
  
  _logdebug("About to get UTF8: %s in %p",txt,ch);  
  fsm {
    fsmSTART {
      if (*s <= 0xC1) { val = *s; len = (*s > 0); fsmGOTO(end);    }
      if (*s <= 0xDF) { val = *s & 0x1F; len = 2; fsmGOTO(len2);   }
      if (*s == 0xE0) { val = *s & 0x0F; len = 3; fsmGOTO(len3_0); }
      if (*s <= 0xEC) { val = *s & 0x0F; len = 3; fsmGOTO(len3_1); }
      if (*s == 0xED) { val = *s & 0x0F; len = 3; fsmGOTO(len3_2); }
      if (*s <= 0xEF) { val = *s & 0x0F; len = 3; fsmGOTO(len3_1); }
      if (*s == 0xF0) { val = *s & 0x07; len = 4; fsmGOTO(len4_0); }
      if (*s <= 0xF3) { val = *s & 0x07; len = 4; fsmGOTO(len4_1); }
      if (*s == 0xF4) { val = *s & 0x07; len = 4; fsmGOTO(len4_2); }
      fsmGOTO(invalid);
    } 
    
    fsmSTATE(len4_0) {
      s++; if ( *s < 0x90 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_1) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_2) {
      s++; if ( *s < 0x80 || 0x8f < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len3_0) {
      s++; if ( *s < 0xA0 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len3_1) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }

    fsmSTATE(len3_2) {
      s++; if ( *s < 0x80 || 0x9f < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len2) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(end);
    }
    
    fsmSTATE(invalid) {val = first; len = 1;}
    
    fsmSTATE(end)   { }
  }
  if (ch) *ch = val;
  return len;
}

// Returns the length in bytes of the character or 0 if it is '\0'
static int32_t utl_pmx_nextch(const char *t, int32_t *c_ptr)
{
  int32_t len = 0;
  
  if (utl_pmx_utf8) len = utl_pmx_get_utf8(t, c_ptr);
  else if ((*c_ptr = (uint8_t)(*t)) != 0) len = 1;
  
  return len;
}

static int32_t utl_pmx_gethex(const char *pat, int32_t *c_ptr)
{
  int32_t ch =0;
  int32_t len =0;
  
  while (*pat && (*pat != '>') && !isxdigit(*pat)) {len++; pat++;}
  while (*pat) {
         if ('0'<= *pat && *pat <= '9') ch = (ch << 4) + (*pat -'0');
    else if ('A'<= *pat && *pat <= 'F') ch = (ch << 4) + (*pat -'A'+10);
    else if ('a'<= *pat && *pat <= 'f') ch = (ch << 4) + (*pat -'a'+10);
    else break;
    len++; pat++;
  }
  *c_ptr = ch;
  return len;  
}

static int utl_pmx_isin(const char *pat, const char *pat_end, int32_t ch, int32_t (*nxt)(const char *, int32_t *))
{
  int32_t c1,c2;
  int32_t len;
  
  while (pat<pat_end) {
    c2=0; c1=0;
    len = nxt(pat, &c1);
    if (ch == c1) return 1;
    pat +=len; 
    if (*pat == '-') {
      len = nxt(++pat, &c2);
      if (c1 <= ch && ch <= c2) return 1;
      pat += len; 
    }
  }
  return 0;
}

#define utl_pmx_isin_chars(p,e,c) utl_pmx_isin(p,e,c,utl_pmx_nextch)
#define utl_pmx_isin_codes(p,e,c) utl_pmx_isin(p,e,c,utl_pmx_gethex)

static int32_t utl_pmx_iscapt(const char *pat, const char *txt)
{
  int32_t len = 0;
  uint8_t capnum = 0; 
  const char *cap;
  
  if ('1' <= *pat && *pat <= '9') {
    capnum = *pat - '0';
    _logdebug("capt: %d %d",capnum,utl_pmx_capnum);
    if (capnum < utl_pmx_capnum) {
      cap = pmxstart(capnum);
      while (cap < pmxend(capnum) && *cap && (*cap == *txt)) {
        len++; txt++; cap++;
      }
      if (cap < pmxend(capnum)) len = 0;
    }
  }
  return len;
}

void utl_pmx_extend(int(*ext)(const char *, const char *, int, int32_t))
{
  utl_pmx_ext = ext;
}

#define UTL_PMX_QUOTED 0
#define UTL_PMX_BRACED 1

static int utl_pmx_get_limits(const char *pat, const char *pat_end, const char *txt, int braced,
                             int32_t *c_beg_ptr, int32_t *c_end_ptr, int32_t *c_esc_ptr)
{
  int32_t c_beg = '(';
  int32_t c_end = ')';
  int32_t c_esc = '\0';
  int32_t ch;
  
  _logdebug("BRACE: [%.*s]",pat_end-pat,pat);
  
  if (pat < pat_end) { /* <B()\> <Q""\>*/
    pat += utl_pmx_nextch(pat,&c_esc);
    if (pat < pat_end) {
      c_beg = c_esc; c_esc = '\0';
      pat += utl_pmx_nextch(pat,&c_end);
    }
    if (pat < pat_end) {
      pat += utl_pmx_nextch(pat,&c_esc);
    }
  }
  else {  /* Just <B> or <Q>, try to infer the braces */
    c_beg = '\0';
    (void)utl_pmx_nextch(txt,&ch);
    if (braced) {
           if (ch == '(')    {c_beg=ch; c_end=')';}
      else if (ch == '[')    {c_beg=ch; c_end=']';}
      else if (ch == '{')    {c_beg=ch; c_end='}';}
      else if (ch == '<')    {c_beg=ch; c_end='>';}
      else if (ch == 0x2039) {c_beg=ch; c_end=0x203A;} /* Unicode single quotes */
      else if (ch == 0x27E8) {c_beg=ch; c_end=0x27E9;} /* Unicode MATHEMATICAL ANGLE BRACKETS */
      else if (ch == 0x27EA) {c_beg=ch; c_end=0x27EB;} /* Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS */
    }
    else { // Quoted string
      c_esc = '\\';
           if (ch == '"')    {c_beg=ch; c_end=ch;}
      else if (ch == '\'')   {c_beg=ch; c_end=ch;}
      else if (ch == '`')    {c_beg=ch; c_end=ch;}
      else if (ch == 0x2018) {c_beg=ch; c_end=0x2019;} /* Unicode single quotes */
      else if (ch == 0x201C) {c_beg=ch; c_end=0x201D;} /* Unicode double quotes */
    }
    if (c_beg=='\0') {
           if (ch == '\xAB') {c_beg=ch; c_end='\xBB';} /* Unicode and ISO-8859-1 "<<" and ">>" */
      else if (ch == 0x2329) {c_beg=ch; c_end=0x232A;} /* Unicode ANGLE BRACKETS */
      else return 0;
    }
  }
  _logdebug("open:'%d' close:'%d' esc:'%d'",c_beg,c_end,c_esc);
  
  *c_beg_ptr = c_beg;
  *c_end_ptr = c_end;
  *c_esc_ptr = c_esc;
  return 1;
}

static int utl_pmx_get_delimited(const char *pat, const char *txt,int32_t c_beg, int32_t c_end, int32_t c_esc)
{
  int n;
  const char *s;
  int cnt;
  int32_t ch;
  
  s = txt;
  n = utl_pmx_nextch(s,&ch);
  if (n == 0 || ch != c_beg) return 0;
  cnt = 0;
  do {
    s += n;
    n = utl_pmx_nextch(s,&ch);
    if (ch == '\0') return 0;
    _logdebug("BRACE: '%c' cnt:%d",ch,cnt);
    
         if (ch == c_end) { if (cnt == 0) return (s+n)-txt;  else cnt--; }
    else if (ch == c_beg) { cnt++;                                       }
    else if (ch == c_esc) { s += n; n = utl_pmx_nextch(s,&ch);           }
    
  } while (ch);
  utl_pmx_set_paterror(pat);
  
  return s-txt;
  
}

static int utl_pmx_delimited(const char *pat, const char *pat_end, const char *txt, int braced)
{
  int32_t c_beg; int32_t c_end; int32_t c_esc;
  if (!utl_pmx_get_limits(pat,pat_end,txt, braced, &c_beg, &c_end, &c_esc)) return 0;
  return utl_pmx_get_delimited(pat,txt,c_beg,c_end,c_esc);  
}


/*
  Dealing with text encoding is a complex business. The most basic
issue for pmx is to deal with lower/upper case characters.

  Even just restricting to the main scripts that have the lower/upper
case distinction (Latin, Greek and Cyrillic) and the major encodings
(Unicode, ISO/IEC, Windows code pages, ...) would provide something
that could be of little use for somebody and of no use for many.

  So, I went for the easiest solution: the Latin-1
characters in the iso-8859-1 and Unicode Latin-1 supplement.
In other words: the characters encoded in a single byte.

  We need to extend the functions `islower()`, `isupper()`, `isalpha()`,
`isalnum()` to include the letters in the range 0xA0-0xFF.

  I've decided to not include the "numeric" caharacters for
superscript or fractions, It seeems counterintuitive to me that
`isdigit(0xBD); // 1/2` returns true. 
  
  To represent this encoding, we need four bits for each character:

    xxxx
    \\\
     \\\_____ isupper 
      \\_____ islower
       \_____ isdigit     
       
  This allows using a table with 128 bytes rather than 256.
*/

static unsigned char utl_ENCODING[] = {
       /*  10   32   54   76   98   BA   DC   FE */
/* 0_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 1_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 2_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 3_ */ 0x88,0x88,0x88,0x88,0x88,0x00,0x00,0x00,
/* 4_ */ 0x20,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
/* 5_ */ 0x22,0x22,0x22,0x22,0x22,0x02,0x00,0x00,
/* 6_ */ 0x40,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
/* 7_ */ 0x44,0x44,0x44,0x44,0x44,0x04,0x00,0x00,
/* 8_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 9_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* A_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* B_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* C_ */ 0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
/* D_ */ 0x22,0x22,0x22,0x02,0x22,0x22,0x22,0x22,
/* E_ */ 0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
/* F_ */ 0x44,0x44,0x44,0x04,0x44,0x44,0x44,0x44
};

/*
  Note that the table above is arranged so to make easy writing the
macro below.
Characters with odd code (i.e. ending with 1) are represented in the
higher half of the byte. So, the last bit of the code can be used to
shift right the byte and pick the higher half.

  To make the macro below a little bit less obscure:
  
    - The byte to pick from the table for character c is c/2 (i.e. c>>1)
    - If the character code is odd c&1 is 1 and the byteis shifted 4 bits right  

*/

#define utl_ENC(c) (utl_ENCODING[c>>1] >> ((c&1) << 2))

static int utl_isdigit(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x08);}
static int utl_isalpha(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x06);}
static int utl_isalnum(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x0E);}
static int utl_islower(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x04);}
static int utl_isupper(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x02);}
static int utl_isblank(int ch) {return (ch == 0xA0) || ((ch <= 0xFF) && isblank(ch));}

static int utl_isspace(int ch)  {return (ch <= 0xFF) && isspace(ch);}
static int utl_iscntrl(int ch)  {return (ch <= 0xFF) && iscntrl(ch);}
static int utl_isgraph(int ch)  {return (ch <= 0xFF) && isgraph(ch);} 
static int utl_ispunct(int ch)  {return (ch <= 0xFF) && ispunct(ch);}
static int utl_isprint(int ch)  {return (ch <= 0xFF) && isprint(ch);}
static int utl_isxdigit(int ch) {return (ch <= 0xFF) && isxdigit(ch);}

static int utl_pmx_fold(int ch)
{
  if (utl_isupper(ch)) ch += 32; 
  return ch;
}

static int utl_pmx_class(const char **pat_ptr, const char **txt_ptr)
{
  int inv = 0;
  
  const char *pat = *pat_ptr;
  const char *txt = *txt_ptr;
  const char *pat_end;
  
  int32_t len   = 0;
  int32_t n     = 0;
  int32_t min_n = 0;
  int32_t max_n = 0;
  int32_t ch;
  
  _logdebug("class:[%s][%s]",pat,txt);
                
  pat++;  /* skip the '<' */
  
  // {{ Find the end of the pattern
  pat_end=pat;
  while (*pat_end && *pat_end != '>') pat_end++;
  if (pat_end[1] == '>') pat_end++; /* allow just one '>' at the end of a pattern */
  //}}
  
  // {{ Get how many times the match has to occur.
  //      Examples: <2-4l> matches 2,3 or 4 lower case letters
  //                <-4d>  matches from 0 to 4 decimal digits
  //                <2-s>  matches at 2 spaces or more
  //                <3u>   matches exactly 3 upper case letters
  while ('0' <= *pat && *pat <= '9') 
    min_n = (min_n*10) + (*pat++ - '0');
  
  if (*pat == '-') {
    pat++;
    while ('0'<=*pat && *pat <= '9') 
      max_n = (max_n*10) + (*pat++ - '0'); 
    
    if (max_n == 0) max_n = INT32_MAX;
  }
  if (max_n < min_n) max_n = min_n;
  
  if (max_n == 0) {
    switch (*pat) {
      case '*' : min_n = 0; max_n = INT32_MAX; pat++; break;
      case '+' : min_n = 1; max_n = INT32_MAX; pat++; break;
      case '?' : min_n = 0; max_n = 1;         pat++; break;
      default  : min_n = 1; max_n = 1;                break;
    }
  }
  // }}
  
  if (*pat == '!') {inv = 1; pat++;}
  
  // {{ This is for handling repetition of patterns between parenthesis:
  //    Example: '<*>(\\'|<!='>)'   <--  single quoted string (\' as escaped)
  //              <3>(xa|pt)        <--  "ptptpt" or "ptxaxa" or "xaxapt" or ...
  if (pat[0] == '>' && pat[1] == '(') {
    pat += 2;
    if (!utl_pmx_state_push(pat,txt,min_n,max_n,inv)) 
      utl_pmx_set_paterror(pat);
    *pat_ptr = pat;
    return 1;
  }
  // }}
  
  // {{ Matches a pattern n times
  #define utl_W(tst) while ((len = utl_pmx_nextch(txt,&ch)) && ((!tst) == inv) && (n<max_n)) {n++; txt+=len;}
  switch (*pat) {
    case 'a' : utl_W(( utl_isalpha(ch) )); break;
    case 's' : utl_W(( utl_isspace(ch) )); break;
    case 'u' : utl_W(( utl_isupper(ch) )); break;
    case 'l' : utl_W(( utl_islower(ch) )); break;
    case 'd' : utl_W(( utl_isdigit(ch) )); break;
    case 'k' : utl_W(( utl_isblank(ch) )); break;
    case 'x' : utl_W(( utl_isxdigit(ch))); break;
    case 'w' : utl_W(( utl_isalnum(ch) )); break;
    case 'c' : utl_W(( utl_iscntrl(ch) )); break;
    case 'g' : utl_W(( utl_isgraph(ch) )); break;
    case 'p' : utl_W(( utl_ispunct(ch) )); break;
    case 'r' : utl_W(( utl_isprint(ch) )); break;

    case 'i' : utl_W((ch < 0x80))               ; break;
    
    case '.' : utl_W((ch !='\0' && ch !='\n'))  ; break;

    case '=' : utl_W(utl_pmx_isin_chars(pat+1,pat_end,ch)) ; break;
    case '#' : utl_W(utl_pmx_isin_codes(pat+1,pat_end,ch)) ; break;
    
    case 'N' : utl_W((txt[0]=='\r'
                            ? (txt[1] == '\n'? (len++) : 1)
                            : (txt[0] == '\n'?  1 : 0)    )) ; break;

    case 'Q' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_QUOTED))); break;
    case 'B' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_BRACED))); break;
    
    case 'I' : utl_pmx_case = 0; n=min_n; break;
    case 'C' : utl_pmx_case = 1; n=min_n; break;
                            
    case '$' : if (*txt == '\0') n=min_n; break;
    
    case '>' : utl_pmx_set_paterror(pat); return 0;
  
    case '^' : if (inv) utl_pmx_set_paterror(pat); inv = 0;
               utl_W((len=utl_pmx_iscapt(pat+1,txt)));
               break;
  
    case ':' : if (utl_pmx_ext)
                 utl_W((len=utl_pmx_ext(pat+1,txt,len,ch)));
               break;
  
    default  : ; //utl_pmx_set_paterror(pat);
  }
  #undef utl_W
  // }}

  // {{ Advance pattern
  while (*pat_end == '>') pat_end++;
  *pat_ptr=pat_end;
  // }}
  
  /* Not enough matches */
  if (n < min_n) return 0;
  
  // {{ Advance matched text
  *txt_ptr = txt;
  // }}
  
  return 1;
}

static const char *utl_pmx_alt_skip(const char *pat)
{
  int paren=0;
  
  while (*pat) {
    switch (*pat++) {
      case '%': if (*pat) pat++; /* works for utf8 as well */
                break;
                
      case '(': paren++;
                utl_pmx_newcap(NULL);
                break;
                
      case ')': if (paren == 0) return (pat-1);
                paren--;
                break;
                
      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;
    }
  }
  return pat;
}

static const char *utl_pmx_alt(const char *pat, const char **txt_ptr)
{
  int paren=0;
  utl_pmx_state_s *state;
  int inv;
  const char *ret = utl_emptystring;
  
  while (*pat) {
    _logdebug("ALT: %s (%d)",pat,utl_pmx_stack_ptr);
    switch (*pat++) {
      case '%': if (*pat) pat++; /* works for utf8 as well */
                break;
                
      case '(': paren++;
                utl_pmx_newcap(NULL);
                break;
                
      case ')': if (paren > 0) {
                  paren--;
                  break;
                }
                if (utl_pmx_stack_ptr < 2) {
                  utl_pmx_set_paterror(pat);
                  break;
                }
                
                // {{ If we are here, we have NOT matched what is in the (...) 
                state = utl_pmx_state_top();
                inv = state->inv;
                ret = pat;
                if (inv) { /* It's ok, we WANTED to fail */
                  *txt_ptr = pmxstart(state->cap);  
                  utl_pmx_state_pop();
                  return ret;
                }
                if (state->n >= state->min_n) {       /* It's ok, we matched enough times */
                  utl_pmx_capt[state->cap][0] = state->txt;
                  utl_pmx_capt[state->cap][1] = *txt_ptr;
                  utl_pmx_state_pop();
                  return ret;
                }
                // }}
                break;

      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;

      case '|': if (paren == 0) {
                  state = utl_pmx_state_top();
                  *txt_ptr = pmxstart(state->cap); 
                  return pat;
                }
                break;
    }
  }
  return utl_emptystring;
}

static const char *utl_pmx_match(const char *pat, const char *txt)
{
  int32_t len;
  int32_t ch;
  int32_t c1;
  int16_t inv =0;
  utl_pmx_state_s *state;
  
  utl_pmx_state_reset();
  utl_pmx_state_push(pat,txt,1,1,0);
  
  while (*pat) {
    _logdebug("[MATCH] %d [%s] [%s]",pmxcount(),pat,txt);
    c1 = 0; 
    switch (*pat) {
      case '(' : pat++;
                 if (*pat == '|') {inv = 1; pat++;}
                 if (!utl_pmx_state_push(pat,txt,1,1,inv)) 
                   utl_pmx_set_paterror(pat);
                 break;
                 
      case '|' : pat = utl_pmx_alt_skip(pat);
                 break;
      
      case ')' : pat++;
                 _logdebug(")->%d",utl_pmx_stack_ptr);
                 if (utl_pmx_stack_ptr < 2) {
                   utl_pmx_set_paterror(pat-1); 
                   break;
                 }
                 
                 /* If we are here, we have matched what is in the (...) */
                 state = utl_pmx_state_top();
                 inv = state->inv;
                 if (inv) { /* we shouldn't have matched it :( */
                   utl_pmx_state_pop();
                   utl_pmx_FAIL;
                 }
                 
                 utl_pmx_capt[state->cap][1] = txt;  
                 state->n++;
                 _logdebug("match #%d min:%d max:%d",state->n,state->min_n, state->max_n);
                 
                 if (state->n < state->max_n) { 
                   utl_pmx_capt[state->cap][0] = txt;
                   pat = state->pat; /* try to match once more */
                 }
                 else {
                   utl_pmx_capt[state->cap][0] = state->txt;  
                   utl_pmx_state_pop();
                 }
                 
                 break;
                 
      case '<' : if (!utl_pmx_class(&pat,&txt)) utl_pmx_FAIL;
                 break;

      case '%' : if (pat[1]) len = utl_pmx_nextch(++pat, &c1);

      default  : if (c1 == 0) len = utl_pmx_nextch(pat, &c1);
                 len = utl_pmx_nextch(txt, &ch);
                 if (!utl_pmx_case) {
                   ch = utl_pmx_fold(ch);
                   c1 = utl_pmx_fold(c1);
                 }
                 if (ch != c1) {
                   _logdebug("FAIL: %d %d",c1,ch);
                   utl_pmx_FAIL;
                 }
                 txt += len;
                 pat += len;
                 break;
                 
      fail     : pat = utl_pmx_alt(pat, &txt) ; /* search for an alternative */
                 if (*pat == '\0') utl_pmx_capnum = 0;
                 break;
    }
  }
  utl_pmx_capt[0][1] = txt;
  
  for (len = utl_pmx_capnum; len < utl_pmx_MAXCAPT; len++) {
    utl_pmx_capt[len][0] = utl_pmx_capt[len][1] = NULL;
  }
  _logdebug("res: %p - %p",utl_pmx_capt[0][0],utl_pmx_capt[0][1]);
  return utl_pmx_capt[0][0];
}

const char *utl_pmx_search(const char *pat, const char *txt, int fromstart)
{
  const char *ret=NULL;
  
  utl_pmx_error = NULL;
  utl_pmx_case = 1;
  
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_utf8=0;}
    
  if (*pat == '^')  ret = utl_pmx_match(pat+1,txt);
  else while (!(ret = utl_pmx_match(pat,txt)) && *txt && !fromstart) {
         txt += utl_pmx_utf8 ? utl_pmx_get_utf8(txt, NULL) : 1;
       }
  _logdebug("ret: %p",ret);
  return ret;
}

#endif
#endif
