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


//<<<//
#ifndef UTL_NOPMX

#define utl_pmx_MAXCAPT 16

extern int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch);

extern const char  *utl_pmx_capt[utl_pmx_MAXCAPT][2];
extern uint8_t      utl_pmx_capnum                  ;
extern const char  *utl_pmx_error                   ;

#define pmxsearch(r,t)  utl_pmx_search(r,t)
#define pmxstart(n)    (utl_pmx_capt[n][0])
#define pmxend(n)      (utl_pmx_capt[n][1])
#define pmxcount()     (utl_pmx_capnum)
#define pmxlen(n)       utl_pmx_len(n)
#define pmxerror()     (utl_pmx_error?utl_pmx_error:utl_emptystring)
#define pmxextend(f)   (void)(utl_pmx_ext = f)

const char *utl_pmx_search(const char *pat, const char *txt);
size_t utl_pmx_len(uint8_t n);
void   utl_pmx_extend(int(*ext)(const char *, const char *,int, int32_t));

#endif
//>>>//
