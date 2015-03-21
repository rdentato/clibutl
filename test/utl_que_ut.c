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
que_t qq=NULL;

typedef struct {
  int x; int y;
} point;

point p1;
point p2;
point *p;
point *pp;

int *v;

int k;

#define lg logStderr

int main (int argc, char *argv[])
{
  logLevel(lg,"DBG");

  p1.x =  1; p1.y =  2;
  p2.x = -1; p2.y = -2;

  logTestPlan(lg,"utl unit test: que") {
    qq = queNew(point);
    logNNULL(lg,qq);
    logEQint(lg,8,queMax(qq));
    logEQint(lg,0,queCount(qq));
    
    queAddPtr(qq,&p1);
    logEQint(lg,1,queCount(qq));
    queAddPtr(qq,&p2);
    logEQint(lg,2,queCount(qq));
    
    p = queFirstPtr(qq);
    logNNULL(lg, p);
    logEQint(lg,p1.x,p->x);
    p = queLastPtr(qq);
    logNNULL(lg, p);
    logEQint(lg,p2.x,p->x);
    
    queDel(qq);
    logEQint(lg,1,queCount(qq));
    p = queFirstPtr(qq);
    logNNULL(lg, p);
    logEQint(lg,p2.x,p->x);

    pp = queLastPtr(qq);
    logNNULL(lg, p);
    logEQptr(lg,p,pp);
    
    queDelLast(qq);
    logEQint(lg,0,queCount(qq));
    p = queFirstPtr(qq);
    logNULL(lg, p);
    pp = queLastPtr(qq);
    logNULL(lg, pp);

    queDel(qq);
    logNEint(lg,0,queEmpty(qq));
    
    qq = queFree(qq);
    logNULL(lg, qq);
    logNEint(lg,0,queEmpty(qq));
    
    qq = queNew(int);
    logEQint(lg,8,queMax(qq));
    logEQint(lg,0,queCount(qq));

    for (k=0; k<10; k++)  queAdd(int,qq,k);
    
    logEQint(lg,16,queMax(qq));
    logEQint(lg,10,queCount(qq));

    for (k=0; k<10; k++) {
      logEQint(lg,k,queFirst(int,qq,k));
      queDel(qq);
    }

    for (k=0; k<10; k++)  queAddFirst(int,qq,k);
    for (k=0; k<10; k++) {
      logEQint(lg,9-k,queFirst(int,qq,k));
      queDel(qq);
    }
    qq = queFree(qq);

    /* Test correct doubling part 1*/
    qq = queNew(int);
    for (k=0;k<5;k++) queAdd(int,qq,k);  /*  [0  1  2  3  4] x  x  x */
    queDel(qq);                          /*   x [1  2  3  4] x  x  x */
    queDel(qq);                          /*   x  x [2  3  4] x  x  x */
    for (k=5;k<10;k++) queAdd(int,qq,k); /*   8  9][2  3  4  5  6  7 */

    logEQint(lg, queMax(qq),queCount(qq));
    logEQint(lg, que_first(qq),que_last(qq));
    logEQint(lg, 2, que_first(qq));
    logEQint(lg, 2, que_last(qq));
                                         /*  0  1  2  3  4  5  6  7  8  9  .  1  2  3  4  5  */
    queAdd(int,qq,10);                   /*  x  x [2  3  4  5  6  7  8  9 10] x  x  x  x  x  */
    logEQint(lg, 16, queMax(qq));
    logEQint(lg,11,que_last(qq));
    
    qq = queFree(qq);
    
    /* Test correct doubling part 2*/
    qq = queNew(int);
    for (k=0;k<8;k++) queAdd(int,qq,k);  /*  [0  1  2  3  4  5  6  7] */
    logEQint(lg, queMax(qq),queCount(qq));
    for (k=0;k<6;k++) queDel(qq);        /*   x  x  x  x  x  x [6  7] */
    logEQint(lg,2,queCount(qq));
    logEQint(lg, 6, que_first(qq));
    logEQint(lg, 0, que_last(qq));
                                         
    for (k=8;k<14;k++) queAdd(int,qq,k);  /*  8  9 10 11 12 13][6  7 */ 
    logEQint(lg,8,queCount(qq));
    logEQint(lg, 6, que_first(qq));
    logEQint(lg, 6, que_last(qq));

                                          /*  0  1  2  3  4  5  6  7  8  9  .  1  2  3  4  5  */
    queAdd(int,qq,14);                    /*  8  9 10 11 12 13 14] x  x  x  x  x  x  x [6  7 */ 
    logEQint(lg, 16, queMax(qq));
    logEQint(lg,14,que_first(qq));
    
                                                /*  0  1  2  3  4  5  6   7  8  9  .  1  2  3  4  5  */
    for (k=15;k<22;k++) queAddFirst(int,qq,k);  /*  8  9 10 11 12 13 14][21 20 19 18 17 16 15  6  7 */ 
    logEQint(lg, queMax(qq),queCount(qq));
    logEQint(lg, 7, que_first(qq));
    logEQint(lg, 7, que_last(qq));
    
    v = vec(int,qq);
    logEQint(lg, 8, v[0]);
    logEQint(lg, 18, v[10]);
    qq = queFree(qq);
  }
  return 0;
}
