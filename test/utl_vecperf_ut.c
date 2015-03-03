/* 
**  (CC) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_C
#define UTL_UNITTEST
#define UTL_MEMCHECK
#include "utl.h"

FILE *f = NULL;
char vec[512];
int k=0;
int c=0;
int s=0;
vec_t vv=NULL;

typedef struct {
  int x; int y;
} point;

point p1;
point p2;
point *p;

int *v;

#define lg logStderr

int intcmp(const void *a, const void *b)
{
  return (*((int *)a) - *((int *)b));
}

int pstrcmp(const void *a, const void *b)
{
  char *aa = *((char **)a);
  char *bb = *((char **)b);
  if (aa == bb) return 0;
  if (aa == NULL) return 1;
  if (bb == NULL) return -1;
  return strcmp(aa,bb);
}


int main (int argc, char *argv[])
{
  logLevel(lg,"ALT");
  c = 100000000;
  
  logTestPlan(lg,"Performance vec") {
    vv=vecNew(int);
    logClockStart(lg);
    // vecSet(int,vv,c-1,0);
    for (k=0; k<c; k++) {
      vecSet(int,vv,k,k);
    } 
    logClockStop;
    logEQint(lg,"Set numbers forward", c,vecCount(vv));
    logClockStart(lg);
    vecSorted(vv,intcmp);   
    logClockStop;

    vv = vecFree(vv);
    
    vv=vecNew(int);
    logClockStart(lg);
    for (k=0; k<c; k++) {
      vecSet(int,vv,c-k-1,k);
    } 
    logClockStop;
    logEQint(lg,"Set numbers forward", c,vecCount(vv));
    
    logClockStart(lg);
    vecSorted(vv,intcmp);   
    logClockStop;
    
    vv = vecFree(vv);
  }
  
  return 0;
}
