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
#include <math.h>

typedef struct point_s {
  float x,y;
} point_t;


int intcmp(void *a, void *b, void *aux)
{
  return (*((int *)a) - *((int *)b));
}

int pntcmp(void *a, void *b, void *aux)
{
  double delta;
  point_t *a_pnt = a, *b_pnt = b;
  delta = a_pnt->x - b_pnt->x;
  if (delta < 0.0) return -1;
  if (delta > 0.0) return 1;
  delta = a_pnt->y - b_pnt->y;
  if (delta < 0.0) return -1;
  if (delta > 0.0) return 1;
  return 0;
}
void logtable(vec_t v)
{
  int *pk;
  int k =0;
  pk = vecfirstptr(v);
  while (pk) {
    logprintf("V[%d] -> %d",k++,*pk);
    pk = vecnextptr(v);
  }
}

int main(int argc, char *argv[])
{
  int k;
  vec_t v;
  int *pk;
  uint64_t *pu;
  point_t p,q;
  point_t *pq;
  
  logopen("l_vec.log","w");
  
  v=vecnew(int);
  logassert(v);
  logcheck(v->cnt == 0);
  logcheck(v->max == 16);
  
  vecset(int,v,9,123);
  logcheck(v->cnt == 10);
  logcheck(v->max == 16);

  vecset(int,v,19,124);
  logcheck(v->cnt == 20);
  logcheck(v->max == 24);
  
  k = vecget(int,v,9,-1);
  logcheck(k==123);

  k = vecget(int,v,999,-1);
  logcheck(k==-1);

  pk = vecgetptr(v,9);
  logcheck(pk && *pk==123);
  
  pk = vecgetptr(v,19);
  logcheck(pk && *pk==124);
  
  pk = vecgetptr(v,190);
  logcheck(!pk);
  
  v=vecfree(v);
  logassert(!v);
  
  logclose();

 
  logopen("l_vec.log","a");
  v=vecnew(point_t);
  logassert(v);
  
  logcheck(v->cnt == 0);
  logcheck(v->max == 16);
  
  p.x=12.3;
  p.y=-4.1;
  vecset(point_t,v,3,p);
  logcheck(v->cnt == 4);
  
  pq = vecgetptr(v,3);
  logcheck(pq && pq->x == p.x && pq->y == p.y);

  pq = vecgetptr(v,4);
  logcheck(!pq);
  
  pq = vecgetptr(v,23);
  logcheck(!pq);

  vecsort(v,pntcmp);
  logcheck(vecsorted(v));
  
  pq = vecsearch(point_t,v,p);
  logexpect(pq,"p.x=%f p.y=%f",p.x,p.y);
  
  v = vecfree(v);
  logclose();  
  
  logopen("l_vec.log","a");
  v = vecnew(uint64_t);
  logassert(v);
  
  vecset(uint64_t,v,0,10010);
  vecset(uint64_t,v,1,10012);
  vecset(uint64_t,v,2,10013);
  logcheck(veccount(v)==3);
  pu=vecgetptr(v,2);
  logcheck(pu && *pu == 10013);
  
  vecins(uint64_t,v,1,10011);
  logcheck(veccount(v)==4);
  logcheck((pu = vecgetptr(v,1)) && *pu == 10011);
  logcheck((pu = vecgetptr(v,2)) && *pu == 10012);
  
  /* Insert after max! */
  vecins(uint64_t,v,100,11000);
  logcheck((pu = vecgetptr(v,100)) && *pu == 11000);
  logcheck(veccount(v) == 101);
  
  /* delete */
  vecdel(v,2);
  logcheck((pu = vecgetptr(v,2)) && *pu == 10013);
  logcheck((pu = vecgetptr(v,99)) && *pu == 11000);
  logcheck(veccount(v) == 100);
  
  v = vecfree(v);
  logclose();  
  
  logopen("l_vec.log","a");
  v=vecnew(point_t);
  p.x = 1.; p.y = 1.;
  q.x = 2.; q.y = 2.;
  vecset(point_t,v,0,p);
  vecset(point_t,v,1,q);
  logcheck(veccount(v)==2);
  {
    FILE *f;
    f = fopen("l_point.tmp","wb");
    logassert(f);
    vecwrite(v,0,veccount(v),f);
    fclose(f);
    f = fopen("l_point.tmp","rb");
    logassert(f);
    vecread(v,2,2,f);
    fclose(f);
    if (!logcheck(veccount(v) == 4)) {
      logprintf(" Count: %d",veccount(v));
    }
    pq = vecgetptr(v,2);
    logcheck(pq && pq->x == p.x && pq->y == p.y);
    
    pq = vecgetptr(v,3);
    if (!logcheck(pq && pq->x == q.x && pq->y == q.y)) {
      if (pq) logprintf("pq: %p pq->x:%f pq->y:%f",(void *)pq, pq->x, pq->y);
      else logprintf("pq is NULL");
    }
  }
  
  v = vecfree(v);

#if 1  
  #define MAXMAX 10000000
  
  v = vecnew(uint64_t);
  logprintf("inserting %d elements", MAXMAX);
  logclock {
    for (k=0; k<MAXMAX;k++) 
      vecset(uint64_t,v,k,k);
  }
  vecfree(v);
  v = vecnew(uint64_t);
  logprintf("inserting %d elements", MAXMAX);
  vecset(uint64_t,v,MAXMAX-1,0);
  logclock {
    for (k=0; k<MAXMAX;k++) 
      vecset(uint64_t,v,k,k);
  }
  vecfree(v);
  
#endif  

  v = vecnew(int);
  vecset(int,v,0,12);
  vecset(int,v,1,3);
  vecset(int,v,2,5);
  vecset(int,v,3,2);
  
  logtable(v);
  vecsort(v,intcmp);
  logtrace("VEC: After sort");
  logtable(v);
  
  logcheck((pk=vecgetptr(v,0)) && *pk == 2);
  logcheck((pk=vecgetptr(v,1)) && *pk == 3);
  logcheck((pk=vecgetptr(v,3)) && *pk == 12);
  
  logcheck(vecissorted(v));
  vecset(int,v,2,15);
  logcheck(!vecissorted(v));

  vecsort(v,intcmp);
  logcheck((pk=vecgetptr(v,3)) && *pk == 15);
  
  pk = vecsearch(int,v,3);
  logcheck(pk && *pk == 3 && pk == (vec(int,v)+1));

  pk = vecsearch(int,v,9);
  logcheck(!pk);
  
  v = vecfree(v);

  v = vecnew(double);
  vecset(double,v,0,12.1);
  logcheck(12.1 == vecget(double,v,0,0));
  
  logcheck(-1. == vecget(double,v,10000,-1.));
  
  logcheck(12.1 == vecget(double,v,0,0));
  v = vecfree(v);
     
  logclose();

  exit(0);
}
