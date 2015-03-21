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

point p0;
point p1;
point p2;
point pp;
point *pt;


int *v;
int *p;

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

int pointcmp(const void *a, const void *b)
{
  const point *pa = a, *pb = b;
  int n;

  n = pa->x - pb->x;  // Only one point for each x
  //if (n==0) n = pa->y - pb->y;
  return n;
}

int main (int argc, char *argv[])
{
  logLevel(lg,"DBG");

  p1.x =  1; p1.y =  2;
  p2.x = -1; p2.y = -2;

  logTestPlan(lg,"utl unit test: vec") {
  
    vv = vecNew(point);
 
    logNNULL(lg, vv );
    logEQint(lg, utlMemValid, utlMemCheck(vv));
    logEQint(lg, 0, vecCount(vv) );

    vecSetPtr(vv,0,&p1);
    logNNULL(lg, vv->vec );
    
    pt = vecGetPtr(vv,0);
    logEQint(lg, 1, pt->x );
    logEQint(lg, 1, vecCount(vv) );

    vecSetPtr(vv,1,&p2);
    logNNULL(lg, vv->vec );
    pt = vecGetPtr(vv,1);
    logEQint(lg, -1, pt->x );
    logEQint(lg, 2, vecCount(vv) );

    pt = vecGetPtr(vv,vecCount(vv)+1);
    logNULL(lg,pt);

    pt = vec(point,vv);
    logNNULL(lg,pt);
    
    logTestSkip(lg,"Vec array is NULL", pt==NULL) {
      logEQint(lg,2, pt[0].y);
      logEQint(lg,-2, pt[1].y);
    }
    
    vecResize(vv,30);
    logGEint(lg, 30, vecMax(vv));

    vecResize(vv,10);
    logGEint(lg, 10, vecMax(vv));
    logTestNote(lg,"Cur max: %d", vecMax(vv));

    vv = vecFree(vv);
    logNULL(lg, vv );
    
    vv = vecNew(int);
    vecSet(int,vv,2,4);
    vecSet(int,vv,0,1);
    vecSet(int,vv,1,2);
    
    v = vec(int,vv);
    logEQint(lg,1,v[0]);
    logEQint(lg,2,v[1]);
    logEQint(lg,4,v[2]);

    logEQint(lg,1,vecGet(int,vv,0,-1));
    logEQint(lg,2,vecGet(int,vv,1,-1));
    logEQint(lg,4,vecGet(int,vv,2,-1));

    vv = vecFree(vv);
    
    vv = vecNew(int);
    vecSet(int,vv,25,10);
    vecSet(int,vv,26,20);
    vecSet(int,vv,27,30);
    logEQint(lg,10,vecGet(int,vv,25,-1));
    logEQint(lg,20,vecGet(int,vv,26,-1));
    logEQint(lg,30,vecGet(int,vv,27,-1));
   
    vecIns(int,vv,26,200);
    logEQint(lg,10,vecGet(int,vv,25,-1));
    logEQint(lg,200,vecGet(int,vv,26,-1));
    logEQint(lg,20,vecGet(int,vv,27,-1));
    logEQint(lg,30,vecGet(int,vv,28,-1));
    
    { /** White Box test!! **/
      int addme[10] = {201,202,203,204,205,206,207,208,209,210};
    
      utl_vec_ins(vv,27,10,addme);
      logGTint(lg,32,vecMax(vv));
      logEQint(lg,64,vecMax(vv));
      for (k=0; k<11; k++) {
        logEQint(lg,200+k,vecGet(int,vv,26+k,-1));     
      }
    }
    
    vecSet(int,vv,0,300);
    vecSet(int,vv,1,301);
    logEQint(lg, 300, vecGet(int,vv,0,-1));
    logEQint(lg, 301, vecGet(int,vv,1,-1));
    vecInsGap(vv,0,2);
    logEQint(lg,   0, vecGet(int,vv,0,-1));
    logEQint(lg,   0, vecGet(int,vv,1,-1));
    logEQint(lg, 300, vecGet(int,vv,2,-1));
    logEQint(lg, 301, vecGet(int,vv,3,-1));
    
    k = vecCount(vv);
    logInfo(lg,"Count is %d",k);
    vecDel(vv,3);
    logEQint(lg,k-1,vecCount(vv));
        
    vv = vecFree(vv);
    
    vv = vecNew(int);
    vecFill(int,vv,14,20,-37);
    logEQint(lg,21,vecCount(vv));
    logEQint(lg,-37,vecGet(int,vv,14,0));
    logEQint(lg,-37,vecGet(int,vv,17,0));
    logEQint(lg,-37,vecGet(int,vv,20,0));
    logNEint(lg,-37,vecGet(int,vv,13,0));
    
    vv = vecFree(vv);
    
    vv = vecNew(int);
    srand(time(0));
    for (k=0; k< 100; k++) {
      vecSet(int, vv, k, rand());
    }
    logEQint(lg,100,vecCount(vv));
    
    k = vecGet(int,vv,99,-1);
    c = vecGet(int,vv,98,-1);
    logInfo(lg,"v[99]=%d v[98]=%d",k,c);
    vecDel(vv,99);
    k = vecGet(int,vv,99,-1);
    logEQint(lg,-1,k);
    logEQint(lg,c,vecGet(int,vv,98,-1));
    
    k = vecGet(int,vv,0,-1);
    c = vecGet(int,vv,1,-1);
    logInfo(lg,"v[0]=%d v[1]=%d",k,c);
    vecDel(vv,0);
    k = vecGet(int,vv,0,-1);
    logEQint(lg,c,k);

    k = vecGet(int,vv,46,-1);
    c = vecGet(int,vv,47,-1);
    s = vecGet(int,vv,vecCount(vv)-1,-1);
    
    logInfo(lg,"v[46]=%d v[47]=%d",k,c);
    vecDel(vv,0);
    k = vecGet(int,vv,46,-1);
    logEQint(lg,c,k);
    logEQint(lg,s, vecGet(int,vv,vecCount(vv)-1,-1));

    vv = vecFree(vv);
    
    vv = stkNew(int);
    stkPush(int,vv,3);
    stkPush(int,vv,2);
    stkPush(int,vv,1);
    logEQint(lg,1,stkTop(int,vv,-1));
    stkPop(vv);
    logEQint(lg,2,stkTop(int,vv,-1));
    stkPop(vv);
    logEQint(lg,3,stkTop(int,vv,-1));
    stkPop(vv);
    logEQint(lg,1,stkEmpty(vv));
    vv = stkFree(vv);
  
    vv = vecNew(int);
    vecSet(int,vv,0,15);
    vecSet(int,vv,1,3);
    vecSet(int,vv,2,12);
    
    vecSorted(vv,intcmp);
    
    logEQint(lg,  3, vecGet(int, vv, 0, -1));
    logEQint(lg, 12, vecGet(int, vv, 1, -1));
    logEQint(lg, 15, vecGet(int, vv, 2, -1));
    
    p = vecSearch(int, vv, 3);
    logNNULL(lg,p);
    p = vecSearch(int, vv, 15);
    logNNULL(lg,p);
    p = vecSearch(int, vv, 12);
    logNNULL(lg,p);
    p = vecSearch(int, vv, 14);
    logNULL(lg,p);
    p = vecSearch(int, vv, 1);
    logNULL(lg,p);
    p = vecSearch(int, vv, 314);
    logNULL(lg,p);
    
    vv = vecFree(vv);
 
    vv = vecNew(char *);
    vecSet(char *,vv,0,"zz");
    vecSet(char *,vv,1,"aaaaaaaaaaaaaa");
    vecSet(char *,vv,2,"llll");
    vecSorted(vv,pstrcmp);
    logEQstr(lg, "aaaaaaaaaaaaaa", vecGet(char *, vv,0,"??"));
    logEQstr(lg, "llll", vecGet(char *, vv,1,"??"));
    logEQstr(lg, "zz", vecGet(char *, vv,2,"??"));
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

    logInfo(lg,"0 1 2 3 4 5 6 7 8");
    { int *vi = vec(int,vv);
      logInfo(lg,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    logEQint(lg, 0, vecSorted(vv));
    
    p=vecSearch(int,vv,3);
    
    logNEint(lg, 0, vecSorted(vv));
    
    logNNULL(lg,p);
    
    logInfo(lg,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(lg,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    vecAdd(int,vv,0);
    logEQint(lg,8,vv->cnt);
    logEQint(lg,0,vec(int,vv)[0]);
    logInfo(lg,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(lg,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    vecAdd(int,vv,4);
    logEQint(lg,9,vv->cnt);
    logEQint(lg,4,vec(int,vv)[4]);
    
    logInfo(lg,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(lg,"%d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6]);
    }
    
    vecAdd(int,vv,9);
    logEQint(lg,10,vv->cnt);
    logEQint(lg,9,vec(int,vv)[9]);
    logInfo(lg,"0 1 2 3 4 5 6 7 8 9");
    { int *vi = vec(int,vv);
      logInfo(lg,"%d %d %d %d %d %d %d %d %d %d", vi[0], vi[1], vi[2], vi[3], vi[4], vi[5], vi[6], vi[7], vi[8], vi[9]);
    }
   
    vv = vecFree(vv);

    vv = vecNew(point,pointcmp);
  
    p0.x =-1; p0.y =-1;
    p1.x = 1; p1.y = 1;
    p2.x = 0; p2.y = 1;
    
    vecAdd(point, vv, p1);
    logEQint(lg,1,vecCount(vv));
    
    pp = vecGet(point , vv, 0, p0);
    
    logEQint(lg,p1.x,pp.x);
    logEQint(lg,p1.y,pp.y);
    
    pp = vecGet(point , vv, 13, p0);
    
    logEQint(lg,p0.x,pp.x);
    logEQint(lg,p0.y,pp.y);
    
    vecAdd(point, vv, p2);
    logEQint(lg,2,vecCount(vv));
    
    pp = vecGet(point , vv, 0, p0);
    
    logEQint(lg,p2.x,pp.x);
    logEQint(lg,p2.y,pp.y);
    
    pp = vecGet(point , vv, 1, p0);
    
    logEQint(lg,p1.x,pp.x);
    logEQint(lg,p1.y,pp.y);
    
    pp.y++;
    
    vecAdd(point,vv,pp);
    logEQint(lg,2,vecCount(vv));
    pp.y = -1;
    pp = vecGet(point , vv, 1, p0);
    logEQint(lg,p1.x,pp.x);
    logEQint(lg,p1.y+1,pp.y);
    
    
    vv = vecFree(vv);
   
  }
  
  return 0;
}
