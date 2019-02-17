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

#ifdef UTL_NET
#ifndef _WIN32
#define _POSIX_C_SOURCE 201112L
#endif
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#ifndef _WIN32
#include <unistd.h>
#endif
#include <string.h>
#include <stdarg.h>
#include <stddef.h>
#include <errno.h>
#include <time.h>
#include <ctype.h>
#include <assert.h>
#include <setjmp.h>
#include <limits.h>
#include <fcntl.h>

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

#define utl_arg0(_x0,...)                                     _x0
#define utl_arg1(_x0,_x1,...)                                 _x1
#define utl_arg2(_x0,_x1,_x2,...)                             _x2
#define utl_arg3(_x0,_x1,_x2,_x3,...)                         _x3
#define utl_arg4(_x0,_x1,_x2,_x3,_x4,...)                     _x4
#define utl_arg5(_x0,_x1,_x2,_x3,_x4,_x5,...)                 _x5
#define utl_arg6(_x0,_x1,_x2,_x3,_x4,_x5,_x6,...)             _x6
#define utl_arg7(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,...)         _x7
#define utl_arg8(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,_x8,...)     _x8
#define utl_arg9(_x0,_x1,_x2,_x3,_x4,_x5,_x6,_x7,_x8,_x9,...) _x9

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

void utl_randstate(uint64_t *s1, uint64_t *s2);
uint32_t utl_rand(void);
void utl_srand(uint64_t s1, uint64_t s2);

#define utlsrand(...)  utl_srand(utl_expand(utl_arg0(__VA_ARGS__,0)),\
                                 utl_expand(utl_arg1(__VA_ARGS__,0,0)))

#define utlrand() utl_rand()
#define utlrandstate(a,b) utl_randstate(a,b)

#ifdef __cplusplus
}
#endif
                               
//>>>//
