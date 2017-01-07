//<<<//
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
#ifndef UTL_H
#define UTL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <setjmp.h>


#ifdef NDEBUG

/* Give NDEBUG precedence over DEBUG */
#ifdef DEBUG
#undef DEBUG
#endif

/* Disable memory logging if NDEBUG is defined */
#ifdef UTL_MEMCHECK
#undef UTL_MEMCHECK
#endif

#endif /* NDEBUG */

#ifdef UTL_MEMCHECK
#ifdef UTL_NOLOG
#undef UTL_NOLOG
#endif
#endif

int   utl_ret(int x);
void *utl_retptr(void *x);

uint32_t utl_hash_string(void *key);
uint32_t utl_hash_int32(void *key);

#define utlhashstring  utl_hash_string
#define utlhashint32   utl_hash_int32

extern const char *utl_emptystring;

// utl_expand is here just to please Microsoft C whose preprocessor
// behaves differently from the other (up to VS2015, at least)
#define utl_expand(x) x

#define utl_arg0(x1,...)        x1
#define utl_arg1(x1,x2,...)     x2
#define utl_arg2(x1,x2,x3,...)  x3

//uint32_t utl_rnd();
  
//>>>//
