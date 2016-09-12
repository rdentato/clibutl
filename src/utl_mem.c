/*  .% Traced memory
**  ================

**      ______   ______  ______ 
**     /      \ / ___  )/      \
**    /  / /  //   ___//  / /  /
**   (__/_/__/ \_____/(__/_/__/ 
**   
**   
*/

#define UTL_MEM
#include "utl.h"


static char  *utl_BEG_CHK = "\xBE\xEF\xF0\x0D";
static char  *utl_END_CHK = "\xDE\xAD\xC0\xDA";
static char  *utl_CLR_CHK = "\xDE\xFA\xCE\xD0";
static size_t utl_mem_allocated;

typedef struct {
   size_t size;
   char   chk[4];
   char   blk[4];
} utl_mem_t;

#define utl_mem(x) ((utl_mem_t *)((char *)(x) -  offsetof(utl_mem_t, blk)))

int utl_check(void *ptr,char *file, int32_t line)
{
  utl_mem_t *p;
  
  if (ptr == NULL) return memNULL;
  p = utl_mem(ptr);
  if (memcmp(p->chk,utl_BEG_CHK,4)) { 
    logprintf("MEM Invalid or double freed %p (%lu %s:%d)",p->blk,
                                               utl_mem_allocated, file, line);     
    return memINVALID; 
  }
  if (memcmp(p->blk+p->size,utl_END_CHK,4)) {
    logprintf("MEM Boundary overflow %p [%lu] (%lu %s:%d)",
                              p->blk, p->size, utl_mem_allocated, file, line); 
    return memOVERFLOW;
  }
  logprintf("MEM Valid pointer %p (%lu %s:%d)",ptr, utl_mem_allocated, file, line); 
  return memVALID; 
}

void *utl_malloc(size_t size, char *file, int32_t line )
{
  utl_mem_t *p;
  
  if (size == 0) logprintf("MEM Requesto for 0 bytes (%lu %s:%d)",
                                                utl_mem_allocated, file, line);
  p = malloc(sizeof(utl_mem_t) +size);
  if (p == NULL) {
    logprintf("MEM Out of Memory (%lu %s:%d)",utl_mem_allocated, file, line);
    return NULL;
  }
  p->size = size;
  memcpy(p->chk,utl_BEG_CHK,4);
  memcpy(p->blk+p->size,utl_END_CHK,4);
  utl_mem_allocated += size;
  logprintf("MEM Allocated %p [%lu] (%lu %s:%d)",p->blk,size,utl_mem_allocated,file,line);
  return p->blk;
}

void *utl_calloc(size_t num, size_t size, char *file, int32_t line)
{
  void *ptr;
  
  size = num * size;
  ptr = utl_malloc(size,file,line);
  if (ptr)  memset(ptr,0x00,size);
  return ptr;
}

void utl_free(void *ptr, char *file, int32_t line)
{
  utl_mem_t *p=NULL;
  
  switch (utl_check(ptr,file,line)) {
    case memNULL  :    logprintf("MEM free NULL (%lu %s:%d)", 
                                                utl_mem_allocated, file, line);
                       break;
                          
    case memOVERFLOW : logprintf( "MEM Freeing an overflown block  (%lu %s:%d)", 
                                                           utl_mem_allocated, file, line);
    case memVALID :    p = utl_mem(ptr); 
                       memcpy(p->chk,utl_CLR_CHK,4);
                       utl_mem_allocated -= p->size;
                       if (p->size == 0)
                         logprintf("MEM Freeing a block of 0 bytes (%lu %s:%d)", 
                                             utl_mem_allocated, file, line);

                       logprintf("MEM free %p [%lu] (%lu %s %d)", ptr, 
                                 p?p->size:0,utl_mem_allocated, file, line);
                       free(p);
                       break;
                          
    case memINVALID :  logprintf("MEM free an invalid pointer! (%lu %s:%d)", 
                                                utl_mem_allocated, file, line);
                       break;
  }
}

void *utl_realloc(void *ptr, size_t size, char *file, int32_t line)
{
  utl_mem_t *p;
  
  if (size == 0) {
    logprintf("MEM realloc() used as free() %p -> [0] (%lu %s:%d)",
                                                      ptr,utl_mem_allocated, file, line);
    utl_free(ptr,file,line); 
  } 
  else {
    switch (utl_check(ptr,file,line)) {
      case memNULL   : logprintf("MEM realloc() used as malloc() (%lu %s:%d)", 
                                             utl_mem_allocated, file, line);
                          return utl_malloc(size,file,line);
                        
      case memVALID  : p = utl_mem(ptr); 
                       p = realloc(p,sizeof(utl_mem_t) + size); 
                       if (p == NULL) {
                         logprintf("MEM Out of Memory (%lu %s:%d)", 
                                          utl_mem_allocated, file, line);
                         return NULL;
                       }
                       utl_mem_allocated -= p->size;
                       utl_mem_allocated += size; 
                       logprintf("MEM realloc %p [%lu] -> %p [%lu] (%lu %s:%d)", 
                                       ptr, p->size, p->blk, size, 
                                       utl_mem_allocated, file, line);
                       p->size = size;
                       memcpy(p->chk,utl_BEG_CHK,4);
                       memcpy(p->blk+p->size,utl_END_CHK,4);
                       ptr = p->blk;
                       break;
    }
  }
  return ptr;
}

void *utl_strdup(void *ptr, char *file, int32_t line)
{
  char *dest;
  size_t size;
  
  if (ptr == NULL) {
    logprintf("MEM strdup NULL (%lu %s:%d)", utl_mem_allocated, file, line);
    return NULL;
  }
  size = strlen(ptr)+1;

  dest = utl_malloc(size,file,line);
  if (dest) memcpy(dest,ptr,size);
  logprintf("MEM strdup %p [%lu] -> %p (%lu %s:%d)", ptr, size, dest, 
                                                utl_mem_allocated, file, line);
  return dest;
}
#undef utl_mem

size_t utl_mem_used() {return utl_mem_allocated;}
