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
#define UTL_MEMCHECK
#include "utl.h"
#include <math.h>

typedef struct point_s {
  float x,y;
} point_t;


typedef struct {
  char *k;
  int   v;
} mapSI_t;

int intcmp(void *a, void *b, void *aux)
{
  return (*((int *)a) - *((int *)b));
}

uint32_t inthash(void *a, void *aux)
{
  return utlhashint32(a);
}

uint32_t strhash(void *a, void *aux)
{
  return utlhashstring(a);
}


/*
int pntcmp(void *a, void *b)
{
  float delta;
  point_t *a_pnt = a, *b_pnt = b;
  delta = a_pnt->x - b_pnt->x;
  if (delta < 0.0) return -1;
  if (delta > 0.0) return 1;
  delta = a_pnt->y - b_pnt->y;
  if (delta < 0.0) return -1;
  if (delta > 0.0) return 1;
  return 0;
}
*/
void logtable(vec_t v)
{  
  uint8_t *p = v->vec;
  uint32_t *q;
  int32_t delta;
  uint32_t k;
  
  for (k=0; k<vecmax(v);k++) {
    q = (uint32_t *)p;
    if (q[1] == 0xFFFFFFFF) delta = -1;
    else delta = k-(q[1] & (vecmax(v)-1)); 
    logprintf("[%3d] -> h: %08X (%4d) key: %d",k,q[1],delta,q[0]);
    p+=v->esz;
  }
}

int main(int argc, char *argv[])
{
  
  vec_t v;
  int *pk;
  int32_t *pi;
  int32_t k;
  int kk=-1;
  /*
  uint64_t *pu;
  point_t p,q;
  point_t *pq;
  */
  
  logopen("l_vec2.log","w");
  
  v=vecnew(int,intcmp);
  
  logcheck(veccount(v) == 0);
  
  vecadd(int,v,37);
  logcheck(veccount(v) == 1);
  
  pk=vecgetptr(v,0);
  logcheck(pk && *pk == 37);
  
  vecadd(int,v,5);
  vecadd(int,v,79);

  pk=vecgetptr(v,0);
  logexpect(pk && *pk == 5,"[0]->%d",*pk);
  pk=vecgetptr(v,1);
  logexpect(pk && *pk == 37,"[1]->%d",*pk);
  pk=vecgetptr(v,2);
  logexpect(pk && *pk == 79,"[2]->%d",*pk);
  
  logcheck(veccount(v) ==3);

  pk = vecfirstptr(v);
  logcheck(pk && *pk == 5);
  
  pk = vecnextptr(v);
  logcheck(pk && *pk == 37);

  pk = vecnextptr(v);
  logcheck(pk && *pk == 79);

  pk = vecnextptr(v);
  logcheck(!pk);
  
  kk = vecfirst(int,v,-1);
  logcheck(kk == 5);
  
  kk = vecnext(int,v,-1);
  logcheck(kk == 37);

  kk = vecnext(int,v,-1);
  logcheck(kk == 79);

  kk = vecnext(int,v,-1);
  logcheck(kk == -1);
  
  vecfree(v);
  
  srand(time(0));
  
  v=vecnew(int,intcmp);
  #define N 1000
  vecset(int,v,N,0);
  vecclear(v);
  logprintf("cnt: %d max: %d",veccount(v),vecmax(v));
  if (N <= 10000) {
    logclock {
      for (k=1;k<=N;k++) 
        //vecset(int,v,k-1,k);
        vecadd(int,v, ((rand()&0x07) << 24) + k); 
    }
    logcheck(veccount(v) == N);
  }
  logprintf("Added elements: %d",veccount(v));
  
#if 1
  pk = vecgetptr(v,3);
  if (pk) kk = *pk;
  
  logprintf("Searching and removing %d",kk);
  logcheck(vecsearch(int,v,kk));
  logcheck(vecremove(int,v,kk));
  logcheck(veccount(v) == (N-1));
  logprintf("Current elements: %d",veccount(v));
  
  logcheck(vecsearch(int,v,-98) == NULL);
  
  vecfree(v);
  
  
  /* * HASH * */
  v = vecnew(int32_t,intcmp,inthash);
  logcheck(v)  ;
  logprintf("cnt: %d max: %d esz:%d",veccount(v), vecmax(v), v->esz);
  
  vecadd(int32_t,v,37);
  logcheck(veccount(v) == 1)  ;

  vecadd(int32_t,v,493);
  logcheck(veccount(v) == 2)  ;

  for (k = 0; k<20; k++) vecadd(int32_t,v,k )  ;
  logcheck(veccount(v) == 22)  ;
  
  vecadd(int32_t,v,-1);
  logcheck(veccount(v) == 23)  ;

  logprintf("cnt: %d max: %d esz:%d",veccount(v), vecmax(v), v->esz);
  logtable(v);
    
  pi = vecsearch(int32_t,v,12);
  if (logcheck(pi && *pi==12)) 
    logprintf("found: %d at %d",*pi,(int)((char *)pi - (char *)(v->vec))/v->esz);
  
  pi = vecsearch(int32_t,v,98);
  logcheck(!pi);
  
  vecremove(int32_t,v,493);
  logcheck(veccount(v) == 22)  ;

  logtable(v);

  vecremove(int32_t,v,2);
  logcheck(veccount(v) == 21)  ;

  logtable(v);

  
  vecfree(v);
  
  
  v = vecnew(int32_t,intcmp,inthash);
  logcheck(v)  ;
  logprintf("cnt: %d max: %d esz:%d",veccount(v), vecmax(v), v->esz);
  logclock {
    for (k=1;k<=N;k++) 
      //vecset(int,v,k-1,k);
      vecadd(int,v, ((rand()&0x07) << 24) + k); 
  }
  logcheck(veccount(v) == N);
  logprintf("Added elements: %d",veccount(v));
  vecfree(v);

#endif  
  
  logclose();

  exit(0);
}
