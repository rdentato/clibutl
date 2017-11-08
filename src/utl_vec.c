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

int utl_vec_nullcmp(void *a, void *b, void* aux){return 0;}

static int16_t utl_vec_makeroom(vec_t v,uint32_t n)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;

  if (n < v->max) return 1;
  new_max = v->max;
  while (new_max <= n) {
    new_max += (new_max / 2);  /* (new_max *= 1.5) instead of (new_max *= 2) */
    new_max += (new_max & 1);  /* ensure new size is even */
  }
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

vec_t utl_vec_new(uint16_t esz, utl_cmp_t cmp, utl_hsh_t hsh)
{
  vec_t v = NULL;
  uint32_t sz;
    
  if (hsh) {
    if (!cmp) return NULL;
    esz += (4+(esz & ~0x03)) & 0x03; // ensure is aligned to 4 bytes
    esz += sizeof(uint32_t); // To store the key hash value
  }
  
  sz = sizeof(vec_s)+(esz-4); // 4 is the size of the elm array
  v = malloc(sz);
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
      v->fst=0;
      v->lst=vec_MAX_CNT;
      v->cur=0;
      v->cnt=0;
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

#define VEC_MAGIC(x) (((uint32_t)'V')<<24 | ((uint32_t)'E')<<16 | ((uint32_t)'C') << 8| (uint32_t)x)

int utl_vec_freeze(FILE *f,vec_t v)
{
  uint32_t sz;
  uint32_t max;
  int ret = 0;
  char *magic = "VEC1";
  
  if (f && v) {
    if (fwrite(magic,4,1,f)) {
      if (fwrite(&(v->esz),sizeof(uint16_t),1,f)) {
        sz = sizeof(vec_s)+(v->esz-4);
        max = v->max;
        // You can't shrink hashtable nor que (TODO!)
        //if (!v->hsh) v->max = v->cnt;
        if (fwrite(v,sz,1,f)) {
          if (fwrite(v->vec,v->esz*v->max,1,f)) {
            v->max = max;
            ret = 1;
          }
        }
        v->max = max;
      }
    }
  }
  
  return ret;
}

vec_t utl_vec_unfreeze(FILE *f, utl_cmp_t cmp, utl_hsh_t hsh)
{
  vec_t v = NULL;
  char magic[8];
  uint16_t esz;
  uint32_t sz;
  
  if (f) {
    if (fread(&magic,4,1,f)) {
      if (strncmp(magic,"VEC1",4) == 0) {
        if (fread(&esz,sizeof(uint16_t),1,f)) {
          sz = sizeof(vec_s)+(esz-4);
          if ((v = malloc(sz))) {
            v->vec = NULL;
            if (fread(v,sz,1,f)) {
              v->vec = malloc(v->esz*v->max);
              if (v->vec) {
                if (fread(v->vec,v->esz*v->max,1,f)) {
                  _logtrace("VEC UNFRZ: %p %p",cmp,hsh);
                  v->elm = ((char *)v) + offsetof(vec_s,eld);
                  v->cmp = cmp;
                  v->hsh = hsh;
                  return v;
                } } } } } } } }
  if (v) {
    if (v->vec) free(v->vec);
    free(v);
  }
  return NULL;
}

int utl_frz(char *fname, vec_t t, int (*fun)(FILE *, vec_t))
{
  int ret = 0;
  FILE *f;
  f = fopen(fname,"wb");
  if (f) {
    ret = fun(f,t);
    fclose(f);
  }
  return ret;
}

vec_t utl_unfrz(char *fname, utl_cmp_t cmp, utl_hsh_t hsh, vec_t (*fun)(FILE *, utl_cmp_t, utl_hsh_t))
{
  sym_t t = NULL;
  FILE *f;
  f = fopen(fname,"rb");
  if (f) {
    t = fun(f, cmp, hsh);
    fclose(f);
  }
  return t;
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

void *utl_vec_alloc(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  
  _logdebug("vecalloc: pos: %d max:%d cnt:%d",i,v->max,v->cnt);
  if (i == vec_MAX_CNT) i = v->cnt;
  if (utl_vec_makeroom(v,i)) {
    elm = v->vec + (i*v->esz);
    if (i >= v->cnt) v->cnt = i+1;
    vecunsorted(v);
  }
  return elm;
}

void *utl_vec_set(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  _logdebug("vecset: %p pos: %d",(void*)v,i);
  elm = utl_vec_alloc(v,i);
  _logdebug("allocated:");
  if (elm) memcpy(elm, v->elm, v->esz);
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

/* * QUEUE */
static int16_t utl_que_makeroom(vec_t v)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;
  int32_t n;

  if (v->lst==vec_MAX_CNT) v->lst=0;
  
  if (v->cnt < v->max) return 1;
  new_max  = v->max;
  new_max += (new_max / 2);  /* (new_max *= 1.5) instead of (new_max *= 2) */
  new_max += (new_max & 1);  /* ensure new size is even */
 
  new_vec = (uint8_t *)realloc(v->vec, new_max * v->esz);
  if (!new_vec) return 0;
 
  if (v->fst == 0) {
    v->lst = v->max;
  }
  else if (v->fst < v->max /2) {
    memcpy(new_vec+(v->max*v->esz), new_vec,v->fst * v->esz);
    v->lst = v->max+v->fst;
  } else {
    n = v->max - v->fst;
    memcpy(new_vec + (new_max-n)*v->esz, new_vec + (v->fst * v->esz), n*v->esz);
    v->fst = new_max - n;
  }
  
  v->vec = new_vec;
  v->max = new_max;
  return 1;
}

void *utl_vec_enq(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (utl_que_makeroom(v)) {
    _logdebug("enque cnt:%d lst:%d",v->cnt,v->lst);
    elm = v->vec + (v->lst*v->esz);
    memcpy(elm, v->elm, v->esz);
    v->cnt++;
    v->lst++;
    if (v->lst >= v->max) v->lst = 0;
    vecunsorted(v);
  }
  return elm;
}

void utl_vec_deq(vec_t v)
{
  if (v->cnt > 0) {
    v->fst++;
    if (v->fst >= v->max) v->fst = 0;
    v->cnt--;
  }
  if (v->cnt == 0) {
    v->fst = 0;
    v->lst = 0;
  }
}

/* * Sorted sets  * */
                          
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
static inline uint32_t utl_dpqrand(void) 
{ 
  if (utl_rnd_status == 0) utl_rnd_status = (uint32_t)time(0);
	//utl_rnd_status = 1664525 * utl_rnd_status + 1013904223; // Linar Congruence
  utl_rnd_status ^= utl_rnd_status << 13;                   // xorshift
	utl_rnd_status ^= utl_rnd_status >> 17;
	utl_rnd_status ^= utl_rnd_status << 5;
	return utl_rnd_status;
}

#define utl_dpqptr(k)    ((uint8_t *)base+(k)*esz)
#define utl_dpqpush(l,r) do {stack[stack_top][0]=(l); stack[stack_top][1]=(r); stack_top++; } while(0)
#define utl_dpqpop(l,r)  do {stack_top--; l=stack[stack_top][0]; r=stack[stack_top][1];} while(0)

void utl_dpqsort(void *base, uint32_t nel, uint32_t esz, utl_cmp_t cmp, void *aux)
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
        //logtrace("DPQ: Insertion sort [%d - %d]",left,right);
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
        //logtrace("DPQ: Randomized left:%d right:%d ",L,G);
        if (cmp(leftptr, rightptr, aux) > 0) {
          utl_dpqswap(leftptr, rightptr, esz);
        }
        L=left+1; K=L; G=right-1;
        //logtrace("DPQ: [%d - %d] L:%d K:%d G:%d [START]",left,right,L,K,G);
        while (K <= G) {
          //logtrace("DPQ: [%d - %d] L:%d K:%d G:%d",left,right,L,K,G);
          if (cmp(utl_dpqptr(K), leftptr, aux) < 0) {
            //logtrace("DPQ: [K] < [left]");
            utl_dpqswap(utl_dpqptr(K), utl_dpqptr(L), esz);
            L++;
          }
          else if (cmp(utl_dpqptr(K), rightptr, aux) > 0) {
            //logtrace("DPQ: [K] >= [right]");
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
        /* Push ranges so that the largest will be handled first */
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
      utl_dpqsort(v->vec,v->cnt,v->esz,v->cmp,v);  
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

/* 
                                    +------+ 
    The utl_h() macro retrieves     |      | actual element 
    the hash value of the element   |      | 
                                    +------+ 
                                    | hash | hash value
                                    +------+
 */
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
  uint32_t delta;
  
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
}

static uint8_t *utl_vec_hsh_set(uint8_t *val, uint8_t *tab, uint32_t max, uint16_t esz,  int (*cmp)(void *, void *,void *), void *aux)
{
  uint32_t pos;
  uint32_t prv;
  uint8_t *elm = NULL;
  uint32_t delta;
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
  //logtrace("Hash add #%d @%d",v->cnt,(int)(elm?(elm-v->vec)/v->esz:-1));
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
    return n;
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
  v->cur = 0;
  if (v->hsh) {
    // Since there is at lease 1 element, we will
    // eventually exit the loop.
    while (utl_h(elm,v->esz) == 0xFFFFFFFF) {
      _logtrace("   checking %d",v->cur);
      elm += v->esz;
      v->cur++;
    }
  }
  else if (v->fst != vec_MAX_CNT) {
    elm = v->vec + v->fst * v->esz ;
  }
  
  v->cur = (elm - v->vec) / v->esz;
  _logtrace("found (first) %d, %d",v->cur, *((uint32_t *)elm));
  memcpy(v->elm,elm,v->esz);
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
  if (v->hsh) {
    elm = v->vec + (v->max-1) * v->esz;
    while (utl_h(elm,v->esz) == 0xFFFFFFFF) {
      elm -= v->esz;
    }
  }
  else if (v->lst != vec_MAX_CNT) {
    elm = v->vec + ((v->lst-1) * v->esz);
  }
  v->cur = (elm - v->vec) / v->esz;
  memcpy(v->elm,elm,v->esz);
  return elm;
}

void *utl_vec_next(vec_t v)
{
  uint32_t max = v->cnt;
  uint8_t *elm = NULL;
  
  _logtrace("Initial cnt:%d cur:%d",v->cnt,v->cur);
  if (v->cnt == 0) v->cur = vec_MAX_CNT;
  if (v->cur == v->lst) v->cur = vec_MAX_CNT;
  if (v->cur == vec_MAX_CNT) return NULL;
  
  v->cur++;
  max = v->max;
  if (v->hsh) {  // Hash table
    while (v->cur < max) {
      _logtrace("   checking %d",v->cur);
      elm = v->vec + v->cur * v->esz;
      if (utl_h(elm,v->esz) != 0xFFFFFFFF) break;
      v->cur++;
    }
  } else if (v->lst != vec_MAX_CNT) { // que
    if (v->cur >= max) v->cur = 0;
    if (v->cur == v->lst) v->cur = max;
  } else { // array
    max = v->cnt;
  }
  
  if (v->cur >= max) return NULL;
  
  if (v->cur != vec_MAX_CNT) {
    elm = v->vec + v->cur * v->esz;
    _logtrace("found (next) %d, %d",v->cur, *((uint32_t *)elm));
    memcpy(v->elm,elm,v->esz);
  }
  return elm;
}

// TODO: Check and fix it
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
  char *s = vecgetptr(b,n);
  return s?*s:'\0';
}

size_t utl_buf_read(buf_t b, uint32_t i, uint32_t n, FILE *f)
{
  size_t r ;
  
  if (i == vec_MAX_CNT) i = b->cnt;
  r = vecread(b,i,n,f);
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

char *utl_buf_readln(buf_t b, uint32_t n, FILE *f)
{
  int c;
  uint32_t i = n;
  
  if (i == vec_MAX_CNT) i = b->cnt;
 
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
  char *r;
  
  if (i == vec_MAX_CNT) i = b->cnt;
    
  r = buf(b)+i;
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

char *utl_buf_addc(buf_t b, char c)
{
  bufsetc(b,b->cnt,c); 
  bufsetc(b,b->cnt,'\0');
  b->cnt--;
  return buf(b) + b->cnt - 1;
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

int utl_buf_fmt(buf_t b, uint32_t i, const char *fmt, ...)
{
  va_list args;
  int n;
  if (i == vec_MAX_CNT) i = b->cnt;
  va_start(args, fmt);
  n = vsnprintf(NULL,0,fmt,args);
  va_end(args);
  if (n > 0) {
    bufsetc(b,i+n+1,'\0'); // ensure there's enough room
    va_start(args, fmt);
    n = vsnprintf(buf(b)+i,n+1,fmt,args);
    va_end(args);
    b->cnt = i+n;
  }
  return n;
}

/* *** SYM *** */
/*
       spare string used  ____         
       for search and set     \
                               \
 [sym_t] -aux-> [buf_t] -aux-> str
    \              \
     \              \___ keeps the strings:
      \                  s1\0s2\0s3\0...
       \
        \__ keeps the ID in an hastable
           (based on strings content)

Each string is preceded by the associated info (a 32 bit integer)
   0 1 2 3 4 5 6 7 8
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  |x|x|x|x|a|b|/|/|x|x|x|x|c|d|e|/|f| ... 
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
   |       |      
   |       id
   |
   data (int32_t)
   (always aligned to 4-bytes)   

*/

static uint32_t utl_sym_hash(void *a, void *c)
{
  sym_t v = c;
  char *a_str = utl_sym_get(v,*((uint32_t *)a));
  return utl_hash_string(a_str);
}

static int utl_sym_cmp(void *a, void *b, void *c)
{
  sym_t v = c;
  char *a_str = utl_sym_get(v,*((uint32_t *)a));
  char *b_str = utl_sym_get(v,*((uint32_t *)b));
  return strcmp(a_str,b_str);
}

sym_t utl_sym_new(void)
{
  sym_t t = NULL;
  buf_t b = NULL;
  
  if ((b = bufnew())) {
    if ((t = vecnew(uint32_t,utl_sym_cmp,utl_sym_hash))) {
      t->aux = b;
      b->aux = NULL;
      return t;
    }
  }
  b=buffree(b);
  return NULL;
}

#define utl_sym_tbl(t)          ((sym_t)(t))
#define utl_sym_buf(t) ((buf_t)(((sym_t)(t))->aux))

sym_t utl_sym_free(sym_t t)
{
  if (t) {
    buffree(utl_sym_buf(t));
    vecfree(utl_sym_tbl(t));
  }
  return NULL; 
}

int utl_sym_freeze(FILE *f, sym_t t)
{
  int ret = 0;
  ret = utl_vec_freeze(f,t);
  _logifdebug {
    logtrace("FREEZE VEC: %p",(void *)(t->vec));
    for (int k=0; k<32; k+=2) {
      logtrace("%2d [%8X] -> %8X",k,((uint32_t *)(t->vec))[k],((uint32_t *)(t->vec))[k+1]);
    }
  }
  if (ret) {
     ret = utl_vec_freeze(f,t->aux);
  }
  return ret;
}

sym_t utl_sym_unfreeze(FILE *f, utl_cmp_t cmp, utl_hsh_t hsh)
{
  sym_t t;
  _logtrace("SYM UNFRZ: %p %p",utl_sym_cmp,utl_sym_hash);
  t = utl_vec_unfreeze(f,utl_sym_cmp,utl_sym_hash);
  _logifdebug {
    logtrace("UNFREEZE VEC: %p",(void *)(t->vec));
    for (int k=0; k<32; k+=2) {
      logtrace("%2d [%8X] -> %8X",k,((uint32_t *)(t->vec))[k],((uint32_t *)(t->vec))[k+1]);
    }
  }
  if (t) {
    t->aux = utl_vec_unfreeze(f,NULL,NULL);
    if (t->aux) {
      utl_sym_buf(t)->aux = NULL;
      return t;
    }
  }
  t=vecfree(t);
  return NULL;
}

static uint32_t utl_sym_store(sym_t t,const char *sym)
{
  uint32_t id;
  uint32_t k;
  buf_t b = utl_sym_buf(t);

  k =  b->cnt;
  id = k + 4;
  bufsets(b,id,sym);
  *((int32_t *)(buf(b)+k)) = 0; // set data to 0
  do { // Ensure the next pointer will be 4-bytes aligned
    buf(b)[b->cnt++] = '\0';
  } while (b->cnt & 0x3);
  return id;
}

uint32_t utl_sym_add(sym_t t, const char *sym, int symis)
{
  uint32_t k = symNULL;
  
  if (!(symis & symNEW))
    k = utl_sym_search(t, sym);
  if (k != symNULL) {
    if (symis & symUNIQ) k = symNULL;
  }
  else {
    k = utl_sym_store(t,sym);
    vecadd(uint32_t,t,k);
  }
  return k;  
}

uint32_t utl_sym_search(sym_t t, const char *sym)
{
  uint32_t k = symNULL;
  uint32_t *p;
  _logtrace("SYM SEARCH: %s %p",sym,t->cmp);
  utl_sym_buf(t)->aux = (void *)sym;
  p=vecsearch(uint32_t,t,symNULL);
  utl_sym_buf(t)->aux = NULL;
  if (p) k = *p;
  return k;  
}

char *utl_sym_get(sym_t t,uint32_t id)
{
  char *s;
  if (id == vec_MAX_CNT) {
    _logtrace("id==MAX %s",(char *)utl_sym_buf(t)->aux);
    return utl_sym_buf(t)->aux;
  }
  if (id >= utl_sym_buf(t)->cnt) return NULL;
  s = buf(utl_sym_buf(t)) + id;
  if (*s == '\0') s = NULL;
  return s;
}

int16_t utl_sym_del(sym_t t, const char *sym)
{
  uint32_t  pos;
  uint32_t  id;
  char     *s;
  uint32_t *p;

  utl_sym_buf(t)->aux = (void *)sym;
  p=vecsearch(uint32_t,t,symNULL);
  utl_sym_buf(t)->aux = NULL;
  if (p) {
    id = *p;
    pos = ((uint8_t *)p - t->vec)/t->esz;
    //logtrace("Removing: %s (%d) @ %d",sym,id,pos);
    if (utl_remove_hashed(t,pos)) {
      s = buf(utl_sym_buf(t))+id;
      ((int32_t *)s)[-1] = 0;
      while (*s) *s++ = '\0';
      return 1;
    }
  }
  return 0;
}

static int32_t *utl_sym_data(sym_t t,uint32_t id)
{
  char *s;
  if (id >= utl_sym_buf(t)->cnt) return NULL;
  s = buf(utl_sym_buf(t)) + id;
  if (*s == '\0') return NULL;
  return ((int32_t *)(s-4));
}

int16_t utl_sym_setdata(sym_t t,uint32_t id, int32_t val)
{
  int32_t *s;
  
  s = utl_sym_data(t,id);
  if (s == NULL) return 0;
  
  *s = val;
  return 1;
}

int32_t utl_sym_getdata(sym_t t,uint32_t id)
{
  int32_t *s;
  s = utl_sym_data(t,id);
  if (s == NULL) return 0;
  return *s;
}


#endif
//>>>//
