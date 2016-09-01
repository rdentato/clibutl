#define UTL_MAIN
#include "utl.h"
#include <math.h>

typedef struct point_s {
  float x,y;
} point_t;

int intcmp(void *a, void *b)
{
  return (*((int *)a) - *((int *)b));
}

int main(int argc, char *argv[])
{
  
  vec_t v;
  int *pk;
  uint64_t *pu;
  point_t p,q;
  point_t *pq;
  
  logopen("ut_vec.log","w");
  
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
  
  pk = vecget(int,v,9);
  logcheck(pk && *pk==123);
  
  pk = vecget(int,v,19);
  logcheck(pk && *pk==124);
  
  pk = vecget(int,v,190);
  logcheck(!pk);
  
  v=vecfree(v);
  logassert(!v);
  
  logclose();

 
  logopen("ut_vec.log","a");
  v=vecnew(point_t);
  logassert(v);
  
  logcheck(v->cnt == 0);
  logcheck(v->max == 16);
  
  p.x=12.3;
  p.y=-4.1;
  vecset(point_t,v,3,p);
  logcheck(v->cnt == 4);
  
  pq = vecget(point_t,v,3);
  logcheck(pq && pq->x == p.x && pq->y == p.y);

  pq = vecget(point_t,v,4);
  logcheck(!pq);
  
  pq = vecget(point_t,v,23);
  logcheck(!pq);
  
  v = vecfree(v);
  logclose();  
  
  logopen("ut_vec.log","a");
  v = vecnew(uint64_t);
  logassert(v);
  
  vecset(uint64_t,v,0,10010);
  vecset(uint64_t,v,1,10012);
  vecset(uint64_t,v,2,10013);
  logcheck(veccount(v)==3);
  pu=vecget(uint64_t,v,2);
  logcheck(pu && *pu == 10013);
  
  vecins(uint64_t,v,1,10011);
  logcheck(veccount(v)==4);
  logcheck((pu = vecget(uint64_t,v,1)) && *pu == 10011);
  logcheck((pu = vecget(uint64_t,v,2)) && *pu == 10012);
  
  /* Insert after max! */
  vecins(uint64_t,v,100,11000);
  logcheck((pu = vecget(uint64_t,v,100)) && *pu == 11000);
  logcheck(veccount(v) == 101);
  
  /* delete */
  vecdel(v,2);
  logcheck((pu = vecget(uint64_t,v,2)) && *pu == 10013);
  logcheck((pu = vecget(uint64_t,v,99)) && *pu == 11000);
  logcheck(veccount(v) == 100);
  
  v = vecfree(v);
  logclose();  
  
  logopen("ut_vec.log","a");
  v=vecnew(point_t);
  p.x = 1.; p.y = 1.;
  q.x = 2.; q.y = 2.;
  vecset(point_t,v,0,p);
  vecset(point_t,v,1,q);
  logcheck(veccount(v)==2);
  {
    FILE *f;
    f = fopen("test.pnt","wb");
    logassert(f);
    vecwrite(v,0,veccount(v),f);
    fclose(f);
    f = fopen("test.pnt","rb");
    logassert(f);
    vecread(v,2,2,f);
    fclose(f);
    if (!logcheck(veccount(v) == 4)) {
      logprintf(" Count: %d",veccount(v));
    }
    pq = vecget(point_t,v,2);
    logcheck(pq && pq->x == p.x && pq->y == p.y);
    
    pq = vecget(point_t,v,3);
    if (!logcheck(pq && pq->x == q.x && pq->y == q.y)) {
      if (pq) logprintf("pq: %p pq->x:%f pq->y:%f",pq, pq->x, pq->y);
      else logprintf("pq is NULL");
    }
  }
  
  v = vecfree(v);
  logclose();  

#if 0  
  #define MAXMAX 100000000
  int k;
  
  logopen("ut_vec.log","a");
  v = vecnew(uint64_t);
  logprintf("inserting elements");
  for (k=0; k<MAXMAX;k++) 
    vecset(uint64_t,v,k,k);
  logclose();
  
  logopen("ut_vec.log","a");
  v = vecnew(uint64_t);
  logprintf("inserting elements");
  vecset(uint64_t,v,MAXMAX-1,0);
  for (k=0; k<MAXMAX;k++) 
    vecset(uint64_t,v,k,k);
  
  vecfree(v);
  logclose();
#endif  

  logopen("ut_vec.log","a");
  v = vecnew(int);
  vecset(int,v,0,12);
  vecset(int,v,1,3);
  vecset(int,v,2,5);
  vecset(int,v,3,2);
  
  vecsort(v,intcmp);
  
  logcheck((pk=vecget(int,v,0)) && *pk == 2);
  logcheck((pk=vecget(int,v,1)) && *pk == 3);
  logcheck((pk=vecget(int,v,3)) && *pk == 12);
  
  logcheck(vecissorted(v));
  vecset(int,v,2,15);
  logcheck(!vecissorted(v));

  vecsort(v,intcmp);
  logcheck((pk=vecget(int,v,3)) && *pk == 15);
  
  pk = vecsearch(int,v,3);
  logcheck(pk && *pk == 3 && pk == (vec(int,v)+1));

  pk = vecsearch(int,v,9);
  logcheck(!pk);

  
  vecfree(v);
  
  logclose();

  exit(0);
}