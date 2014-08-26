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
vec_t vv=NULL;

typedef struct {
  int x; int y;
} point;

point p1;
point p2;
point *p;

#define lg logStderr

int main (int argc, char *argv[])
{
  logLevel(lg,"DBG");

  p1.x =  1; p1.y =  2;
  p2.x = -1; p2.y = -2;

  logTestPlan(lg,"utl unit test: vec") {
  
    vv = vecNew(point);
 
    logTestNNULL(lg,"Is not NULL", vv );
    logTestEQInt(lg,"Mem Valid", utlMemValid, utlMemCheck(vv));
    logTestEQInt(lg,"Len 0", 0, vecCount(vv) );

    vecSet(vv,0,&p1);
    logTestNNULL(lg,"Is not NULL", vv->vec );
    p = vecGet(vv,0);
    logTestEQInt(lg,"Set properly direct access", 1, p->x );
    logTestEQInt(lg,"Len 1", 1, vecCount(vv) );

    vecSet(vv,1,&p2);
    logTestNNULL(lg,"Is not NULL", vv->vec );
    p = vecGet(vv,1);
    logTestEQInt(lg,"Set properly direct access", -1, p->x );
    logTestEQInt(lg,"Len 1", 2, vecCount(vv) );

    p = vecGet(vv,vecCount(vv)+1);
    logTestNULL(lg,"Is Null",p);

    p = vec(vv,point);
    logTestNNULL(lg,"Is not Null",p);
    
    logTestSkip(lg,"Vec array is NULL", p==NULL) {
      logTestEQInt(lg,"v[0]",2, p[0].y);
      logTestEQInt(lg,"v[1]",-2, p[1].y);
    }
    
    vecResize(vv,30);
    logTestLEInt(lg,"Grown up", 30, vv->max);

    vecResize(vv,10);
    logTestLEInt(lg,"Shrink", 10, vv->max);
    logTestNote(lg,"Cur max: %d", vv->max);

    vv = vecFree(vv);
    logTestNULL(lg,"Is NULL", vv );
  }
}
