/* 
**  (C) 2014 by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                     ___   __
**                  __/  /_ /  )
**          ___  __(_   ___)  /
**         /  / /  )/  /  /  /  Minimalist
**        /  (_/  //  (__/  /  C utility 
**       (____,__/(_____/__/  Library
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
#endif /* UTL_NOMEM */
//>>>//
