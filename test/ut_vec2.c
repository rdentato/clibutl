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


int intcmp(void *a, void *b)
{
  return (*((int *)a) - *((int *)b));
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
  int k;
  /*
  uint64_t *pu;
  point_t p,q;
  point_t *pq;
  */
  
  logopen("l_vec2.log","w");
  
  v=vecnew(int);
  vecsort(v,intcmp);
  
  logcheck(veccount(v) == 0);
  
  vecadd(int,v,37);
  logcheck(veccount(v) == 1);
  
  pk=vecget(int,v,0);
  logcheck(pk && *pk == 37);
  
  vecadd(int,v,5);
  vecadd(int,v,79);

  pk=vecget(int,v,0);
  if (!logcheck(pk && *pk == 5)) logprintf("[0]->%d",*pk);
  pk=vecget(int,v,1);
  if (!logcheck(pk && *pk == 37)) logprintf("[1]->%d",*pk);
  pk=vecget(int,v,2);
  if (!logcheck(pk && *pk == 79)) logprintf("[2]->%d",*pk);
  
  logcheck(veccount(v) ==3);
  
  vecfree(v);
  
  srand(time(0));
  
  v=vecnew(int);
  vecsort(v,intcmp);

  logclock {
    for (k=1;k<=10000;k++) 
      //vecset(int,v,k-1,k);
      vecadd(int,v, ((rand()&0x07) << 24) + k); 
  }
  logprintf("Added elements: %d",veccount(v));
  vecfree(v);
  
  logclose();

  exit(0);
}
