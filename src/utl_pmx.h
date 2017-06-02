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

#define pmxaction_t utl_txt_action_t

typedef struct {
  const char *pat;
  const char *txt;
  int32_t min_n;
  int32_t max_n;
  int32_t n;
  int16_t inv;
  int16_t cap; 
} utl_pmx_state_s;

typedef struct {
const char       *start;
const char       *capt[utl_pmx_MAXCAPT][2];
const char       *error;
uint16_t          capnum;
uint16_t          utf8;
uint16_t          csens;
utl_pmx_state_s   stack[utl_pmx_MAXCAPT];
uint16_t          stack_ptr;
} pmx_t;


extern int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch);

//extern const char  *utl_pmx_capt[utl_pmx_MAXCAPT][2];
//extern uint8_t      utl_pmx_capnum                  ;
//extern const char  *utl_pmx_error                   ;

extern pmx_t utl_pmx_ ;


#define pmxsearch(r,t)  utl_pmx_search(r,t,0)
#define pmxmatch(r,t)   utl_pmx_search(r,t,1)
#define pmxstart(n)    (utl_pmx_.capt[n][0])
#define pmxend(n)      (utl_pmx_.capt[n][1])
#define pmxcount()     (utl_pmx_.capnum)
#define pmxlen(n)       utl_pmx_len(n)
#define pmxerror()     (utl_pmx_.error?utl_pmx_.error:utl_emptystring)
#define pmxextend(f)   (void)(utl_pmx_ext = f)
#define pmxscan(r,t,f,a) utl_pmx_scan(r,t,f,a)

#define pmxclear(p_)    utl_pmx_save(p_)
#define pmxrestore(p_) utl_pmx_restore(p_)

void        utl_pmx_save(pmx_t *);
void        utl_pmx_restore(pmx_t *);

const char *utl_pmx_search(const char *pat, const char *txt, int fromstart);
int         utl_pmx_len(uint8_t n);
void        utl_pmx_extend(int(*ext)(const char *, const char *,int, int32_t));
const char *utl_pmx_scan(const char *pat, const char *txt, pmxaction_t func, void *aux);



#endif
//>>>//
