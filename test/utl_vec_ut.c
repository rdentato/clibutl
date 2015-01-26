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

int *v;

#define lg logStderr

int main (int argc, char *argv[])
{
  logLevel(lg,"DBG");

  p1.x =  1; p1.y =  2;
  p2.x = -1; p2.y = -2;

  logTestPlan(lg,"utl unit test: vec") {
  
    vv = vecNew(point);
 
    logNNULL(lg,"Is not NULL", vv );
    logEQint(lg,"Mem Valid", utlMemValid, utlMemCheck(vv));
    logEQint(lg,"Len 0", 0, vecCount(vv) );

    vecSetRaw(vv,0,&p1);
    logNNULL(lg,"Is not NULL", vv->vec );
    p = vecGetRaw(vv,0);
    logEQint(lg,"Set properly direct access", 1, p->x );
    logEQint(lg,"Len 1", 1, vecCount(vv) );

    vecSetRaw(vv,1,&p2);
    logNNULL(lg,"Is not NULL", vv->vec );
    p = vecGetRaw(vv,1);
    logEQint(lg,"Set properly direct access", -1, p->x );
    logEQint(lg,"Len 1", 2, vecCount(vv) );

    p = vecGetRaw(vv,vecCount(vv)+1);
    logNULL(lg,"Is Null",p);

    p = vec(point,vv);
    logNNULL(lg,"Is not Null",p);
    
    logTestSkip(lg,"Vec array is NULL", p==NULL) {
      logEQint(lg,"v[0]",2, p[0].y);
      logEQint(lg,"v[1]",-2, p[1].y);
    }
    
    vecResize(vv,30);
    logGEint(lg,"Grown up", 30, vv->max);

    vecResize(vv,10);
    logGEint(lg,"Shrink", 10, vv->max);
    logTestNote(lg,"Cur max: %d", vv->max);

    vv = vecFree(vv);
    logNULL(lg,"Is NULL", vv );
    
    vv = vecNew(int);
    vecSet(int,vv,2,4);
    vecSet(int,vv,0,1);
    vecSet(int,vv,1,2);
    
    v = vec(int,vv);
    logEQint(lg,"vecSet",1,v[0]);
    logEQint(lg,"vecSet",2,v[1]);
    logEQint(lg,"vecSet",4,v[2]);

    logEQint(lg,"vecGet",1,vecGet(int,vv,0,-1));
    logEQint(lg,"vecGet",2,vecGet(int,vv,1,-1));
    logEQint(lg,"vecGet",4,vecGet(int,vv,2,-1));

    vv = vecFree(vv);
    
    vv = vecNew(int);
    vecSet(int,vv,25,10);
    vecSet(int,vv,26,20);
    vecSet(int,vv,27,30);
    logEQint(lg,"vecGet",10,vecGet(int,vv,25,-1));
    logEQint(lg,"vecGet",20,vecGet(int,vv,26,-1));
    logEQint(lg,"vecGet",30,vecGet(int,vv,27,-1));
   
    vecIns(int,vv,26,200);
    logEQint(lg,"vecIns",10,vecGet(int,vv,25,-1));
    logEQint(lg,"vecIns",200,vecGet(int,vv,26,-1));
    logEQint(lg,"vecIns",20,vecGet(int,vv,27,-1));
    logEQint(lg,"vecIns",30,vecGet(int,vv,28,-1));
    
    {
      int addme[10] = {201,202,203,204,205,206,207,208,209,210};
    
      utl_vec_ins(vv,27,10,addme);
      logGTint(lg,"increased by ins",32,vecMax(vv));
      logEQint(lg,"increased by ins",64,vecMax(vv));
      for (k=0; k<11; k++) {
        logEQint(lg,"Insert successful",200+k,vecGet(int,vv,26+k,-1));     
      }
    }
    
    vecSet(int,vv,0,300);
    vecSet(int,vv,1,301);
    logEQint(lg,"[0] is 300", 300, vecGet(int,vv,0,-1));
    logEQint(lg,"[1] is 301", 301, vecGet(int,vv,1,-1));
    vecInsGap(vv,0,2);
    logEQint(lg,"[0] is 0",   0, vecGet(int,vv,0,-1));
    logEQint(lg,"[1] is 0",   0, vecGet(int,vv,1,-1));
    logEQint(lg,"[2] is 300", 300, vecGet(int,vv,2,-1));
    logEQint(lg,"[3] is 301", 301, vecGet(int,vv,3,-1));
        
    vv = vecFree(vv);
    
    vv = vecNew(int);
    vecFill(int,vv,14,20,-37);
    logEQint(lg,"Filled (count)",21,vecCount(vv));
    logEQint(lg,"Filled (1st value)",-37,vecGet(int,vv,14,0));
    logEQint(lg,"Filled (mid value)",-37,vecGet(int,vv,17,0));
    logEQint(lg,"Filled (last value)",-37,vecGet(int,vv,20,0));
    logNEint(lg,"Filled (before 1st)",-37,vecGet(int,vv,13,0));
    
    vv = vecFree(vv);
    
    
    vv = stkNew(int);
    stkPush(int,vv,3);
    stkPush(int,vv,2);
    stkPush(int,vv,1);
    logEQint(lg,"stkTop",1,stkTop(int,vv,-1));
    stkPop(vv);
    logEQint(lg,"stkTop",2,stkTop(int,vv,-1));
    stkPop(vv);
    logEQint(lg,"stkTop",3,stkTop(int,vv,-1));
    stkPop(vv);
    logEQint(lg,"stkEmpty",1,stkEmpty(vv));
    vv = stkFree(vv);
  }
  return 0;
}
