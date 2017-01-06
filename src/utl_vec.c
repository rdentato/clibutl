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

#include "utl.h"

//<<<//
#ifndef UTL_NOVEC
#ifdef UTL_MAIN

int utl_vec_nullcmp(void *a, void *b, void* aux){return 0;}

static int16_t utl_vec_makeroom(vec_t v,uint32_t n)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;

  if (n < v->max) return 1;
  new_max = v->max;
  while (new_max <= n) new_max += (new_max / 2);  /*  (new_max *= 1.5) instead of (new_max *= 2) */
  new_vec = (uint8_t *)realloc(v->vec, new_max * v->esz);
  if (!new_vec) return 0;
  v->vec = new_vec;
  v->max = new_max;
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

int16_t utl_vec_del(vec_t v, uint32_t i)
{
  return utl_vec_delgap(v,i,1);
}

int16_t utl_vec_delrange(vec_t v, uint32_t i,  uint32_t j)
{
  if (j<i) return 0;
  return utl_vec_delgap(v,i,j-i+1);  
}

vec_t utl_vec_new(uint16_t esz, int (*cmp)(void *, void *,void *), uint32_t (*hsh)(void *, void *))
{
  vec_t v = NULL;
  uint32_t sz;
    
  if (hsh) {
    if (!cmp) return NULL;
    esz += (4+(esz & ~0x03)) & 0x03; // ensure is aligned to 4 bytes
    esz += sizeof(uint32_t); // To store the key hash value
  }
  
  sz = sizeof(vec_s)+(esz-4); // 4 is the size of the elm array
  v = (vec_t)malloc(sz);
  if (v) {
    memset(v,0,sz);
    v->esz = esz;
    v->max = vec_MIN_ELEM;
    v->vec = (uint8_t *)malloc(v->max * esz);
    if (!v->vec) { 
      free(v);  v = NULL;
    }
    else {
      v->elm = ((char *)v) + offsetof(vec_s,eld);
      v->cmp = cmp;
      v->hsh = hsh;
      memset(v->vec,0xFF,v->max *esz); // if hashed, hash would be 0xFFFFFFFF
      vecunsorted(v);
      v->fst=vec_MAX_CNT;
      v->lst=vec_MAX_CNT;
      v->cur=0;
      v->aux=NULL;
    }
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

/* * Sorted sets  * */

/*                             
#define utl_dpqswap(a,b) do { if (a!=b) { \
                                uint8_t t;\
                                pa = ((uint8_t *)a); pb = ((uint8_t *)b); \
                                for (uint32_t k=0; k<esz; k++,pa++,pb++) { \
                                  t = *pa; *pa = *pb; *pb = t;\
                                }\
                              }\
                           } while (0)
*/
/*
#define utl_dpqswap(a,b) do { if (a!=b) { \
                                  uint32_t sz = esz;\
                                  uint8_t  tmp8; \
                                  uint32_t tmp32; \
                                  uint8_t *pa = ((uint8_t *)a); \
                                  uint8_t *pb = ((uint8_t *)b); \
                                  while (sz >= 4) { \
                                    tmp32 = *(uint32_t *)pa; \
                                    *(uint32_t *)pa = *(uint32_t *)pb;\
                                    *(uint32_t *)pb = tmp32;\
                                    sz-=4; pa+=4; pb+=4;\
                                  }\
                                  switch (sz) {\
                                    case 3: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;\
                                    case 2: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;\
                                    case 1: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;\
                                  }\
                                }\
                           } while (0)
*/                           
static inline void utl_dpqswap(void *a, void *b, uint32_t sz)
{
  uint8_t  tmp8;
  uint32_t tmp32;
  uint8_t *pa = ((uint8_t *)a);
  uint8_t *pb = ((uint8_t *)b);
  
  if (a!=b) {
    while (sz >= 4) {
      tmp32 = *(uint32_t *)pa;
      *(uint32_t *)pa = *(uint32_t *)pb;
      *(uint32_t *)pb = tmp32;
      sz-=4; pa+=4; pb+=4;
    }
    switch (sz) {
      case 3: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;
      case 2: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;
      case 1: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;
    }
  }
}

static inline int utl_dpqordrange(int32_t a, int32_t b, int32_t c)
{
  if (a>b) {
    if (b>c) return 123;  // a>b>c
    if (c>a) return 312;  // c>a>b
             return 132;  // a>c>b
  } 
  if (a>c) return 213;    // b>a>c
  if (c>b) return 321;    // c>b>a 
           return 231;    // b>c>a
}

static uint32_t utl_rnd_status = 0;
static inline uint32_t utl_dpqrand() 
{ // xorshift
  if (utl_rnd_status == 0) utl_rnd_status = (uint32_t)time(0);
	//utl_rnd_status = 1664525 * utl_rnd_status + 1013904223; // LCRNG 
  utl_rnd_status ^= utl_rnd_status << 13;
	utl_rnd_status ^= utl_rnd_status >> 17;
	utl_rnd_status ^= utl_rnd_status << 5;
	return utl_rnd_status;
}

#define utl_dpqptr(k)    ((uint8_t *)base+(k)*esz)
#define utl_dpqpush(l,r) do {stack[stack_top][0]=(l); stack[stack_top][1]=(r); stack_top++; } while(0)
#define utl_dpqpop(l,r)  do {stack_top--; l=stack[stack_top][0]; r=stack[stack_top][1];} while(0)

void utl_dpqsort(void *base, uint32_t nel, uint32_t esz, int (*cmp)(const void *, const void *, const void *), void *aux)
{
  int32_t left,right;
  uint8_t *leftptr, *rightptr; 
  uint32_t L,K,G;
  
  int32_t stack[128][2]; // Enough for 2^31 max elements in the array
  int16_t stack_top = 0;
  //uint32_t stack_max = 0;
  
  utl_dpqpush(0,nel-1);
  while (stack_top > 0) {
    //if (stack_top > stack_max) stack_max=stack_top;
    utl_dpqpop(left, right);
    if (left < right) {
      if ((right - left) <= 16) {  // Use insertion sort
        for (int32_t i = left+1; i<=right; i++) {
          rightptr = utl_dpqptr(i);
          leftptr = rightptr - esz;
          for (int32_t j=i; j>0 && cmp(leftptr, rightptr, aux) > 0; j--) {
            utl_dpqswap(rightptr, leftptr, esz);
            rightptr = leftptr;
            leftptr = rightptr - esz;
          }
        }
      }
      else {
        leftptr = utl_dpqptr(left);
        rightptr = utl_dpqptr(right);
        
        /* Randomize pivot to avoid worst case (already sorted array) */
        L = left + (utl_dpqrand() % (right-left));
        G = left + (utl_dpqrand() % (right-left));
        utl_dpqswap(utl_dpqptr(L),leftptr, esz);
        utl_dpqswap(utl_dpqptr(G),rightptr, esz);
        
        if (cmp(leftptr, rightptr, aux) > 0) {
          utl_dpqswap(leftptr, rightptr, esz);
        }
        L=left+1; K=L; G=right-1;
        while (K <= G) {
          if (cmp(utl_dpqptr(K), leftptr, aux) < 0) {
            utl_dpqswap(utl_dpqptr(K), utl_dpqptr(L), esz);
            L++;
          }
          else if (cmp(utl_dpqptr(K), rightptr, aux) > 0) {
            while ((cmp(utl_dpqptr(G), rightptr, aux) > 0) && (K<G)) 
              G--;

            utl_dpqswap(utl_dpqptr(K), utl_dpqptr(G), esz);
            G--;
            if (cmp(utl_dpqptr(K), leftptr, aux) < 0) {
              utl_dpqswap(utl_dpqptr(K), utl_dpqptr(L), esz);
              L++;
            }
          }
          K++;
        }
        L--; G++;

        utl_dpqswap(leftptr,  utl_dpqptr(L), esz);
        utl_dpqswap(rightptr, utl_dpqptr(G), esz);

        switch (utl_dpqordrange((right-(G+1)), ((G-1)-(L+1)), ((L-1)-left))) {
          case 123: utl_dpqpush(G+1, right); utl_dpqpush(L+1, G-1);   utl_dpqpush(left, L-1);  break;
          case 132: utl_dpqpush(G+1, right); utl_dpqpush(left, L-1);  utl_dpqpush(L+1, G-1);   break;
          case 213: utl_dpqpush(L+1, G-1);   utl_dpqpush(G+1, right); utl_dpqpush(left, L-1);  break;
          case 231: utl_dpqpush(L+1, G-1);   utl_dpqpush(left, L-1);  utl_dpqpush(G+1, right); break;
          case 312: utl_dpqpush(left, L-1);  utl_dpqpush(G+1, right); utl_dpqpush(L+1, G-1);   break;
          case 321: utl_dpqpush(left, L-1);  utl_dpqpush(L+1, G-1);   utl_dpqpush(G+1, right); break;
        }
      }
    }
  }
  //logtrace("DPQ: maxstack: %d",stack_max);
}

void utl_vec_sort(vec_t v, int (*cmp)(void *, void *, void *))
{
  if (v->hsh) {vecunsorted(v); return;}
  
  if (cmp != utl_vec_nullcmp && cmp != v->cmp) {
    v->cmp = cmp;
    vecunsorted(v);
  }
  if (v->cmp == NULL) {
    vecunsorted(v);
  }
  else if (!vecissorted(v)) {
    if (v->cnt > 1) {
      //qsort(v->vec,v->cnt,v->esz,(int (*)(const void *, const void *))(v->cmp));  
      utl_dpqsort(v->vec,v->cnt,v->esz,(int (*)(const void *, const void *, const void *))(v->cmp),v);  
    }
    vecsorted(v);
  }
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
    ret = v->cmp(v->elm,elm,v);
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

static void *utl_vec_search_sorted(vec_t v)
{
  uint8_t *elm=NULL;
  int32_t mid,lo,hi;
  int ret;
  
  vecsort(v); 
  lo = 0; hi = v->cnt-1;
  while (lo <= hi) {
    mid = lo + (hi-lo)/2;
    elm = v->vec + mid * v->esz;
    ret = v->cmp(v->elm,elm,v);
    if (ret == 0) return elm;
    if (ret < 0) hi = mid-1;
    else         lo = mid+1;
  }
  return NULL;
}

/* * Unsorted sets (Hash tables) * */

#define utl_h(p,sz) *((uint32_t *)(((uint8_t *)(p))+(sz)-sizeof(uint32_t)))

static void *utl_remove_hashed(vec_t v,uint32_t pos)
{
  uint32_t nxt;
  uint32_t elm_h;
  uint8_t  *elm;
  uint8_t  *val;
  
  while(1) {
    val = v->vec + pos*v->esz;
    memset(val,0xFF,v->esz);
    
    nxt = (pos+1) & (v->max-1);
    elm = v->vec + nxt*v->esz;
    elm_h = utl_h(elm,v->esz);
    if (elm_h == 0xFFFFFFFF) break; // empty spot
    if ((elm_h & (v->max-1)) == nxt) break; // with delta 0
    memcpy(val, elm, v->esz);
    pos = nxt;
  }
  v->cnt--;
  return elm;
}

static void *utl_search_hashed(vec_t v)
{
  uint8_t *elm = NULL;
  uint32_t pos;
  uint32_t val_h;
  uint32_t elm_h;
  uint32_t max = v->max;
  int32_t delta;
  
  val_h = v->hsh(v->elm,v) & 0x7FFFFFFF;
  pos = val_h;
  while(1) {
    delta = 0;
    pos &= (max-1);
    elm = v->vec + pos * v->esz;
    elm_h = utl_h(elm,v->esz);
    if (elm_h == 0xFFFFFFFF) return NULL;
    if ((elm_h == val_h) && (v->cmp(v->elm,elm,v) == 0)) return elm;
    if ((elm_h - (utl_h(elm,v->esz) & (max-1))) < delta) return NULL;
    pos++; delta++;
  }
  v->cnt--;
  return elm; 
}

static uint8_t *utl_vec_hsh_set(uint8_t *val, uint8_t *tab, uint32_t max, uint16_t esz,  int (*cmp)(void *, void *,void *), void *aux)
{
  uint32_t pos;
  uint32_t prv;
  uint8_t *elm = NULL;
  int32_t delta;
  uint32_t val_h;
  uint32_t elm_h;
  
  val_h = utl_h(val,esz);
  if (val_h < 0x80000000) {
    delta = 0;
    pos = val_h;
    while (1) {    
      pos &= (max-1);
      //logtrace("pos: %d hash: %08X max: %d",pos,h,max);
      elm = tab + pos * esz;
      elm_h = utl_h(elm,esz);
      if ((elm_h == 0xFFFFFFFF) || ((elm_h == val_h) && (cmp(val,elm,aux) == 0)))  {
        memcpy(elm,val,esz);
        break;
      }
      pos++; delta++;
    } 
    // Robin Hood: swap with previous if delta is lower
    while(delta > 0) {
      prv = (pos+max-1) & (max-1);
      elm = tab + prv * esz;
      // logtrace("RH: (delta: %d pos: %d) ( deltap: %d prev: %d)",delta,pos,(prv - (utl_h(elm,esz) & (max-1))),prv);
      delta--;
      if ((prv - (utl_h(elm,esz) & (max-1))) > delta) break;
      memcpy(tab+pos*esz, tab+prv*esz, esz);
      memcpy(elm, val, esz);
      pos = prv;
    }
  }
  return elm;
}

static uint8_t *utl_reash(uint8_t *tab,uint32_t max, uint16_t esz, int (*cmp)(void *, void *, void *),void *aux)
{
  uint8_t *new_tab;
  uint32_t new_max;
  uint32_t k;
  
  new_max = max * 2;
  new_tab = malloc(new_max * esz);
  if (new_tab) {
    memset(new_tab,0xFF,new_max*esz);
    for (k=0; k<max;k++) {
      //logtrace("reash %d",k);
      utl_vec_hsh_set(tab, new_tab, new_max,esz,cmp,aux);
      tab += esz;
    }
  }
  //logtrace("reash completed");
  return new_tab;
}

static void *utl_vec_add_hashed(vec_t v)
{
  uint8_t *elm;
  uint8_t *tab;
  uint32_t h;
  
  if (((v->cnt * 100) / 80) >= v->max) {  // Load factor max: 80%
    //logtrace("Reash at %d",v->cnt);
    tab = utl_reash(v->vec,v->max,v->esz,v->cmp,v);
    if (tab == NULL) return NULL;
    free(v->vec);
    v->vec = tab;
    v->max *= 2;
  }
  
  h = v->hsh(v->elm,v) & 0x7FFFFFFF;
  elm = v->elm;
  utl_h(elm, v->esz) = h;
  elm = utl_vec_hsh_set(elm,v->vec,v->max,v->esz,v->cmp,v);
  
  if (elm) v->cnt += 1;
 
  return elm;
}

/* * sets * */
void *utl_vec_add(vec_t v, uint32_t i) {
  if (v->hsh) return utl_vec_add_hashed(v);
  if (v->cnt > 0) {
    if (v->cmp) return utl_vec_add_sorted(v);
  }
  return utl_vec_set(v,v->cnt);
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


void *utl_vec_search(vec_t v, int x)
{
  uint8_t *elm = NULL;
  
  if (v->cnt == 0) return NULL;
  
  if (v->hsh) {
    elm = utl_search_hashed(v);
  }
  else if (v->cmp) {
    elm = utl_vec_search_sorted(v);
  }
  return elm;
}

int utl_vec_remove(vec_t v, int x)
{
  uint8_t *elm = NULL;
  uint32_t pos;
  
  if (v->cnt == 0) return 0;
  
  elm = utl_vec_search(v,0);
  if (elm) {
    pos = (elm - v->vec)/v->esz;
    if (v->hsh) {
      elm = utl_remove_hashed(v,pos);
    } 
    else {
      if (!utl_vec_del(v,pos)) elm = NULL;
    }
  }
  return (elm != NULL);
}

void *utl_vec_first(vec_t v)
{
  uint8_t  *elm = NULL;
  
  if (v->cnt == 0) {
    v->cur = vec_MAX_CNT;
    return NULL;
  }
  elm = v->vec;
  if (v->fst != vec_MAX_CNT) {
    elm = v->vec + v->fst * v->esz ;
  }
  else if (v->hsh) {
     while (utl_h(elm,v->esz) == 0xFFFFFFFF) {
       elm += v->esz;
     }
  }
  v->cur = (elm - v->vec) / v->esz;
  return elm;
}

void *utl_vec_last(vec_t v)
{
  uint8_t  *elm = NULL;
  
  if (v->cnt == 0) {
    v->cur = vec_MAX_CNT;
    return NULL;
  }
  elm = v->vec + (v->cnt-1) * v->esz;
  if (v->lst != vec_MAX_CNT) {
    elm = v->vec + v->lst * v->esz ;
  }
  else if (v->hsh) {
    elm = v->vec + (v->max-1) * v->esz;
    while (utl_h(elm,v->esz) == 0xFFFFFFFF) {
      elm -= v->esz;
    }
  }
  v->cur = (elm - v->vec) / v->esz;
  return elm;
}

void *utl_vec_next(vec_t v)
{
  uint32_t max = v->cnt;
  uint8_t *elm;
  
  if (v->cnt == 0) v->cur = vec_MAX_CNT;
  if (v->cur == v->lst) v->cur = vec_MAX_CNT;
  if (v->cur == vec_MAX_CNT) return NULL;
  v->cur++;
  elm = v->vec + v->cur * v->esz;
  if (v->hsh) {
    max = v->max;
    while (v->cur < max && (utl_h(elm,v->esz) == 0xFFFFFFFF)) {
      v->cur++;
      elm = v->vec + v->cur * v->esz;
    }
  }
  
  if (v->cur >= max) v->cur = vec_MAX_CNT;
  if (v->cur == vec_MAX_CNT) return NULL;
  return elm;
}

void *utl_vec_prev(vec_t v)
{
  uint8_t *elm;
  
  if (v->cnt == 0) v->cur = vec_MAX_CNT;
  if (v->cur == v->fst) v->cur = vec_MAX_CNT;
  if (v->cur == 0) v->cur = vec_MAX_CNT;
  if (v->cur == vec_MAX_CNT) return NULL;
  v->cur--;
  elm = v->vec + v->cur * v->esz;
  if (v->hsh) {
    while (v->cur > 0 && (utl_h(elm,v->esz) == 0xFFFFFFFF)) {
      v->cur--;
      elm = v->vec + v->cur * v->esz;
    }
    if (utl_h(elm,v->esz) == 0xFFFFFFFF) v->cur = vec_MAX_CNT;
  }
  
  if (v->cur == vec_MAX_CNT) return NULL;
  return elm;
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
//>>>//
