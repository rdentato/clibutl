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

**      ______   ______  ______ 
**     /      \ / ___  )/      \
**    /  ) )  //   ___//  ) )  /
**   (__/_/__/ \_____/(__/_/__/ 
**   
**   
*/
//<<<//
#ifndef UTL_NOMEM

#ifndef memINVALID
#define memINVALID    -2
#define memOVERFLOW   -1
#define memVALID       0
#define memNULL        1
#endif

void  *utl_malloc   (size_t size, const char *file, int32_t line );
void  *utl_calloc   (size_t num, size_t size, const char *file, int32_t line);
void  *utl_realloc  (void *ptr, size_t size, const char *file, int32_t line);
void   utl_free     (void *ptr, const char *file, int32_t line );
void  *utl_strdup   (const char *ptr, const char *file, int32_t line);
                    
int    utl_check    (void *ptr, const char *file, int32_t line);
size_t utl_mem_used (void);


#ifdef UTL_MEMCHECK

#ifndef UTL_MEM
#define malloc(n)     utl_malloc(n,__FILE__,__LINE__)
#define calloc(n,s)   utl_calloc(n,s,__FILE__,__LINE__)
#define realloc(p,n)  utl_realloc(p,n,__FILE__,__LINE__)
#define free(p)       utl_free(p,__FILE__,__LINE__)

#ifdef strdup
#undef strdup
#endif
#define strdup(p)     utl_strdup(p,__FILE__,__LINE__)

#define memcheck(p)   utl_check(p,__FILE__, __LINE__)
#define memused()     utl_mem_used()
#endif /* UTL_MEM */

#else /* UTL_MEMCHECK */

#define memcheck(p)     memVALID
#define memused()       0

#endif /* UTL_MEMCHECK */


typedef struct utl_mpl_node_s {
  struct utl_mpl_node_s *next;
  uint32_t               size;
  struct utl_mpl_node_s *blk;
} utl_mpl_node_s;


typedef struct {
  utl_mpl_node_s used;
  utl_mpl_node_s unused;
} mpl_s, *mpl_t;

mpl_t utl_mpl_new(void);
void *utl_mpl_malloc(mpl_t mp, uint32_t sz);
void *utl_mpl_calloc(mpl_t mp, uint32_t sz);
void *utl_mpl_realloc(mpl_t mp, uint32_t sz);
void *utl_mpl_strdup(mpl_t mp, char *s);
void *utl_mpl_free(mpl_t mp, void *e,int clean);

#define mplnew()        utl_mpl_new()
#define mplmalloc(m,sz) utl_mpl_malloc(m,sz)
#define mplfree(...)    utl_mpl_free(\
                             (void *)utl_expand(utl_arg0(__VA_ARGS__,0,0,0)), \
                             (void *)utl_expand(utl_arg1(__VA_ARGS__,0,0,0)), \
                             (int)   utl_expand(utl_arg2(__VA_ARGS__,0,0,0))  ) 


#define mplclean(m)     utl_mpl_free(m,NULL,1);


#endif /* UTL_NOMEM */
//>>>//
