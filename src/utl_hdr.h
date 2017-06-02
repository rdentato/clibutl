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
#define UTL_NOTRY
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
#include <limits.h>


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
#define utlemptystring utl_emptystring;

// utl_expand() is just to please Microsoft C whose preprocessor
// behaves differently from the other compilers (up to VS2015, at least)
#define utl_expand(x) x

#define utl_arg0(x0,...)              x0
#define utl_arg1(x0,x1,...)           x1
#define utl_arg2(x0,x1,x2,...)        x2
#define utl_arg3(x0,x1,x2,x3,...)     x3
#define utl_arg4(x0,x1,x2,x3,x4,...)  x4

int utl_unpow2(int n);

//uint32_t utl_rnd();

// Extended to include the full Latin-1 characters
int utlisdigit(int ch);
int utlisalpha(int ch);
int utlisalnum(int ch);
int utlislower(int ch);
int utlisupper(int ch);
int utlisblank(int ch);
int utlisspace(int ch);
int utliscntrl(int ch);
int utlisgraph(int ch);
int utlispunct(int ch);
int utlisprint(int ch);
int utlisxdigit(int ch);

int utlfoldchar(int ch);
  
int utl_next_utf8(const char *txt, int32_t *ch);
#define utlnextutf8(...) \
                 utl_next_utf8(utl_expand(utl_arg0(__VA_ARGS__,NULL)),\
                               utl_expand(utl_arg1(__VA_ARGS__,NULL,NULL)))

typedef int (*utl_txt_action_t)(const char *, const char *, void *);
                               
//>>>//
