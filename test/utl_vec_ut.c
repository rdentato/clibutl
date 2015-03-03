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
  logLevel(lg,"DBG");

  p1.x =  1; p1.y =  2;
  p2.x = -1; p2.y = -2;

  logTestPlan(lg,"utl unit test: vec") {
  
    vv = vecNew(point);
 
    logNNULL(lg,"Is not NULL", vv );
    logEQint(lg,"Mem Valid", utlMemValid, utlMemCheck(vv));
    logEQint(lg,"Len 0", 0, vecCount(vv) );

    vecSetPtr(vv,0,&p1);
    logNNULL(lg,"Is not NULL", vv->vec );
    
    p = vecGetPtr(vv,0);
    logEQint(lg,"Set properly direct access", 1, p->x );
    logEQint(lg,"Len 1", 1, vecCount(vv) );

    vecSetPtr(vv,1,&p2);
    logNNULL(lg,"Is not NULL", vv->vec );
    p = vecGetPtr(vv,1);
    logEQint(lg,"Set properly direct access", -1, p->x );
    logEQint(lg,"Len 1", 2, vecCount(vv) );

    p = vecGetPtr(vv,vecCount(vv)+1);
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
    
    { /** White Box test!! **/
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
    
    k = vecCount(vv);
    logInfo(lg,"Count is %d",k);
    vecDel(vv,3);
    logEQint(lg,"Deleted",k-1,vecCount(vv));
        
    vv = vecFree(vv);
    
    vv = vecNew(int);
    vecFill(int,vv,14,20,-37);
    logEQint(lg,"Filled (count)",21,vecCount(vv));
    logEQint(lg,"Filled (1st value)",-37,vecGet(int,vv,14,0));
    logEQint(lg,"Filled (mid value)",-37,vecGet(int,vv,17,0));
    logEQint(lg,"Filled (last value)",-37,vecGet(int,vv,20,0));
    logNEint(lg,"Filled (before 1st)",-37,vecGet(int,vv,13,0));
    
    vv = vecFree(vv);
    
    vv = vecNew(int);
    srand(time(0));
    for (k=0; k< 100; k++) {
      vecSet(int, vv, k, rand());
    }
    logEQint(lg,"Insert random",100,vecCount(vv));
    
    k = vecGet(int,vv,99,-1);
    c = vecGet(int,vv,98,-1);
    logInfo(lg,"v[99]=%d v[98]=%d",k,c);
    vecDel(vv,99);
    k = vecGet(int,vv,99,-1);
    logEQint(lg,"Deleted last",-1,k);
    logEQint(lg,"Deleted last",c,vecGet(int,vv,98,-1));
    
    k = vecGet(int,vv,0,-1);
    c = vecGet(int,vv,1,-1);
    logInfo(lg,"v[0]=%d v[1]=%d",k,c);
    vecDel(vv,0);
    k = vecGet(int,vv,0,-1);
    logEQint(lg,"Deleted first",c,k);

    k = vecGet(int,vv,46,-1);
    c = vecGet(int,vv,47,-1);
    s = vecGet(int,vv,vecCount(vv)-1,-1);
    
    logInfo(lg,"v[46]=%d v[47]=%d",k,c);
    vecDel(vv,0);
    k = vecGet(int,vv,46,-1);
    logEQint(lg,"Deleted middle",c,k);
    logEQint(lg,"Deleted middle",s, vecGet(int,vv,vecCount(vv)-1,-1));

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
  
    int *p;
 
    vv = vecNew(int);
    vecSet(int,vv,0,15);
    vecSet(int,vv,1,3);
    vecSet(int,vv,2,12);
    
    vecSorted(vv,intcmp);
    
    logEQint(lg,"Sort 1",  3, vecGet(int, vv, 0, -1));
    logEQint(lg,"Sort 2", 12, vecGet(int, vv, 1, -1));
    logEQint(lg,"Sort 3", 15, vecGet(int, vv, 2, -1));
    
    p = vecSearch(int, vv, 3);
    logNNULL(lg,"Found (int)",p);
    p = vecSearch(int, vv, 15);
    logNNULL(lg,"Found (int)",p);
    p = vecSearch(int, vv, 12);
    logNNULL(lg,"Found (int)",p);
    p = vecSearch(int, vv, 14);
    logNULL(lg,"Not found (int)",p);
    p = vecSearch(int, vv, 1);
    logNULL(lg,"Not found (int)",p);
    p = vecSearch(int, vv, 314);
    logNULL(lg,"Not found (int)",p);
    
    vv = vecFree(vv);
 
    vv = vecNew(char *);
    vecSet(char *,vv,0,"zz");
    vecSet(char *,vv,1,"aaaaaaaaaaaaaa");
    vecSet(char *,vv,2,"llll");
    vecSorted(vv,pstrcmp);
    logEQstr(lg,"Sort str 1", "aaaaaaaaaaaaaa", vecGet(char *, vv,0,"??"));
    logEQstr(lg,"Sort str 2", "llll", vecGet(char *, vv,1,"??"));
    logEQstr(lg,"Sort str 3", "zz", vecGet(char *, vv,2,"??"));
    vv = vecFree(vv);

    vv = vecNew(int,intcmp);

    /* except 0,4,9 */    
    vecSet(int, vv, 6, 5);
    vecSet(int, vv, 5, 3);
    vecSet(int, vv, 4, 2);
    vecSet(int, vv, 3, 1);
    vecSet(int, vv, 2, 8);
    vecSet(int, vv, 1, 7);
    vecSet(int, vv, 0, 6);

    logInfo(logStderr,"0 1 2 3 4 5 6 7 8");
    { int *vi = vec(int,vv);
      logInfo(logStderr,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }

    p=vecSearch(int,vv,3);
    
    logNNULL(logStderr,"Found 3",p);
    
    logInfo(logStderr,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(logStderr,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    vecAdd(int,vv,0);
    logEQint(logStderr,"Added (pos)",8,vv->cnt);
    logEQint(logStderr,"Added (val)",0,vec(int,vv)[0]);
    logInfo(logStderr,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(logStderr,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    vecAdd(int,vv,4);
    logEQint(logStderr,"Added (pos)",9,vv->cnt);
    logEQint(logStderr,"Added (val)",4,vec(int,vv)[4]);
    
    logInfo(logStderr,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(logStderr,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    vecAdd(int,vv,9);
    logEQint(logStderr,"Added (pos)",10,vv->cnt);
    logEQint(logStderr,"Added (val)",9,vec(int,vv)[9]);
    logInfo(logStderr,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(logStderr,"%d %d %d %d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6], vi[7], vi[8], vi[9]);
    }
    
  }
  
  return 0;
}
