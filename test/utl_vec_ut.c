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

int main (int argc, char *argv[])
{
  logLevel(logStderr,"DBG");
  logPre(logStderr,"#");

  p1.x =  1; p1.y =  2;
  p2.x = -1; p2.y = -2;

  TSTPLAN("utl unit test: vec") {
  
    TSTSECTION("vec creation") {
      TSTGROUP("vecNew()") {
        TSTCODE {
          vv = vecNew(point);
        }
        TSTNNULL("Is not NULL", vv );
        TSTEQINT("Mem Valid", utlMemValid, utlMemCheck(vv));
        TSTEQINT("Len 0", 0, vecCount(vv) );
      }
    }
    TSTSECTION("vec add") {
      TSTGROUP("vec set() 1") {
        vecSet(vv,0,&p1);
        TSTNNULL("Is not NULL", vv->vec );
        p = vecGet(vv,0);
        TSTEQINT("Set properly direct access", 1, p->x );
        TSTEQINT("Len 1", 1, vecCount(vv) );
     }
     TSTGROUP("vec set() 2") {
        vecSet(vv,1,&p2);
        TSTNNULL("Is not NULL", vv->vec );
        p = vecGet(vv,1);
        TSTEQINT("Set properly direct access", -1, p->x );
        TSTEQINT("Len 1", 2, vecCount(vv) );
      }
    }
    TSTSECTION("vec get") {
      TSTGROUP("vec get outbound") {
        p = vecGet(vv,vecCount(vv)+1);
        TSTNULL("Is Null",p);
      }
      TSTGROUP("vec as array") {
        p = vec(vv,point);
        TSTNNULL("Is not Null",p);
        TSTSKIP(p==NULL,"Vec array is NULL") {
          TSTEQINT("v[0]",2, p[0].y);
          TSTEQINT("v[1]",-2, p[1].y);
        }
      }
    }
    TSTSECTION("vec Resize") {
      TSTGROUP("vec grow") {
        vecResize(vv,30);
        TSTLEINT("Grown up", 30, vv->max);
      }
      TSTGROUP("vec shrink") {
        vecResize(vv,10);
        TSTLEINT("Shrink", 10, vv->max);
        TSTNOTE("Cur max: %d", vv->max);
      }
    }
    TSTSECTION("vec cleanup") {
      TSTGROUP("vec free") {
        vv = vecFree(vv);
        TSTNULL("Is NULL", vv );
      }
    }
  }
}
