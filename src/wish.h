/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**
*/

#ifndef WISH_H
#define WISH_H

#include <stdio.h>

void wishlist();

#ifndef NDEBUG

#define wish_0(t1,t2,e,r,o,x) \
  do {\
    t1 utl_exp = (e); t1 utl_ret = (r); \
    if (!(x)) { \
      ret=0; \
      fprintf(stderr, "Line %d: Wish for a (" #t1 ") " #o " '" #t2 "'; got '" #t2 "'\n", \
                                                                 __LINE__,utl_exp,utl_ret); \
    } \
  } while (0)
                             
#define wishint(e,o,r)   wish_0(int   ,%d,e,r,o, utl_ret o utl_exp)
#define wishptr(e,o,r)   wish_0(void *,%p,e,r,o, utl_ret o utl_exp)
#define wishdbl(e,o,r)   wish_0(double,%f,e,r,o, utl_ret o utl_exp)
#define wishstr(e,r,o,n) wish_0(char *,%s,e,r,o, (1 << (1+strcmp(utl_ret, utl_exp))) & n)

#define wishEQint(e,r)  wishint(e, == , r )
#define wishNEint(e,r)  wishint(e, != , r )
#define wishGTint(e,r)  wishint(e, >  , r )
#define wishGEint(e,r)  wishint(e, >= , r )
#define wishLTint(e,r)  wishint(e, <  , r )
#define wishLEint(e,r)  wishint(e, <= , r )

#define wishEQptr(e,r)  wishptr(e, == , r )
#define wishNEptr(e,r)  wishptr(e, != , r )
#define wishGTptr(e,r)  wishptr(e, >  , r )
#define wishGEptr(e,r)  wishptr(e, >= , r )
#define wishLTptr(e,r)  wishptr(e, <  , r )
#define wishLEptr(e,r)  wishptr(e, <= , r )

#define wishNULL (e)    wishEQPtr(e,NULL)
#define wishNNULL(e)    wishNEPtr(e,NULL)                                     

#define wishEQdbl(e,r)  wishdbl(e, == , r )
#define wishNEdbl(e,r)  wishdbl(e, != , r )
#define wishGTdbl(e,r)  wishdbl(e, >  , r )
#define wishGEdbl(e,r)  wishdbl(e, >= , r )
#define wishLTdbl(e,r)  wishdbl(e, <  , r )
#define wishLEdbl(e,r)  wishdbl(e, <= , r )

#define wishDELTA(d,e,r) wishLTdbl(d,fabs((e)-(r)))

#define wishEQstr(e,r)  wishstr(e, r, ==, 2 )
#define wishNEstr(e,r)  wishstr(e, r, !=, 5 )
#define wishGTstr(e,r)  wishstr(e, r, > , 4 )
#define wishGEstr(e,r)  wishstr(e, r, >=, 6 )
#define wishLTstr(e,r)  wishstr(e, r, < , 1 )
#define wishLEstr(e,r)  wishstr(e, r, <=, 3 )

#define wishprintf(...) fprintf(stderr, __VA_ARGS__ )
#define _wishprintf(...) 

#else

#define wishEQint(e,r)  
#define wishNEint(e,r)  
#define wishGTint(e,r)  
#define wishGEint(e,r)  
#define wishLTint(e,r)  
#define wishLEint(e,r)  

#define wishEQptr(e,r)  
#define wishNEptr(e,r)  
#define wishGTptr(e,r)  
#define wishGEptr(e,r)  
#define wishLTptr(e,r)  
#define wishLEptr(e,r)  

#define wishNULL (e)    
#define wishNNULL(e)                                    

#define wishEQdbl(e,r)  
#define wishNEdbl(e,r)  
#define wishGTdbl(e,r)  
#define wishGEdbl(e,r)  
#define wishLTdbl(e,r)  
#define wishLEdbl(e,r)  

#define wishDELTA(d,e,r)

#define wishEQstr(e,r)  
#define wishNEstr(e,r)  
#define wishGTstr(e,r)  
#define wishGEstr(e,r)  
#define wishLTstr(e,r)  
#define wishLEstr(e,r)  

#define wishprintf(...) 
#define _wishprintf(...) 

#endif

#endif /* WISH_H */

