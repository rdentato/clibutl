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
  int ia = *((int *)a);
  int ib = *((int *)b);
  //logtrace("CMP: %d %d",ia,ib);
  return (ia - ib);
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
  
  vec_t v;
  //int *pk;
  int k;
  
  logopen("l_dpq.log","w");
  
  v=vecnew(int);
  logcheck(veccount(v) == 0);
  
  vecset(int,v,0,37);
  vecset(int,v,1,5);
  vecset(int,v,2,79);

  //logtable(v);
  
  vecsort(v,intcmp);
  logcheck(vecsorted(v));
  
  //logtable(v);
  
  vecset(int,v,0,3);
  vecset(int,v,1,57);
  vecset(int,v,2,79);

  //logtable(v);
  
  vecsort(v,intcmp);
  logcheck(vecsorted(v));
  
  //logtable(v);
  logtrace("random vector (small)");
  srand(time(0));
  for (k=0;k<=18;k++) {
    vecset(int,v,k,((rand() & 0xF) <<2)+k);
  }
  
  logtable(v);
  logclock {
    vecsort(v);
  }
  logcheck(vecsorted(v));
  logtable(v);
  
  logtrace("random vector (large)");
  
  #define N 1000
  for (k=0;k<=N;k++) {
    vecset(int,v,k,((rand() & 0xF) <<24)+k);
  }
  logtrace("done (%d on %d)",veccount(v), vecmax(v));
  
  //logtable(v);
  
  logclock {
    vecsort(v);
  }
  //logtable(v);
  
  logtrace("sorted vector");
  for (k=0;k<=N;k++) {
    vecset(int,v,k,10000000+k);
  }

  //logtable(v);
  logtrace("done");
  
  logclock {
    vecsort(v);
  }
  //logtable(v);

  vecfree(v);
  logclose();
  exit(0);
}
