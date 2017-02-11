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

int main(int argc, char *argv[])
{
  
  vec_t v;
  int *pk;
  /*int32_t *pi;*/
  int32_t k;
  int kk=-1;
  /*
  uint64_t *pu;
  point_t p,q;
  point_t *pq;
  */
  
  logopen("l_vec3.log","w");
  
  v=vecnew(int);
  logcheck(v && veccount(v) == 0);
  
  vecenq(int,v,543);
  logcheck(veccount(v) == 1);
  logcheck(vec(int,v)[0] == 543);
  logcheck(v->fst == 0);
  logcheck(v->lst == 1);
  logtrace("max: %d",v->max);
 
  for (k=0;k<10;k++) {
    vecenq(int,v,8000+k);
  }
  logcheck(veccount(v) == 11);
  logcheck(vec(int,v)[v->lst-1] == 8009);
  pk = veclastptr(v);
  logcheck(pk && *pk == 8009);
  pk = vecfirstptr(v);
  logexpect(pk && *pk == 543,"pk:%p *pk:%d",(void *)pk,(pk?*pk:-1));
  
  vecdeq(v);
  logcheck(veccount(v) == 10);
  pk = vecfirstptr(v);
  logexpect(pk && *pk == 8000,"pk:%p *pk:%d",(void *)pk,(pk?*pk:-1));
  kk=vecmax(v);
  k=9000;
  while(veccount(v) < kk) {
    vecenq(int,v,k++);
  }
  logcheck(kk==vecmax(v));
  logexpect(veccount(v)==vecmax(v),"Count: %d",veccount(v));
  
  // Now the queue is full. Let's add something else
  logtrace("before expansion max:%d  first: %d  last: %d count: %d", vecmax(v),v->fst,v->lst,veccount(v));
  vecenq(int,v,15);
  logcheck(kk<vecmax(v)); // vec expanded
  logtrace("after  expansion max:%d  first: %d  last: %d count: %d", vecmax(v),v->fst,v->lst,veccount(v));
  
  pk = vecfirstptr(v);
  logexpect(pk && *pk == 8000,"pk:%p *pk:%d",(void *)pk,(pk?*pk:-1));

  v=vecfree(v);
  logtrace("*** STACK ***");
  
  v=vecnew(int);
  
  vecpush(int,v,43);
  vecpush(int,v,53);
  vecpush(int,v,63);
  
  logexpect(veccount(v) == 3, "count: %d", veccount(v));
  logexpect(vec(int,v)[0] == 43, "[0]->%d",vec(int,v)[0]);
  logexpect(vec(int,v)[1] == 53, "[0]->%d",vec(int,v)[1]);
  logexpect(vec(int,v)[2] == 63, "[0]->%d",vec(int,v)[2]);
  
  kk = vectop(int,v,-1);
  logexpect(kk==63, "top: %d",kk);
  
  pk = vectopptr(v);
  logexpect(pk && kk == *pk,"pk:%p kk:%d *pk=%d",(void *)pk,kk,pk?*pk:0);
  
  vecdrop(v);
  kk = vectop(int,v,-1);
  logexpect(kk==53, "expect: 53 top: %d",kk);
  vecdrop(v);
  kk = vectop(int,v,-1);
  logexpect(kk==43, "expect: 43 top: %d",kk);
  vecdrop(v);
  kk = vectop(int,v,-1);
  logexpect(kk==-1, "expect: -1 top: %d",kk);
  logexpect(veccount(v) == 0, "count: %d", veccount(v));
  
  
  logclose();
  exit(0);
}
