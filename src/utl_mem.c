/* 
**  (C) 2009 by Remo Dentato (rdentato@gmail.com)
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
[[[
# Memory trace
**      ______  ______ ______ 
**     /      \/ ___  )      \
**    /  ) )  /   ___/  ) )  /
**   (__/_/__/\_____(__/_/__/ 
**   
**   

## Enabling trace
   You can trace memory allocation (and deallocation) by

 - `UTL_MEMCHECK` will instrument the dynamic memory functions (`malloc()`, `free()`, ...) 
   to log their activities and perform additional checks. It also enables logging
   even if `UTL_NOLOG` is defined.
   
 - `NDEBUG` will cause:
   - Memory checking functions will be disabled (even if `UTL_MEMCHECK`
     is defined).
]]]
*/

#define UTL_MEM
#include "utl.h"

//<<<//
#ifndef UTL_NOMEM

#ifndef memINVALID
#define memINVALID    -2
#define memOVERFLOW   -1
#define memVALID       0
#define memNULL        1
#endif

static const char  *utl_BEG_CHK = "\xBE\xEF\xF0\x0D";
static const char  *utl_END_CHK = "\xDE\xAD\xC0\xDA";
static const char  *utl_CLR_CHK = "\xDE\xFA\xCE\xD0";
static size_t utl_mem_allocated;

typedef struct {
   size_t size;
   char   chk[4];
   char   blk[4];
} utl_mem_t;

#define utl_mem(x) ((utl_mem_t *)((char *)(x) -  offsetof(utl_mem_t, blk)))

int utl_check(void *ptr,const char *file, int32_t line)
{
  utl_mem_t *p;
  
  if (ptr == NULL) return memNULL;
  p = utl_mem(ptr);
  if (memcmp(p->chk,utl_BEG_CHK,4)) { 
    logprintf("TRC Invalid or double freed %p (%lu)\x09:%s:%d\x09",p->blk,
                                               (unsigned long)utl_mem_allocated, file, line);     
    return memINVALID; 
  }
  if (memcmp(p->blk+p->size,utl_END_CHK,4)) {
    logprintf("TRC Boundary overflow %p [%lu] (%lu)\x09:%s:%d\x09",
                              p->blk, (unsigned long)p->size, (unsigned long)utl_mem_allocated, file, line); 
    return memOVERFLOW;
  }
  logprintf("TRC Valid pointer %p (%lu)\x09:%s:%d\x09",ptr, (unsigned long)utl_mem_allocated, file, line); 
  return memVALID; 
}

void *utl_malloc(size_t size, const char *file, int32_t line )
{
  utl_mem_t *p;
  
  if (size == 0) logprintf("TRC Request for 0 bytes (%lu)\x09:%s:%d\x09",
                                                (unsigned long)utl_mem_allocated, file, line);
  p = (utl_mem_t *)malloc(sizeof(utl_mem_t) +size);
  if (p == NULL) {
    logprintf("TRC Out of Memory (%lu)\x09:%s:%d\x09",(unsigned long)utl_mem_allocated, file, line);
    return NULL;
  }
  p->size = size;
  memcpy(p->chk,utl_BEG_CHK,4);
  memcpy(p->blk+p->size,utl_END_CHK,4);
  utl_mem_allocated += size;
  logprintf("TRC Allocated %p [%lu] (%lu)\x09:%s:%d\x09",p->blk,(unsigned long)size,(unsigned long)utl_mem_allocated,file,line);
  return p->blk;
}

void *utl_calloc(size_t num, size_t size, const char *file, int32_t line)
{
  void *ptr;
  
  size = num * size;
  ptr = utl_malloc(size,file,line);
  if (ptr)  memset(ptr,0x00,size);
  return ptr;
}

void utl_free(void *ptr, const char *file, int32_t line)
{
  utl_mem_t *p=NULL;
  
  switch (utl_check(ptr,file,line)) {
    case memNULL  :    logprintf("TRC free NULL (%lu)\x09:%s:%d\x09", 
                                                (unsigned long)utl_mem_allocated, file, line);
                       break;
                          
    case memOVERFLOW : logprintf("TRC Freeing an overflown block  (%lu)\x09:%s:%d\x09", 
                                                           (unsigned long)utl_mem_allocated, file, line);
    case memVALID :    p = utl_mem(ptr); 
                       memcpy(p->chk,utl_CLR_CHK,4);
                       utl_mem_allocated -= p->size;
                       if (p->size == 0)
                         logprintf("TRC Freeing a block of 0 bytes (%lu)\x09:%s:%d\x09", 
                                             (unsigned long)utl_mem_allocated, file, line);

                       logprintf("TRC free %p [%lu] (%lu)\x09:%s:%d\x09", ptr, 
                                 (unsigned long)(p?p->size:0),(unsigned long)utl_mem_allocated, file, line);
                       free(p);
                       break;
                          
    case memINVALID :  logprintf("TRC free an invalid pointer! (%lu)\x09:%s:%d\x09", 
                                                (unsigned long)utl_mem_allocated, file, line);
                       break;
  }
}

void *utl_realloc(void *ptr, size_t size, const char *file, int32_t line)
{
  utl_mem_t *p;
  
  if (size == 0) {
    logprintf("TRC realloc() used as free() %p -> [0] (%lu)\x09:%s:%d\x09",
                                                      ptr,(unsigned long)utl_mem_allocated, file, line);
    utl_free(ptr,file,line); 
  } 
  else {
    switch (utl_check(ptr,file,line)) {
      case memNULL   : logprintf("TRC realloc() used as malloc() (%lu)\x09:%s:%d\x09", 
                                             (unsigned long)utl_mem_allocated, file, line);
                          return utl_malloc(size,file,line);
                        
      case memVALID  : p = utl_mem(ptr); 
                       p = (utl_mem_t *)realloc(p,sizeof(utl_mem_t) + size); 
                       if (p == NULL) {
                         logprintf("TRC Out of Memory (%lu)\x09:%s:%d\x09", 
                                          (unsigned long)utl_mem_allocated, file, line);
                         return NULL;
                       }
                       utl_mem_allocated -= p->size;
                       utl_mem_allocated += size; 
                       logprintf("TRC realloc %p [%lu] -> %p [%lu] (%lu)\x09:%s:%d\x09", 
                                       ptr, (unsigned long)p->size, p->blk, (unsigned long)size, 
                                       (unsigned long)utl_mem_allocated, file, line);
                       p->size = size;
                       memcpy(p->chk,utl_BEG_CHK,4);
                       memcpy(p->blk+p->size,utl_END_CHK,4);
                       ptr = p->blk;
                       break;
    }
  }
  return ptr;
}

void *utl_strdup(const char *ptr, const char *file, int32_t line)
{
  char *dest;
  size_t size;
  
  if (ptr == NULL) {
    logprintf("TRC strdup NULL (%lu)\x09:%s:%d\x09", (unsigned long)utl_mem_allocated, file, line);
    return NULL;
  }
  size = strlen(ptr)+1;

  dest = (char *)utl_malloc(size,file,line);
  if (dest) memcpy(dest,ptr,size);
  logprintf("TRC strdup %p [%lu] -> %p (%lu)\x09:%s:%d\x09", ptr, (unsigned long)size, dest, 
                                                (unsigned long)utl_mem_allocated, file, line);
  return dest;
}
#undef utl_mem

size_t utl_mem_used(void) {return utl_mem_allocated;}


mpl_t utl_mpl_new()
{
  mpl_t mp = NULL;
  mp = malloc(sizeof(mpl_s));
  if (mp) {
    mp->used.next   = NULL;
    mp->unused.next = NULL;
    mp->used.size   = 0;
    mp->unused.size = 0;
  }
  return mp;
}

static utl_mpl_node_s *utl_mpl_getparent(utl_mpl_node_s *s, utl_mpl_node_s *p)
{
  while (s->next && s->next != p) {
    s = s->next;
  }
  return (s->next == p)? s : NULL;
}

static utl_mpl_node_s *utl_mpl_searchfit(utl_mpl_node_s *s, uint32_t sz)
{
   while (s->next && s->next->size < sz) {
     s = s->next;
   }
   return s->next ? s : NULL;
}

static utl_mpl_node_s *utl_mpl_searchpos(utl_mpl_node_s *s, uint32_t sz)
{
   while (s->next && s->next->size < sz) {
     s = s->next;
   }
   return s;
}

void *utl_mpl_malloc(mpl_t mp, uint32_t sz)
{
  utl_mpl_node_s *p = NULL;
  utl_mpl_node_s *parent;
  if (mp) {
    if (sz <= sizeof(utl_mpl_node_s *)) sz = sizeof(utl_mpl_node_s *);
    /* Search suitable block in the pool (sorted list) */
    parent = utl_mpl_searchfit(&mp->unused,sz);
    if (parent && parent->next->size < (sz*2)) {
      p = parent->next;
      parent->next = p->next;   // remove from "unused"
    }
    else { /* Otherwise alloc a new block */
      p = malloc(offsetof(utl_mpl_node_s,blk)+sz);
      //p = utl_malloc(offsetof(utl_mpl_node_s,blk)+sz,__FILE__,__LINE__);
      if (p) p->size = sz;
    }
    //logdebug("POOL MALLOC: %u result -> %p",sz, (void *)p);
    if (p) { /* add to the "used" list */
      p->next = mp->used.next;
      mp->used.next = p;
      p = (void *)(&p->blk);
    }
  }
  return p;
}

void *utl_mpl_free(mpl_t mp, void *e,int clean)
{
  utl_mpl_node_s *p;
  utl_mpl_node_s *parent;
  
  if (mp) { 
    if (clean) {  // release everything
      while (mp->used.next != NULL) {
        p = mp->used.next;
        mp->used.next = p->next; // remove from used
        parent = utl_mpl_searchpos(&mp->unused,p->size);
        //logdebug("POOL CLEAN BLOCK: %p %p",(void *)parent, (void *)p);
        p->next = parent->next; 
        parent->next = p;       // add to unused
      }
    }
    else if (e) { // release (move from "used" to "unused")
      // Search in "used"
      p = (utl_mpl_node_s *)(((char *)e) - offsetof(utl_mpl_node_s,blk));
      parent = utl_mpl_getparent(&mp->used,p);
      //logdebug("POOL FREE ELEM: %p %p",(void *) parent,(void *)p);
      if (parent) {
        parent->next = p->next; // Remove from used
        parent = utl_mpl_searchpos(&mp->unused,p->size);
        p->next = parent->next;
        p->blk  = NULL;
        parent->next = p;       // add to unused
      }
    }
    else { // free everything!
      parent = &mp->used;
      while (1) {
        while (parent->next != NULL) {
          p = parent->next;
          parent->next = p->next; // remove from list
          //logdebug("POOL FREE BLOCK: %p %p",(void *)parent, (void *)p);
          //utl_free(p,__FILE__,__LINE__);
          free(p);
        }
        if (parent == &mp->unused ) break;
        parent = &mp->unused;
      }
      free(mp);
    }
  }
  return NULL;
}

#endif
//>>>//
