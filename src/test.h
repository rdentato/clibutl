/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**
*/

#ifndef TST_H
#define TST_H

#include <stdio.h>

#ifndef NDEBUG

#define tst_0(t1,t2,e,r,o,x) \
  do {\
    t1 utl_exp = (e); t1 utl_ret = (r); \
    if (!(x)) { \
      fprintf(stderr, "Line %d: expected (" #t1 ") " #o " '" #t2 "' . Got '" #t2 "'\n",__LINE__,utl_exp,utl_ret); \
    } \
  } while (0)
                             
#define tstint(e,o,r)   tst_0(int   ,%d,e,r,o, utl_exp o utl_ret)
#define tstptr(e,o,r)   tst_0(void *,%p,e,r,o, utl_exp o utl_ret)
#define tstdbl(e,o,r)   tst_0(double,%f,e,r,o, utl_exp o utl_ret)
#define tststr(e,r,o,n) tst_0(char *,%s,e,r,o, (1 << (1+strcmp(utl_ret, utl_exp))) & n)

#define tstEQint(e,r)  tstint(e, == , r )
#define tstNEint(e,r)  tstint(e, != , r )
#define tstGTint(e,r)  tstint(e, >  , r )
#define tstGEint(e,r)  tstint(e, >= , r )
#define tstLTint(e,r)  tstint(e, <  , r )
#define tstLEint(e,r)  tstint(e, <= , r )
                                    
#define tstEQptr(e,r)  tstptr(e, == , r )
#define tstNEptr(e,r)  tstptr(e, != , r )
#define tstGTptr(e,r)  tstptr(e, >  , r )
#define tstGEptr(e,r)  tstptr(e, >= , r )
#define tstLTptr(e,r)  tstptr(e, <  , r )
#define tstLEptr(e,r)  tstptr(e, <= , r )
        
#define tstNULL (e)    tstEQPtr(e,NULL)
#define tstNNULL(e)    tstNEPtr(e,NULL)                                     

#define tstEQdbl(e,r)  tstdbl(e, == , r )
#define tstNEdbl(e,r)  tstdbl(e, != , r )
#define tstGTdbl(e,r)  tstdbl(e, >  , r )
#define tstGEdbl(e,r)  tstdbl(e, >= , r )
#define tstLTdbl(e,r)  tstdbl(e, <  , r )
#define tstLEdbl(e,r)  tstdbl(e, <= , r )

#define tstDELTA(e,r,d) tstLTdbl(fabs((e)-(r)), d)

#define tstEQstr(e,r)  tststr(e, r, ==, 2 )
#define tstNEstr(e,r)  tststr(e, r, !=, 5 )
#define tstGTstr(e,r)  tststr(e, r, > , 4 )
#define tstGEstr(e,r)  tststr(e, r, >=, 6 )
#define tstLTstr(e,r)  tststr(e, r, < , 1 )
#define tstLEstr(e,r)  tststr(e, r, <=, 3 )

#define tstprintf(f, ...) fprintf(stderr,f, __VA_ARGS__ )

#else

#define tstEQint(e,r)  
#define tstNEint(e,r)  
#define tstGTint(e,r)  
#define tstGEint(e,r)  
#define tstLTint(e,r)  
#define tstLEint(e,r)  
                       
#define tstEQptr(e,r)  
#define tstNEptr(e,r)  
#define tstGTptr(e,r)  
#define tstGEptr(e,r)  
#define tstLTptr(e,r)  
#define tstLEptr(e,r)  
        
#define tstNULL (e)    
#define tstNNULL(e)                                    

#define tstEQdbl(e,r)  
#define tstNEdbl(e,r)  
#define tstGTdbl(e,r)  
#define tstGEdbl(e,r)  
#define tstLTdbl(e,r)  
#define tstLEdbl(e,r)  

#define tstDELTA(e,r,d)

#define tstEQstr(e,r)  
#define tstNEstr(e,r)  
#define tstGTstr(e,r)  
#define tstGEstr(e,r)  
#define tstLTstr(e,r)  
#define tstLEstr(e,r)  

#define tstprintf(f, ...) 

#endif

#endif /* TST_H */

#ifdef TST_IT
int main (int argc, char *argv[])
{
  tstGTint(3,5);
  tstEQdbl(2.5, 5./2);
  tstEQdbl(0.3333, 1./3);
  tstEQstr("aaa", "aaa");
  tstNEstr("bbb", "bbb");
  tstGTstr("aa", "bbb");
  tstGTstr("caa", "bbb");
  tstGTstr("bbb", "bbb");
  tstGEstr("bbb", "bbb");
}
#endif