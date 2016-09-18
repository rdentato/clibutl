/* 
**  (C) 2016 by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                     ___   __
**                  __/  /_ /  )
**          ___  __(_   ___)  /
**         /  / /  )/  /  /  /  Minimalist
**        /  /_/  //  (__/  /  C utility 
**       (____,__/(_____(__/  Library
** 

*/


//<<<//
#ifndef UTL_NOPMX

#define utl_pmx_MAXCAPT 16

extern int(*utl_pmx_ext)(char *pat, char *txt, int, int32_t ch);

extern char     *utl_pmx_capt[utl_pmx_MAXCAPT][2];
extern uint8_t   utl_pmx_capnum                  ;
extern char     *utl_pmx_error                   ;

#define pmxsearch(r,t)  utl_pmx_search(r,t)
#define pmxstart(n)    (utl_pmx_capt[n][0])
#define pmxend(n)      (utl_pmx_capt[n][1])
#define pmxcount()     (utl_pmx_capnum)
#define pmxlen(n)       utl_pmx_len(n)
#define pmxerror()     (utl_pmx_error)
#define pmxextend(f)   (utl_pmx_ext = f)

char  *utl_pmx_search(char *pat, char *txt);
size_t utl_pmx_len(uint8_t n);
void   utl_pmx_extend(int(*ext)(char *, char *,int, int32_t));

#endif
//>>>//
