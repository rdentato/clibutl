/* *** 2019-05-25 15:32:12 *** */
#line 2 "src/utl_hdr.c"
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
*/
#line 93 "src/utl_hdr.c"
#define UTL_MAIN
#include "utl.h"


const char *utl_emptystring = "";

int   utl_ret(int x)      {return x;}
void *utl_retptr(void *x) {return x;}

#ifndef UTL_NOTRY
utl_jb_t *utl_jmp_list = NULL; // For try/catch
#endif

/* * Collection of hash functions * */

/* Bob Jenkins' "one_at_a_time" hash function
**  http://burtleburtle.net/bob/hash/doobs.html
*/
uint32_t utl_hash_string(void *key)
{
  uint32_t h = 0x071f9f8f;
  uint8_t *k = key;
  
  while (*k)  {
    h += *k++;
    h += (h << 10);
    h ^= (h >> 6);
  }
  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);
  
  return h;
}

/* Bob Jenkins' integer hash function
**  http://burtleburtle.net/bob/hash/integer.html
*/

uint32_t utl_hash_int32(void *key)
{
  uint32_t h = *((uint32_t *)key);
  h = (h+0x7ed55d16) + (h<<12);
  h = (h^0xc761c23c) ^ (h>>19);
  h = (h+0x165667b1) + (h<<5);
  h = (h+0xd3a2646c) ^ (h<<9);
  h = (h+0xfd7046c5) + (h<<3);
  h = (h^0xb55a4f09) ^ (h>>16);
  return h;
}

/* Quick and dirty PRNG */
/*
uint32_t utl_rnd()
{ // xorshift
  static uint32_t rnd = 0;
  while (rnd == 0) rnd = (uint32_t)time(0);
	rnd ^= rnd << 13;
	rnd ^= rnd >> 17;
	rnd ^= rnd << 5;
	return rnd;
}
*/
/*
uint32_t utl_rnd()
{ // Linear Congruetial
  static uint32_t rnd = 0;
  if (rnd == 0) rnd = (uint32_t)time(0);
	rnd = 1664525 * rnd + 1013904223;
	return rnd;
}
*/

// *Really* minimal PCG32 code / (c) 2014 M.E. O'Neill / pcg-random.org
// Licensed under Apache License 2.0 (NO WARRANTY, etc. see website)
// http://www.pcg-random.org/download.html

static uint64_t rng_state = (uint64_t)&rng_state;
static uint64_t rng_inc = (uint64_t)&rng_inc;

uint32_t utl_rand()
{
    uint64_t oldstate = rng_state;
    // Advance internal state
    rng_state = oldstate * 6364136223846793005ULL + (rng_inc|1);
    // Calculate output function (XSH RR), uses old state for max ILP
    uint32_t xorshifted = ((oldstate >> 18u) ^ oldstate) >> 27u;
    uint32_t rot = oldstate >> 59u;
    return (xorshifted >> rot) | (xorshifted << ((-rot) & 31));
}

void utl_randstate(uint64_t *s1, uint64_t *s2)
{
  if (s1) *s1 = rng_state;
  if (s2) *s2 = rng_inc;
}

void utl_srand(uint64_t s1, uint64_t s2)
{
  if (s1 == 0) {
    int fd = open("/dev/urandom",O_RDONLY);
    if (fd >= 0) {
      read(fd, &s1, sizeof(s1));
      close(fd);
    }
  }
  rng_state = s1 ? s1 : (uint64_t)time(0);
  rng_inc   = s2 ? s2 : (uint64_t)&rng_inc;
}

/* returns log2(n) assuming n is 2^m */
int utl_unpow2(int n)
{ int r;

  r  =  (n & 0xAAAA) != 0;
  r |= ((n & 0xCCCC) != 0) << 1;
  r |= ((n & 0xF0F0) != 0) << 2;
  r |= ((n & 0xFF00) != 0) << 3;
  return r;
}


#line 33 "src/utl_utf.c"

static unsigned char utl_ENCODING[] = {
       /*  10   32   54   76   98   BA   DC   FE */
/* 0_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 1_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 2_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 3_ */ 0x88,0x88,0x88,0x88,0x88,0x00,0x00,0x00,
/* 4_ */ 0x20,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
/* 5_ */ 0x22,0x22,0x22,0x22,0x22,0x02,0x00,0x00,
/* 6_ */ 0x40,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
/* 7_ */ 0x44,0x44,0x44,0x44,0x44,0x04,0x00,0x00,
/* 8_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* 9_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* A_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* B_ */ 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
/* C_ */ 0x22,0x22,0x22,0x22,0x22,0x22,0x22,0x22,
/* D_ */ 0x22,0x22,0x22,0x02,0x22,0x22,0x22,0x42,
/* E_ */ 0x44,0x44,0x44,0x44,0x44,0x44,0x44,0x44,
/* F_ */ 0x44,0x44,0x44,0x04,0x44,0x44,0x44,0x44
};

/*
  Note that the table above is arranged so to make easy writing the
macro below.
Characters with odd code (i.e. ending with 1) are represented in the
higher half of the byte. So, the last bit of the code can be used to
shift right the byte and pick the higher half.

  To make the macro below a little bit less obscure:
  
    - The byte to pick from the table for character c is c/2 (i.e. c>>1)
    - If the character code is odd c&1 is 1 and the byteis shifted 4 bits right  

*/

#define utl_ENC(c) (utl_ENCODING[c>>1] >> ((c&1) << 2))

int utlisdigit(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x08);}
int utlisalpha(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x06);}
int utlisalnum(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x0E);}
int utlislower(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x04);}
int utlisupper(int ch) {return (ch <= 0xFF) && (utl_ENC(ch) & 0x02);}
int utlisblank(int ch) {return (ch == 0xA0) || ((ch <= 0xFF) && isblank(ch));}

int utlisspace(int ch)  {return (ch <= 0xFF) && isspace(ch);}
int utliscntrl(int ch)  {return (ch <= 0xFF) && iscntrl(ch);}
int utlisgraph(int ch)  {return (ch <= 0xFF) && isgraph(ch);} 
int utlispunct(int ch)  {return (ch <= 0xFF) && ispunct(ch);}
int utlisprint(int ch)  {return (ch <= 0xFF) && isprint(ch);}
int utlisxdigit(int ch) {return (ch <= 0xFF) && isxdigit(ch);}

int utlfoldchar(int ch)
{
  if (utlisupper(ch)) ch += 32; 
  return ch;
}

// Returns the length of the next UTF8 character and stores in ch
// (if it's not null) the corresponding codepoint.
// It is based on a work by Bjoern Hoehrmann:
//            http://bjoern.hoehrmann.de/utf-8/decoder/dfa
//
int utl_next_utf8(const char *txt, int32_t *ch)
{
  int len;
  uint8_t *s = (uint8_t *)txt;
  uint8_t first = *s;
  int32_t val;
  
  _logdebug("About to get UTF8: %s in %p",txt,ch);  
  fsm {
    fsmSTART {
      if (*s <= 0xC1) { val = *s; len = (*s > 0); fsmGOTO(end);    }
      if (*s <= 0xDF) { val = *s & 0x1F; len = 2; fsmGOTO(len2);   }
      if (*s == 0xE0) { val = *s & 0x0F; len = 3; fsmGOTO(len3_0); }
      if (*s <= 0xEC) { val = *s & 0x0F; len = 3; fsmGOTO(len3_1); }
      if (*s == 0xED) { val = *s & 0x0F; len = 3; fsmGOTO(len3_2); }
      if (*s <= 0xEF) { val = *s & 0x0F; len = 3; fsmGOTO(len3_1); }
      if (*s == 0xF0) { val = *s & 0x07; len = 4; fsmGOTO(len4_0); }
      if (*s <= 0xF3) { val = *s & 0x07; len = 4; fsmGOTO(len4_1); }
      if (*s == 0xF4) { val = *s & 0x07; len = 4; fsmGOTO(len4_2); }
      fsmGOTO(invalid);
    } 
    
    fsmSTATE(len4_0) {
      s++; if ( *s < 0x90 || 0xBF < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_1) {
      s++; if ( *s < 0x80 || 0xBF < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_2) {
      s++; if ( *s < 0x80 || 0x8F < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len3_0) {
      s++; if ( *s < 0xA0 || 0xBF < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len3_1) {
      s++; if ( *s < 0x80 || 0xBF < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }

    fsmSTATE(len3_2) {
      s++; if ( *s < 0x80 || 0x9F < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len2) {
      s++; if ( *s < 0x80 || 0xBF < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(end);
    }
    
    // Return 1 character if the sequence is invalid
    fsmSTATE(invalid) {val = first; len = 1;}
    
    fsmSTATE(end)   { }
  }
  if (ch) *ch = val;
  return len;
}


#line 286 "src/utl_log.c"
#ifndef UTL_NOLOG
#ifdef UTL_MAIN

FILE *utl_log_file = NULL;
uint32_t utl_log_check_num   = 0;
uint32_t utl_log_check_fail  = 0;
int16_t utl_log_dbglvl = 0;
int16_t utl_log_prdlvl = 0;
const char *utl_log_w = "w";

int utl_log_example_i;

log_watch_t *utl_log_watch = NULL;

int utl_log_close(const char *msg)
{
  int ret = 0;
  
  if (utl_log_check_num) {
    logprintf("CHK #KO: %d (of %d)",utl_log_check_fail,utl_log_check_num);
    utl_log_check_fail = 0;
    utl_log_check_num = 0;
  }
  if (msg) logprintf("%s",msg);
  if (utl_log_file && utl_log_file != stderr) ret = fclose(utl_log_file);
  utl_log_file = NULL;
  return ret;
}

FILE *utl_log_open(const char *fname, const char *mode)
{
  char md[4];
  md[0] = (mode && *mode == 'w')? 'w' : 'a';
  md[1] = '+';
  md[2] = '\0';
  utl_log_close(NULL);
  utl_log_file = fopen(fname,md);
  logprintf("LOG START");
  utl_log_check_num = 0;
  utl_log_check_fail = 0;
  return utl_log_file;
}

int utl_log_time(void)
{
  char       log_tstr[32];
  time_t     log_time;
  struct tm *log_time_tm;
  int        ret = 0;
  
  if (!utl_log_file) utl_log_file = stderr;
  if (time(&log_time) == ((time_t)-1)) ret = -1;
  if (ret >= 0 && !(log_time_tm = localtime(&log_time))) ret = -1;
  if (ret >= 0 && !strftime(log_tstr,32,"%Y-%m-%d %H:%M:%S",log_time_tm)) ret =-1;
  if (ret >= 0) ret = fprintf(utl_log_file,"%s ",log_tstr);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  
  return ret;
}

int utl_log_check(int res, const char *test, const char *file, int32_t line)
{
  int ret = 0;
  
  if (utl_log_dbglvl > UTL_LOG_D) return 1;
  
  ret = utl_log_time();
  
  if (ret >= 0) ret = fprintf(utl_log_file,"CHK %s (%s)?\x09:%s:%d\x09\n", (res?"PASS":"FAIL"), test, file, line);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  if (!res) utl_log_check_fail++;
  utl_log_check_num++;
  return res;
}

void utl_log_assert(int res, const char *test, const char *file, int32_t line)
{
  if (!utl_log_check(res,test,file,line)) {
    logprintf("CHK ASSERTION FAILED");
    logclose();
    abort();
  }
}

void utl_log_watch_check(char *buf, log_watch_t *lwatch, const char *file, int32_t line)
{ 
  int k=0;
  char *p;
  int expected = 1;
  int res = 1;
  pmx_t pmx_state;
  char **watch;
  
  _logprintf("XXX %s (%p)",buf,(void*)lwatch);
  if (!lwatch) return;
  watch = lwatch->watch;
  for (k=0; k<UTL_LOG_WATCH_SIZE; k++) {
    expected = 1;
    p = watch[k];
    _logprintf(">>> %s",p?p:"");
    if (p) {
      if (p[0] == '\1' && p[1] == '\0') break;
      if (p[0] == '!') {p++; expected = (p[0]=='!'); }
     
      _logprintf("?? err:%d exp:%d %s %s [%s]",utl_log_check_fail,expected,(char *)watch[k],p,buf);
      pmxclear(&pmx_state);
    	res = pmxsearch(p,buf) != NULL;
      pmxrestore(&pmx_state);
      if (res) {
        utl_log_check(expected,watch[k],file,line);
        if (expected) watch[k] = NULL;
      }
    }
  }
}

void utl_log_watch_last(log_watch_t *lwatch, const char *file, int32_t line)
{ 
  /* The  only tests in `watch[]` should be the ones with `!` at the beginning */
  int k;
  int expected = 0;
  char *p;
  char **watch;
  
  if (!lwatch) return;
  watch = lwatch->watch;
  for (k=0; k<UTL_LOG_WATCH_SIZE;k++) {
    p = watch[k];
    expected = 0;
    if (p) {
      if (p[0] == '\1' && p[1] == '\0') break;
      if (p[0] != '!') expected = 1;
      utl_log_check(!expected,(char *)(watch[k]),file,line);
    }
  }
}

void utl_log_setlevel(const char *lvl) {
  int l = 0;
  if (lvl) {
    if (*lvl == '*') lvl = "I,T";
    l = 2;
    switch (toupper(*lvl)) {
      case 'N' : l++; 
      case 'E' : l++; 
      case 'W' : l++; 
      case 'I' : utl_log_prdlvl = l; 
                 break;
    }

    while (*lvl && *lvl!=',') lvl++;
    if (*lvl) {
      lvl++;
      l = 0;
      switch (toupper(*lvl)) {
        case 'N' : l+=4; 
        case 'D' : l++; 
        case 'T' : utl_log_dbglvl = l; 
                   break;
      }
    }
  }
}

#endif
#endif
#line 41 "src/utl_mem.c"
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
    logprintf("TRC Invalid or double freed %p (%lu)\x09:%s:%d\x09",(void *)(p->blk),
                                               (unsigned long)utl_mem_allocated, file, line);     
    return memINVALID; 
  }
  if (memcmp(p->blk+p->size,utl_END_CHK,4)) {
    logprintf("TRC Boundary overflow %p [%lu] (%lu)\x09:%s:%d\x09",
                              (void *)(p->blk), (unsigned long)p->size, (unsigned long)utl_mem_allocated, file, line); 
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
  logprintf("TRC Allocated %p [%lu] (%lu)\x09:%s:%d\x09",(void *)(p->blk),(unsigned long)size,(unsigned long)utl_mem_allocated,file,line);
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
                                       (void *)ptr, (unsigned long)p->size, (void *)(p->blk), (unsigned long)size, 
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
  logprintf("TRC strdup %p [%lu] -> %p (%lu)\x09:%s:%d\x09",(void *)ptr, (unsigned long)size, (void *)dest, 
                                                (unsigned long)utl_mem_allocated, file, line);
  return dest;
}
#undef utl_mem

size_t utl_mem_used(void) {return utl_mem_allocated;}


mpl_t utl_mpl_new(void)
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
#line 20 "src/utl_vec.c"
#ifndef UTL_NOVEC

int utl_vec_nullcmp(void *a, void *b, void* aux){return 0;}

static int16_t utl_vec_makeroom(vec_t v,uint32_t n)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;

  if (n < v->max) return 1;
  new_max = v->max;
  while (new_max <= n) {
    new_max += (new_max / 2);  /* (new_max *= 1.5) instead of (new_max *= 2) */
    new_max += (new_max & 1);  /* ensure new size is even */
  }
  new_vec = (uint8_t *)realloc(v->vec, new_max * v->esz);
  if (!new_vec) return 0;
  v->vec = new_vec;
  v->max = new_max;
  return 1;
}

static int16_t utl_vec_makegap(vec_t v, uint32_t i, uint32_t l)
{
 
  if (!utl_vec_makeroom(v,v->cnt + l)) return 0;
  
  /*
  **                    __l__
  **  ABCDEFGH       ABC-----DEFGH
  **  |  |    |      |  |    |    |
  **  0  i    cnt    0  i   i+l   cnt+l
  */
  
  if (i < v->cnt) {
    memmove(v->vec + (i+l)*v->esz,  v->vec + i*v->esz,  (v->cnt-i)*v->esz);
    v->cnt += l;
  }
  return 1;
}

static int16_t utl_vec_delgap(vec_t v, uint32_t i, uint32_t l)
{
  
  /*
  **       __l__                                
  **    ABCdefghIJKLM              ABCIJKLM     
  **    |  |    |    |             |  |    |    
  **    0  i   i+l   cnt           0  i    cnt-l  
  */
  _logdebug("DELGAP: %d %d",i,l);
  if (i < v->cnt) {
    if (i+l >= v->cnt) v->cnt = i; /* Just drop last elements */
    else {
      memmove(v->vec + i*v->esz,  v->vec + (i+l)*v->esz,  (v->cnt-(i+l))*v->esz);
      v->cnt -= l;
    }
  }
  return 1;
}

int16_t utl_vec_del(vec_t v, uint32_t i)
{
  return utl_vec_delgap(v,i,1);
}

int16_t utl_vec_delrange(vec_t v, uint32_t i,  uint32_t j)
{
  if (j<i) return 0;
  return utl_vec_delgap(v,i,j-i+1);  
}

vec_t utl_vec_new(uint16_t esz, utl_cmp_t cmp, utl_hsh_t hsh)
{
  vec_t v = NULL;
  uint32_t sz;
    
  if (hsh) {
    if (!cmp) return NULL;
    esz += (4+(esz & ~0x03)) & 0x03; // ensure is aligned to 4 bytes
    esz += sizeof(uint32_t); // To store the key hash value
  }
  
  sz = sizeof(vec_s)+(esz-4); // 4 is the size of the elm array
  v = malloc(sz);
  if (v) {
    memset(v,0,sz);
    v->esz = esz;
    v->max = vec_MIN_ELEM;
    v->vec = (uint8_t *)malloc(v->max * esz);
    if (!v->vec) { 
      free(v);  v = NULL;
    }
    else {
      v->elm = ((char *)v) + offsetof(vec_s,eld);
      v->cmp = cmp;
      v->hsh = hsh;
      memset(v->vec,0xFF,v->max *esz); // if hashed, hash would be 0xFFFFFFFF
      vecunsorted(v);
      v->fst=0;
      v->lst=vec_MAX_CNT;
      v->cur=0;
      v->cnt=0;
      v->aux=NULL;
    }
  }
  return v;
}

void *utl_vec_aux(vec_t v,void *p)
{
  if (p) v->aux = p;
  return v->aux;
}

vec_t utl_vec_free(vec_t v)
{
  if (v) { 
    if (v->vec) free(v->vec);
    v->vec = NULL;
    free (v);
  }
  return NULL;
}

#define VEC_MAGIC(x) (((uint32_t)'V')<<24 | ((uint32_t)'E')<<16 | ((uint32_t)'C') << 8| (uint32_t)x)

int utl_vec_freeze(FILE *f,vec_t v)
{
  uint32_t sz;
  uint32_t max;
  int ret = 0;
  char *magic = "VEC1";
  
  if (f && v) {
    if (fwrite(magic,4,1,f)) {
      if (fwrite(&(v->esz),sizeof(uint16_t),1,f)) {
        sz = sizeof(vec_s)+(v->esz-4);
        max = v->max;
        // You can't shrink hashtable nor que (TODO!)
        //if (!v->hsh) v->max = v->cnt;
        if (fwrite(v,sz,1,f)) {
          if (fwrite(v->vec,v->esz*v->max,1,f)) {
            v->max = max;
            ret = 1;
          }
        }
        v->max = max;
      }
    }
  }
  
  return ret;
}

vec_t utl_vec_unfreeze(FILE *f, utl_cmp_t cmp, utl_hsh_t hsh)
{
  vec_t v = NULL;
  char magic[8];
  uint16_t esz;
  uint32_t sz;
  
  if (f) {
    if (fread(&magic,4,1,f)) {
      if (strncmp(magic,"VEC1",4) == 0) {
        if (fread(&esz,sizeof(uint16_t),1,f)) {
          sz = sizeof(vec_s)+(esz-4);
          if ((v = malloc(sz))) {
            v->vec = NULL;
            if (fread(v,sz,1,f)) {
              v->vec = malloc(v->esz*v->max);
              if (v->vec) {
                if (fread(v->vec,v->esz*v->max,1,f)) {
                  _logtrace("VEC UNFRZ: %p %p",cmp,hsh);
                  v->elm = ((char *)v) + offsetof(vec_s,eld);
                  v->cmp = cmp;
                  v->hsh = hsh;
                  return v;
                } } } } } } } }
  if (v) {
    if (v->vec) free(v->vec);
    free(v);
  }
  return NULL;
}

int utl_frz(char *fname, vec_t t, int (*fun)(FILE *, vec_t))
{
  int ret = 0;
  FILE *f;
  f = fopen(fname,"wb");
  if (f) {
    ret = fun(f,t);
    fclose(f);
  }
  return ret;
}

vec_t utl_unfrz(char *fname, utl_cmp_t cmp, utl_hsh_t hsh, vec_t (*fun)(FILE *, utl_cmp_t, utl_hsh_t))
{
  sym_t t = NULL;
  FILE *f;
  f = fopen(fname,"rb");
  if (f) {
    t = fun(f, cmp, hsh);
    fclose(f);
  }
  return t;
}

void *utl_vec_get(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  
  if (i == vec_MAX_CNT) i = v->cnt-1;
  if (i < v->cnt) {
    elm = v->vec + (i*v->esz);
    memcpy(v->elm,elm,v->esz);
  }
  return elm;
}

void *utl_vec_alloc(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  
  _logdebug("vecalloc: pos: %d max:%d cnt:%d",i,v->max,v->cnt);
  if (i == vec_MAX_CNT) i = v->cnt;
  if (utl_vec_makeroom(v,i)) {
    elm = v->vec + (i*v->esz);
    if (i >= v->cnt) v->cnt = i+1;
    vecunsorted(v);
  }
  return elm;
}

void *utl_vec_set(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  _logdebug("vecset: %p pos: %d",(void*)v,i);
  elm = utl_vec_alloc(v,i);
  _logdebug("allocated:");
  if (elm) memcpy(elm, v->elm, v->esz);
  return elm;
}

void *utl_vec_ins(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (i == vec_MAX_CNT) i = v->cnt;
  if (utl_vec_makegap(v,i,1))
    elm = (uint8_t *)utl_vec_set(v,i);
  vecunsorted(v);
  return elm;
}

/* * QUEUE */
static int16_t utl_que_makeroom(vec_t v)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;
  int32_t n;

  if (v->lst==vec_MAX_CNT) v->lst=0;
  
  if (v->cnt < v->max) return 1;
  new_max  = v->max;
  new_max += (new_max / 2);  /* (new_max *= 1.5) instead of (new_max *= 2) */
  new_max += (new_max & 1);  /* ensure new size is even */
 
  new_vec = (uint8_t *)realloc(v->vec, new_max * v->esz);
  if (!new_vec) return 0;
 
  if (v->fst == 0) {
    v->lst = v->max;
  }
  else if (v->fst < v->max /2) {
    memcpy(new_vec+(v->max*v->esz), new_vec,v->fst * v->esz);
    v->lst = v->max+v->fst;
  } else {
    n = v->max - v->fst;
    memcpy(new_vec + (new_max-n)*v->esz, new_vec + (v->fst * v->esz), n*v->esz);
    v->fst = new_max - n;
  }
  
  v->vec = new_vec;
  v->max = new_max;
  return 1;
}

void *utl_vec_enq(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (utl_que_makeroom(v)) {
    _logdebug("enque cnt:%d lst:%d",v->cnt,v->lst);
    elm = v->vec + (v->lst*v->esz);
    memcpy(elm, v->elm, v->esz);
    v->cnt++;
    v->lst++;
    if (v->lst >= v->max) v->lst = 0;
    vecunsorted(v);
  }
  return elm;
}

void utl_vec_deq(vec_t v)
{
  if (v->cnt > 0) {
    v->fst++;
    if (v->fst >= v->max) v->fst = 0;
    v->cnt--;
  }
  if (v->cnt == 0) {
    v->fst = 0;
    v->lst = 0;
  }
}

void utl_vec_deq_all(vec_t v)
{
  v->cnt = 0;
  v->fst = 0;
  v->lst = 0;
}

/* * Sorted sets  * */
                          
static inline void utl_dpqswap(void *a, void *b, uint32_t sz)
{
  uint8_t  tmp8;
  uint32_t tmp32;
  uint8_t *pa = ((uint8_t *)a);
  uint8_t *pb = ((uint8_t *)b);
  
  if (a!=b) {
    while (sz >= 4) {
      tmp32 = *(uint32_t *)pa;
      *(uint32_t *)pa = *(uint32_t *)pb;
      *(uint32_t *)pb = tmp32;
      sz-=4; pa+=4; pb+=4;
    }
    switch (sz) {
      case 3: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;
      case 2: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;
      case 1: tmp8=*pa; *pa=*pb; *pb=tmp8; pa--; pb--;
    }
  }
}

static inline int utl_dpqordrange(int32_t a, int32_t b, int32_t c)
{
  if (a>b) {
    if (b>c) return 123;  // a>b>c
    if (c>a) return 312;  // c>a>b
             return 132;  // a>c>b
  } 
  if (a>c) return 213;    // b>a>c
  if (c>b) return 321;    // c>b>a 
           return 231;    // b>c>a
}

static uint32_t utl_rnd_status = 0;
static inline uint32_t utl_dpqrand(void) 
{ 
  if (utl_rnd_status == 0) utl_rnd_status = (uint32_t)time(0);
	//utl_rnd_status = 1664525 * utl_rnd_status + 1013904223; // Linar Congruence
  utl_rnd_status ^= utl_rnd_status << 13;                   // xorshift
	utl_rnd_status ^= utl_rnd_status >> 17;
	utl_rnd_status ^= utl_rnd_status << 5;
	return utl_rnd_status;
}

#define utl_dpqptr(k)    ((uint8_t *)base+(k)*esz)
#define utl_dpqpush(l,r) do {stack[stack_top][0]=(l); stack[stack_top][1]=(r); stack_top++; } while(0)
#define utl_dpqpop(l,r)  do {stack_top--; l=stack[stack_top][0]; r=stack[stack_top][1];} while(0)

void utl_dpqsort(void *base, uint32_t nel, uint32_t esz, utl_cmp_t cmp, void *aux)
{
  int32_t left,right;
  uint8_t *leftptr, *rightptr; 
  uint32_t L,K,G;
  
  int32_t stack[128][2]; // Enough for 2^31 max elements in the array
  int16_t stack_top = 0;
  //uint32_t stack_max = 0;
  
  utl_dpqpush(0,nel-1);
  while (stack_top > 0) {
    //if (stack_top > stack_max) stack_max=stack_top;
    utl_dpqpop(left, right);
    if (left < right) {
      if ((right - left) <= 16) {  // Use insertion sort
        //logtrace("DPQ: Insertion sort [%d - %d]",left,right);
        for (int32_t i = left+1; i<=right; i++) {
          rightptr = utl_dpqptr(i);
          leftptr = rightptr - esz;
          for (int32_t j=i; j>0 && cmp(leftptr, rightptr, aux) > 0; j--) {
            utl_dpqswap(rightptr, leftptr, esz);
            rightptr = leftptr;
            leftptr = rightptr - esz;
          }
        }
      }
      else {
        leftptr = utl_dpqptr(left);
        rightptr = utl_dpqptr(right);
        
        /* Randomize pivot to avoid worst case (already sorted array) */
        L = left + (utl_dpqrand() % (right-left));
        G = left + (utl_dpqrand() % (right-left));
        utl_dpqswap(utl_dpqptr(L),leftptr, esz);
        utl_dpqswap(utl_dpqptr(G),rightptr, esz);
        //logtrace("DPQ: Randomized left:%d right:%d ",L,G);
        if (cmp(leftptr, rightptr, aux) > 0) {
          utl_dpqswap(leftptr, rightptr, esz);
        }
        L=left+1; K=L; G=right-1;
        //logtrace("DPQ: [%d - %d] L:%d K:%d G:%d [START]",left,right,L,K,G);
        while (K <= G) {
          //logtrace("DPQ: [%d - %d] L:%d K:%d G:%d",left,right,L,K,G);
          if (cmp(utl_dpqptr(K), leftptr, aux) < 0) {
            //logtrace("DPQ: [K] < [left]");
            utl_dpqswap(utl_dpqptr(K), utl_dpqptr(L), esz);
            L++;
          }
          else if (cmp(utl_dpqptr(K), rightptr, aux) > 0) {
            //logtrace("DPQ: [K] >= [right]");
            while ((cmp(utl_dpqptr(G), rightptr, aux) > 0) && (K<G)) 
              G--;

            utl_dpqswap(utl_dpqptr(K), utl_dpqptr(G), esz);
            G--;
            if (cmp(utl_dpqptr(K), leftptr, aux) < 0) {
              utl_dpqswap(utl_dpqptr(K), utl_dpqptr(L), esz);
              L++;
            }
          }
          K++;
        }
        L--; G++;

        utl_dpqswap(leftptr,  utl_dpqptr(L), esz);
        utl_dpqswap(rightptr, utl_dpqptr(G), esz);
        /* Push ranges so that the largest will be handled first */
        switch (utl_dpqordrange((right-(G+1)), ((G-1)-(L+1)), ((L-1)-left))) {
          case 123: utl_dpqpush(G+1, right); utl_dpqpush(L+1, G-1);   utl_dpqpush(left, L-1);  break;
          case 132: utl_dpqpush(G+1, right); utl_dpqpush(left, L-1);  utl_dpqpush(L+1, G-1);   break;
          case 213: utl_dpqpush(L+1, G-1);   utl_dpqpush(G+1, right); utl_dpqpush(left, L-1);  break;
          case 231: utl_dpqpush(L+1, G-1);   utl_dpqpush(left, L-1);  utl_dpqpush(G+1, right); break;
          case 312: utl_dpqpush(left, L-1);  utl_dpqpush(G+1, right); utl_dpqpush(L+1, G-1);   break;
          case 321: utl_dpqpush(left, L-1);  utl_dpqpush(L+1, G-1);   utl_dpqpush(G+1, right); break;
        }
      }
    }
  }
  //logtrace("DPQ: maxstack: %d",stack_max);
}

void utl_vec_sort(vec_t v, int (*cmp)(void *, void *, void *))
{
  if (v->hsh) {vecunsorted(v); return;}
  
  if (cmp != utl_vec_nullcmp && cmp != v->cmp) {
    v->cmp = cmp;
    vecunsorted(v);
  }
  if (v->cmp == NULL) {
    vecunsorted(v);
  }
  else if (!vecissorted(v)) {
    if (v->cnt > 1) {
      //qsort(v->vec,v->cnt,v->esz,(int (*)(const void *, const void *))(v->cmp));  
      utl_dpqsort(v->vec,v->cnt,v->esz,v->cmp,v);  
    }
    vecsorted(v);
  }
}

static void *utl_vec_add_sorted(vec_t v)
{
  uint8_t *elm=NULL;
  int32_t mid,lo,hi;
  int ret;
  
  vecsort(v); 
  lo = 0; hi = v->cnt-1;
  while (lo <= hi) {
    mid = lo + (hi-lo)/2;
    elm = v->vec + mid * v->esz;
    ret = v->cmp(v->elm,elm,v);
    if (ret == 0) {
      memcpy(elm, v->elm, v->esz);
      return elm;
    } 
    if (ret < 0) hi = mid-1;
    else         lo = mid+1;
  }
  // Not found. `lo` points to the first element greater than the one to add
  // (or just past the last element)
  //logtrace("add: %d at %d size: %d max:%d",*((int *)v->elm),lo,v->cnt,v->max);
  return utl_vec_ins(v,lo);
}

static void *utl_vec_search_sorted(vec_t v)
{
  uint8_t *elm=NULL;
  int32_t mid,lo,hi;
  int ret;
  
  vecsort(v); 
  lo = 0; hi = v->cnt-1;
  while (lo <= hi) {
    mid = lo + (hi-lo)/2;
    elm = v->vec + mid * v->esz;
    ret = v->cmp(v->elm,elm,v);
    if (ret == 0) return elm;
    if (ret < 0) hi = mid-1;
    else         lo = mid+1;
  }
  return NULL;
}

/* * Unsorted sets (Hash tables) * */

/* 
                                    +------+ 
    The utl_h() macro retrieves     |      | actual element 
    the hash value of the element   |      | 
                                    +------+ 
                                    | hash | hash value
                                    +------+
 */
#define utl_h(p,sz) *((uint32_t *)(((uint8_t *)(p))+(sz)-sizeof(uint32_t)))

static void *utl_remove_hashed(vec_t v,uint32_t pos)
{
  uint32_t nxt;
  uint32_t elm_h;
  uint8_t  *elm;
  uint8_t  *val;
  
  while(1) {
    val = v->vec + pos*v->esz;
    memset(val,0xFF,v->esz);
    
    nxt = (pos+1) & (v->max-1);
    elm = v->vec + nxt*v->esz;
    elm_h = utl_h(elm,v->esz);
    if (elm_h == 0xFFFFFFFF) break; // empty spot
    if ((elm_h & (v->max-1)) == nxt) break; // with delta 0
    memcpy(val, elm, v->esz);
    pos = nxt;
  }
  v->cnt--;
  return elm;
}

static void *utl_search_hashed(vec_t v)
{
  uint8_t *elm = NULL;
  uint32_t pos;
  uint32_t val_h;
  uint32_t elm_h;
  uint32_t max = v->max;
  uint32_t delta;
  
  val_h = v->hsh(v->elm,v) & 0x7FFFFFFF;
  pos = val_h;
  while(1) {
    delta = 0;
    pos &= (max-1);
    elm = v->vec + pos * v->esz;
    elm_h = utl_h(elm,v->esz);
    if (elm_h == 0xFFFFFFFF) return NULL;
    if ((elm_h == val_h) && (v->cmp(v->elm,elm,v) == 0)) return elm;
    if ((elm_h - (utl_h(elm,v->esz) & (max-1))) < delta) return NULL;
    pos++; delta++;
  }
}

static uint8_t *utl_vec_hsh_set(uint8_t *val, uint8_t *tab, uint32_t max, uint16_t esz,  int (*cmp)(void *, void *,void *), void *aux)
{
  uint32_t pos;
  uint32_t prv;
  uint8_t *elm = NULL;
  uint32_t delta;
  uint32_t val_h;
  uint32_t elm_h;
  
  val_h = utl_h(val,esz);
  if (val_h < 0x80000000) {
    delta = 0;
    pos = val_h;
    while (1) {    
      pos &= (max-1);
      //logtrace("pos: %d hash: %08X max: %d",pos,h,max);
      elm = tab + pos * esz;
      elm_h = utl_h(elm,esz);
      if ((elm_h == 0xFFFFFFFF) || ((elm_h == val_h) && (cmp(val,elm,aux) == 0)))  {
        memcpy(elm,val,esz);
        break;
      }
      pos++; delta++;
    } 
    // Robin Hood: swap with previous if delta is lower
    while(delta > 0) {
      prv = (pos+max-1) & (max-1);
      elm = tab + prv * esz;
      // logtrace("RH: (delta: %d pos: %d) ( deltap: %d prev: %d)",delta,pos,(prv - (utl_h(elm,esz) & (max-1))),prv);
      delta--;
      if ((prv - (utl_h(elm,esz) & (max-1))) > delta) break;
      memcpy(tab+pos*esz, tab+prv*esz, esz);
      memcpy(elm, val, esz);
      pos = prv;
    }
  }
  return elm;
}

static uint8_t *utl_reash(uint8_t *tab,uint32_t max, uint16_t esz, int (*cmp)(void *, void *, void *),void *aux)
{
  uint8_t *new_tab;
  uint32_t new_max;
  uint32_t k;
  
  new_max = max * 2;
  new_tab = malloc(new_max * esz);
  if (new_tab) {
    memset(new_tab,0xFF,new_max*esz);
    for (k=0; k<max;k++) {
      //logtrace("reash %d",k);
      utl_vec_hsh_set(tab, new_tab, new_max,esz,cmp,aux);
      tab += esz;
    }
  }
  //logtrace("reash completed");
  return new_tab;
}

static void *utl_vec_add_hashed(vec_t v)
{
  uint8_t *elm;
  uint8_t *tab;
  uint32_t h;
  
  if (((v->cnt * 100) / 80) >= v->max) {  // Load factor max: 80%
    //logtrace("Reash at %d",v->cnt);
    tab = utl_reash(v->vec,v->max,v->esz,v->cmp,v);
    if (tab == NULL) return NULL;
    free(v->vec);
    v->vec = tab;
    v->max *= 2;
  }
  
  h = v->hsh(v->elm,v) & 0x7FFFFFFF;
  elm = v->elm;
  utl_h(elm, v->esz) = h;
  elm = utl_vec_hsh_set(elm,v->vec,v->max,v->esz,v->cmp,v);
  
  if (elm) v->cnt += 1;
  //logtrace("Hash add #%d @%d",v->cnt,(int)(elm?(elm-v->vec)/v->esz:-1));
  return elm;
}

/* * sets * */
void *utl_vec_add(vec_t v, uint32_t i) {
  if (v->hsh) return utl_vec_add_hashed(v);
  if (v->cnt > 0) {
    if (v->cmp) return utl_vec_add_sorted(v);
  }
  return utl_vec_set(v,v->cnt);
}

size_t utl_vec_read(vec_t v,uint32_t i, size_t n,FILE *f)
{
  if (utl_vec_makeroom(v,i+n+1)) {
    n = fread(v->vec + (i*v->esz), v->esz, n, f);
    if (v->cnt < i+n) v->cnt = i+n;
    vecunsorted(v);
    return n;
  }
  return 0;
}

size_t utl_vec_write(vec_t v, uint32_t i, size_t n, FILE *f)
{
  if (i+n> v->max) n = v->max - i;
  return fwrite(v->vec+(i*v->esz),v->esz,n,f);
}

void *utl_vec_search(vec_t v, int x)
{
  uint8_t *elm = NULL;
  
  if (v->cnt == 0) return NULL;
  
  if (v->hsh) {
    elm = utl_search_hashed(v);
  }
  else if (v->cmp) {
    elm = utl_vec_search_sorted(v);
  }
  return elm;
}

int utl_vec_remove(vec_t v, int x)
{
  uint8_t *elm = NULL;
  uint32_t pos;
  
  if (v->cnt == 0) return 0;
  
  elm = utl_vec_search(v,0);
  if (elm) {
    pos = (elm - v->vec)/v->esz;
    if (v->hsh) {
      elm = utl_remove_hashed(v,pos);
    } 
    else {
      if (!utl_vec_del(v,pos)) elm = NULL;
    }
  }
  return (elm != NULL);
}

void *utl_vec_first(vec_t v)
{
  uint8_t  *elm = NULL;

  if (v->cnt == 0) {
    v->cur = vec_MAX_CNT;
    return NULL;
  }
  elm = v->vec;
  v->cur = 0;
  if (v->hsh) {
    // Since there is at lease 1 element, we will
    // eventually exit the loop.
    while (utl_h(elm,v->esz) == 0xFFFFFFFF) {
      _logtrace("   checking %d",v->cur);
      elm += v->esz;
      v->cur++;
    }
  }
  else if (v->fst != vec_MAX_CNT) {
    elm = v->vec + v->fst * v->esz ;
  }
  
  v->cur = (elm - v->vec) / v->esz;
  _logtrace("found (first) %d, %d",v->cur, *((uint32_t *)elm));
  memcpy(v->elm,elm,v->esz);
  return elm;
}

void *utl_vec_last(vec_t v)
{
  uint8_t  *elm = NULL;
  
  if (v->cnt == 0) {
    v->cur = vec_MAX_CNT;
    return NULL;
  }
  elm = v->vec + (v->cnt-1) * v->esz;
  if (v->hsh) {
    elm = v->vec + (v->max-1) * v->esz;
    while (utl_h(elm,v->esz) == 0xFFFFFFFF) {
      elm -= v->esz;
    }
  }
  else if (v->lst != vec_MAX_CNT) {
    elm = v->vec + ((v->lst-1) * v->esz);
  }
  v->cur = (elm - v->vec) / v->esz;
  memcpy(v->elm,elm,v->esz);
  return elm;
}

void *utl_vec_next(vec_t v)
{
  uint32_t max = v->cnt;
  uint8_t *elm = NULL;
  
  _logtrace("Initial cnt:%d cur:%d",v->cnt,v->cur);
  if (v->cnt == 0) v->cur = vec_MAX_CNT;
  if (v->cur == v->lst) v->cur = vec_MAX_CNT;
  if (v->cur == vec_MAX_CNT) return NULL;
  
  v->cur++;
  max = v->max;
  if (v->hsh) {  // Hash table
    while (v->cur < max) {
      _logtrace("   checking %d",v->cur);
      elm = v->vec + v->cur * v->esz;
      if (utl_h(elm,v->esz) != 0xFFFFFFFF) break;
      v->cur++;
    }
  } else if (v->lst != vec_MAX_CNT) { // que
    if (v->cur >= max) v->cur = 0;
    if (v->cur == v->lst) v->cur = max;
  } else { // array
    max = v->cnt;
  }
  
  if (v->cur >= max) return NULL;
  
  if (v->cur != vec_MAX_CNT) {
    elm = v->vec + v->cur * v->esz;
    _logtrace("found (next) %d, %d",v->cur, *((uint32_t *)elm));
    memcpy(v->elm,elm,v->esz);
  }
  return elm;
}

// TODO: Check and fix it
void *utl_vec_prev(vec_t v)
{
  uint8_t *elm;
  
  if (v->cnt == 0) v->cur = vec_MAX_CNT;
  if (v->cur == v->fst) v->cur = vec_MAX_CNT;
  if (v->cur == 0) v->cur = vec_MAX_CNT;
  if (v->cur == vec_MAX_CNT) return NULL;
  v->cur--;
  elm = v->vec + v->cur * v->esz;
  if (v->hsh) {
    while (v->cur > 0 && (utl_h(elm,v->esz) == 0xFFFFFFFF)) {
      v->cur--;
      elm = v->vec + v->cur * v->esz;
    }
    if (utl_h(elm,v->esz) == 0xFFFFFFFF) v->cur = vec_MAX_CNT;
  }
  
  if (v->cur == vec_MAX_CNT) return NULL;
  return elm;
}


/* ** BUF ********************* */

char utl_buf_get(buf_t b, uint32_t n)
{
  char *s = vecgetptr(b,n);
  return s?*s:'\0';
}

size_t utl_buf_read(buf_t b, uint32_t i, uint32_t n, FILE *f)
{
  size_t r ;
  
  if (i == vec_MAX_CNT) i = b->cnt;
  r = vecread(b,i,n,f);
  bufsetc(b,i+n,'\0');
  b->cnt = i+n;
  return r;
}

size_t utl_buf_readall(buf_t b, uint32_t i, FILE *f)
{
  uint32_t n,pos;
  size_t ret;

  pos = ftell(f);
  fseek(f,0,SEEK_END);
  n = ftell(f);
  fseek(f,pos,SEEK_SET);
  ret = bufread(b,i,n-pos,f);
  bufsetc(b,i+(n-pos),'\0');
  b->cnt = i+(n-pos);
  return ret;
}

char *utl_buf_readln(buf_t b, uint32_t n, FILE *f)
{
  int c;
  uint32_t i = n;
  
  if (i == vec_MAX_CNT) i = b->cnt;
 
  if (!b || !f || feof(f)) return NULL;
  while ((c=fgetc(f)) != EOF) {
    if (c == '\r') {
      c = fgetc(f);
      if (c != '\n') ungetc(c,f);
      c = '\n';
    }
    bufsetc(b,i++,c);
    if (c=='\n') break;
  }
  if (i==n) return NULL;
  if (bufgetc(b,i-1) != '\n') bufsetc(b,i++,'\n');
  bufsetc(b,i,'\0');
  b->cnt = i;
  return buf(b)+n;
}

char *utl_buf_sets(buf_t b, uint32_t i, const char *s)
{
  char *r;
  
  if (i == vec_MAX_CNT) i = b->cnt;
    
  r = buf(b)+i;
  while (*s) bufsetc(b,i++,*s++);
  bufsetc(b,i,'\0');
  b->cnt=i;
  return r;
}

char *utl_buf_inss(buf_t b, uint32_t i, const char *s)
{
  uint32_t n;
  char *p;
  if (!s || !b) return NULL;
  n = strlen(s);
  
  if (n == 0 || !utl_vec_makegap(b, i, n)) return NULL;
    
  p = buf(b)+i;
  while (*s) *p++ = *s++;

  bufsetc(b,b->cnt,'\0');  b->cnt--;
  return buf(b)+i;
}

char *utl_buf_addc(buf_t b, char c)
{
  bufsetc(b,b->cnt,c); 
  bufsetc(b,b->cnt,'\0');
  b->cnt--;
  return buf(b) + b->cnt - 1;
}

char *utl_buf_insc(buf_t b, uint32_t i, char c)
{
  if (!utl_vec_makegap(b, i, 1)) return NULL;
    
  buf(b)[i] = c;
  bufsetc(b,b->cnt,'\0');  b->cnt--;
  return buf(b)+i;
}

int16_t utl_buf_del(buf_t b, uint32_t i,  uint32_t j)
{
  int16_t r;
  _logdebug("len:%d",b->cnt);
  r = utl_vec_delgap(b, i, j-i+1);
  _logdebug("len:%d",b->cnt);
  if (r) {
    bufsetc(b,b->cnt,'\0');
    b->cnt--;
  }
  return r;
}

int utl_buf_fmt(buf_t b, uint32_t i, const char *fmt, ...)
{
  va_list args;
  int n;
  if (i == vec_MAX_CNT) i = b->cnt;
  va_start(args, fmt);
  n = vsnprintf(NULL,0,fmt,args);
  va_end(args);
  if (n > 0) {
    bufsetc(b,i+n+1,'\0'); // ensure there's enough room
    va_start(args, fmt);
    n = vsnprintf(buf(b)+i,n+1,fmt,args);
    va_end(args);
    b->cnt = i+n;
  }
  return n;
}

/* *** SYM *** */
/*
       spare string used  ____         
       for search and set     \
                               \
 [sym_t] -aux-> [buf_t] -aux-> str
    \              \
     \              \___ keeps the strings:
      \                  s1\0s2\0s3\0...
       \
        \__ keeps the ID in an hastable
           (based on strings content)

Each string is preceded by the associated info (a 32 bit integer)
   0 1 2 3 4 5 6 7 8
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
  |x|x|x|x|a|b|/|/|x|x|x|x|c|d|e|/|f| ... 
  +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-
   |       |      
   |       id
   |
   data (int32_t)
   (always aligned to 4-bytes)   

*/

static uint32_t utl_sym_hash(void *a, void *c)
{
  sym_t v = c;
  char *a_str = utl_sym_get(v,*((uint32_t *)a));
  return utl_hash_string(a_str);
}

static int utl_sym_cmp(void *a, void *b, void *c)
{
  sym_t v = c;
  char *a_str = utl_sym_get(v,*((uint32_t *)a));
  char *b_str = utl_sym_get(v,*((uint32_t *)b));
  return strcmp(a_str,b_str);
}

sym_t utl_sym_new(void)
{
  sym_t t = NULL;
  buf_t b = NULL;
  
  if ((b = bufnew())) {
    if ((t = vecnew(uint32_t,utl_sym_cmp,utl_sym_hash))) {
      t->aux = b;
      b->aux = NULL;
      return t;
    }
  }
  b=buffree(b);
  return NULL;
}

#define utl_sym_tbl(t)          ((sym_t)(t))
#define utl_sym_buf(t) ((buf_t)(((sym_t)(t))->aux))

sym_t utl_sym_free(sym_t t)
{
  if (t) {
    buffree(utl_sym_buf(t));
    vecfree(utl_sym_tbl(t));
  }
  return NULL; 
}

int utl_sym_freeze(FILE *f, sym_t t)
{
  int ret = 0;
  ret = utl_vec_freeze(f,t);
  _logifdebug {
    logtrace("FREEZE VEC: %p",(void *)(t->vec));
    for (int k=0; k<32; k+=2) {
      logtrace("%2d [%8X] -> %8X",k,((uint32_t *)(t->vec))[k],((uint32_t *)(t->vec))[k+1]);
    }
  }
  if (ret) {
     ret = utl_vec_freeze(f,t->aux);
  }
  return ret;
}

sym_t utl_sym_unfreeze(FILE *f, utl_cmp_t cmp, utl_hsh_t hsh)
{
  sym_t t;
  _logtrace("SYM UNFRZ: %p %p",utl_sym_cmp,utl_sym_hash);
  t = utl_vec_unfreeze(f,utl_sym_cmp,utl_sym_hash);
  _logifdebug {
    logtrace("UNFREEZE VEC: %p",(void *)(t->vec));
    for (int k=0; k<32; k+=2) {
      logtrace("%2d [%8X] -> %8X",k,((uint32_t *)(t->vec))[k],((uint32_t *)(t->vec))[k+1]);
    }
  }
  if (t) {
    t->aux = utl_vec_unfreeze(f,NULL,NULL);
    if (t->aux) {
      utl_sym_buf(t)->aux = NULL;
      return t;
    }
  }
  t=vecfree(t);
  return NULL;
}

static uint32_t utl_sym_store(sym_t t,const char *sym)
{
  uint32_t id;
  uint32_t k;
  buf_t b = utl_sym_buf(t);

  k =  b->cnt;
  id = k + 4;
  bufsets(b,id,sym);
  *((int32_t *)(buf(b)+k)) = 0; // set data to 0
  do { // Ensure the next pointer will be 4-bytes aligned
    buf(b)[b->cnt++] = '\0';
  } while (b->cnt & 0x3);
  return id;
}

uint32_t utl_sym_add(sym_t t, const char *sym, int symis)
{
  uint32_t k = symNULL;
  
  if (!(symis & symNEW))
    k = utl_sym_search(t, sym);
  if (k != symNULL) {
    if (symis & symUNIQ) k = symNULL;
  }
  else {
    k = utl_sym_store(t,sym);
    vecadd(uint32_t,t,k);
  }
  return k;  
}

uint32_t utl_sym_search(sym_t t, const char *sym)
{
  uint32_t k = symNULL;
  uint32_t *p;
  _logtrace("SYM SEARCH: %s %p",sym,t->cmp);
  utl_sym_buf(t)->aux = (void *)sym;
  p=vecsearch(uint32_t,t,symNULL);
  utl_sym_buf(t)->aux = NULL;
  if (p) k = *p;
  return k;  
}

char *utl_sym_get(sym_t t,uint32_t id)
{
  char *s;
  if (id == vec_MAX_CNT) {
    _logtrace("id==MAX %s",(char *)utl_sym_buf(t)->aux);
    return utl_sym_buf(t)->aux;
  }
  if (id >= utl_sym_buf(t)->cnt) return NULL;
  s = buf(utl_sym_buf(t)) + id;
  if (*s == '\0') s = NULL;
  return s;
}

int16_t utl_sym_del(sym_t t, const char *sym)
{
  uint32_t  pos;
  uint32_t  id;
  char     *s;
  uint32_t *p;

  utl_sym_buf(t)->aux = (void *)sym;
  p=vecsearch(uint32_t,t,symNULL);
  utl_sym_buf(t)->aux = NULL;
  if (p) {
    id = *p;
    pos = ((uint8_t *)p - t->vec)/t->esz;
    //logtrace("Removing: %s (%d) @ %d",sym,id,pos);
    if (utl_remove_hashed(t,pos)) {
      s = buf(utl_sym_buf(t))+id;
      ((int32_t *)s)[-1] = 0;
      while (*s) *s++ = '\0';
      return 1;
    }
  }
  return 0;
}

static int32_t *utl_sym_data(sym_t t,uint32_t id)
{
  char *s;
  if (id >= utl_sym_buf(t)->cnt) return NULL;
  s = buf(utl_sym_buf(t)) + id;
  if (*s == '\0') return NULL;
  return ((int32_t *)(s-4));
}

int16_t utl_sym_setdata(sym_t t,uint32_t id, int32_t val)
{
  int32_t *s;
  
  s = utl_sym_data(t,id);
  if (s == NULL) return 0;
  
  *s = val;
  return 1;
}

int32_t utl_sym_getdata(sym_t t,uint32_t id)
{
  int32_t *s;
  s = utl_sym_data(t,id);
  if (s == NULL) return 0;
  return *s;
}

/* *** ARB *** */

#define ARB_NODE(a_,n_)   ((utl_arb_node_t *)(a_->vec))[(n_)-1]

arb_t utl_arb_new(void)
{
  arb_t a;
  a= utl_vec_new(sizeof(utl_arb_node_t),NULL,NULL);
  if (a) { 
    a->flg |= vecARB;
    a->lst  = 0;
  }
  return a;
}

uint32_t utl_arb_cnt(arb_t a)
{
  uint32_t cnt =0;
  if (a) {
    cnt = a->cnt;
    if (a->lst) cnt -= ARB_NODE(a,a->lst).dat;
  }
  return cnt;
}

arb_node_t utl_arb_root(arb_t a) { return a ? a->cur = a->fst : 0; }

#define arbMOVE(fname,dir)   arb_node_t utl_arb_##fname(arb_t a)            \
                             {                                              \
                               arb_node_t n = 0;                            \
                               if (a && a->cur) n = ARB_NODE(a,a->cur).dir; \
                               if (n) a->cur = n;                           \
                               return n;                                    \
                             }
                           
arbMOVE(firstchild,dwn)
arbMOVE(nextsibling,nxt)
arbMOVE(parent,upn)

int32_t utl_arb_getdata(arb_t a)            {return a && a->cur?  ARB_NODE(a,a->cur).dat:0; }
int32_t utl_arb_setdata(arb_t a, int32_t v) {return a && a->cur? (ARB_NODE(a,a->cur).dat=(v),1):0;}

int utl_arb_islast(arb_t a)   {return (a && a->cur && ARB_NODE(a,a->cur).nxt == 0); }
int utl_arb_isleaf(arb_t a)   {return (a && a->cur && ARB_NODE(a,a->cur).dwn == 0); }
int utl_arb_isroot(arb_t a)   {return (a && a->cur && (a->cur == a->fst)); }

arb_node_t utl_arb_current(arb_t a, arb_node_t n)
{
  if (!a) return 0;
  if (n > 0) a->cur = n;
  return a->cur;
}

static utl_arb_node_t *utl_arb_newnode(arb_t a, uint32_t *ret_n)
{
  uint32_t n = 0;
  utl_arb_node_t node, *np = NULL;
  
  if (a) {
    
    if (a->lst > 0) { // take a node from the free list
      n = a->lst;
      a->lst = ARB_NODE(a,a->lst).nxt;
    }
    else {
      node.upn = a->cur;
      node.dwn = 0;
      node.nxt = 0;
      node.dat = 0;
      vecadd(utl_arb_node_t,a,node);
      n = veccount(a);
    }
    np = vecgetptr(a,n-1);
    *ret_n = n;
  }  
  return np;
}

arb_node_t utl_arb_addnode(arb_t a)
{
  uint32_t n = 0;
  utl_arb_node_t *np;
  
  if (a) {
    np = utl_arb_newnode(a,&n);
    if (np) {    
      if (a->fst == 0) { // Just added the root!
        a->fst = n;
        np->upn = 0;
      }
      else { // add as first (leftmost) child
        np->upn = a->cur;
        np->nxt = ARB_NODE(a,a->cur).dwn;
        ARB_NODE(a,a->cur).dwn = n;
      }
      a->cur = n;
    }
  }  
   
  return n;
}

arb_node_t  utl_arb_addsibling(arb_t a)
{
  uint32_t n = 0;
  utl_arb_node_t *np;
  
  if (a && a->fst && a->cur != a->fst) { // Root can't have siblings!!
    np = utl_arb_newnode(a,&n);
    if (np) { // add as next child
      np->upn = ARB_NODE(a,a->cur).upn;
      np->nxt = ARB_NODE(a,a->cur).nxt;
      ARB_NODE(a,a->cur).nxt = n;
      a->cur = n;
    }
  }     
  return n;
}

#if 0
// recursive implementation
int utl_arb_dfsX(arb_t a, arb_fun_t pre, arb_fun_t post)
{
  arb_node_t nxt = 0;
  arb_node_t n;
  int ret = 0;
  
  if (pre) ret = pre(a);
  if (ret == 0) {
    n = a->cur;
    nxt = arbfirstchild(a);
    while (nxt) {
      if ((ret = utl_arb_dfs(a,pre,post))) break;
      a->cur = nxt;
      nxt = arbnextsibling(a);
    }
    a->cur = n;
  }
  if (post) ret = post(a) || ret;
  return ret;   
}
#endif 

#define POST_ACTION 0x80000000

int utl_arb_dfs(arb_t a, arb_fun_t pre, arb_fun_t post)
{
  arb_node_t cur;
  
  int ret = 0; 
  vec_t stk = NULL;
  
  if (!a || a->fst == 0) return 1;

  stk = vecnew(arb_node_t);
  vecalloc(stk,100); // 100 Nodes per level!
  vecdropall(stk);
  vecpush(arb_node_t, stk, a->cur);
  _logdebug("PUSH: %08X",a->cur);
  while (!vecisempty(stk)) {
    cur = vectop(arb_node_t, stk, 0);
    vecdrop(stk);
    _logdebug("POP:  %08X",cur);
    
    if (cur & POST_ACTION) {
      a->cur = cur & (POST_ACTION-1);
      if (post) ret = post(a) || ret;
    }
    else {
      a->cur = cur;
      if (pre && !ret) ret = pre(a);
      
      if (ARB_NODE(a,a->cur).nxt) {
        if (!ret) {
          vecpush(arb_node_t, stk, ARB_NODE(a,a->cur).nxt);
          _logdebug("PUSH: %08X",ARB_NODE(a,a->cur).nxt);
        }
      }
      else {  // last child
        if (a->cur != a->fst) { // and is not root
          vecpush(arb_node_t, stk, ARB_NODE(a,a->cur).upn | POST_ACTION);
          _logdebug("PUSH: %08X",ARB_NODE(a,a->cur).upn | POST_ACTION);
        }
      }
      if (ARB_NODE(a,a->cur).dwn) {
        if (!ret) {
          vecpush(arb_node_t, stk, ARB_NODE(a,a->cur).dwn);
          _logdebug("PUSH: %08X", ARB_NODE(a,a->cur).dwn);
        }
      }
      else {
        if (post) ret = post(a) || ret;
      }
    }
  }  
  
  vecfree(stk);  
  
  return ret;   
}

int utl_arb_bfs(arb_t a, arb_fun_t pre)
{
  vec_t que=NULL;
  arb_node_t nxt;
  arb_node_t cur;
  int ret;
  
  if (!a || a->fst == 0) return 1;
  
  que = vecnew(arb_node_t);
  vecalloc(que,100); // 100 Nodes per level!
  vecdeqall(que);
  
  cur = a->cur;
  vecenq(arb_node_t, que, a->cur);
  while (!vecisempty(que)) {
    a->cur = vecfirst(arb_node_t,que,0);
    _logdebug("First: %d",a->cur);
    _logassert(a->cur);
    vecdeq(que);
    
    ret = pre(a);
    if (ret) break;
    
    nxt = arbfirstchild(a);
    while (nxt) {
      _logdebug("Enque: %d",nxt);
      vecenq(arb_node_t,que,nxt);
      nxt = arbnextsibling(a);
    }    
  }
  que = vecfree(que);
  a->cur = cur;
  return ret;   
}


#define DELETED_NODE 0x80000000

static arb_node_t utl_arb_detach(arb_t a)
{
  arb_node_t node = 0;
  arb_node_t upn;
  arb_node_t nxt;
  arb_node_t prv = 0;
  // Detach the subtree that has a->cur as root

  if (!a || a->fst == 0 || a->cur == a->fst) return 0;
  
  node = a->cur;
  _logdebug("ARBDETACH %d",node);
  upn = ARB_NODE(a,node).upn;
  ARB_NODE(a,node).upn = DELETED_NODE;
  
  a->cur = upn;  

  nxt = arbfirstchild(a);
  
  if (nxt == node) {
    ARB_NODE(a,upn).dwn = ARB_NODE(a,nxt).nxt;
  }
  else {  
    while (nxt && nxt != node) {
      prv = nxt;
      nxt = arbnextsibling(a);
    }
    logassert(nxt != 0);
    ARB_NODE(a,prv).nxt = ARB_NODE(a,nxt).nxt ;
  }
  a->cur = upn;  
  _logdebug("ARBDETACH %d DONE",node);
  return node;
}

int utl_arb_prune(arb_t a)
{
  arb_node_t nxt;
  arb_node_t node;
  
  if (!a || a->fst == 0) return 1;

  if (a->cur == a->fst) { // delete all tree nodes
    a->fst = 0;
    a->lst = 0;
    a->cnt = 0;
    a->cur = 0;
    return 0;
  }
  
  logdebug("ARBDEL %d",a->cur);
  // detach the node from root
  node = utl_arb_detach(a);
  if (node) {
    vec_t que = vecnew(arb_node_t);
    if (!que) return 1;
    
    vecalloc(que,100); // 100 Nodes per level!
    vecdeqall(que);
    
    vecenq(arb_node_t, que, node);
    logdebug("ENQ %d",node);
    while (!vecisempty(que)) {
      node = vecfirst(arb_node_t,que,0);
      logassert(node != 0);
      vecdeq(que);
      logdebug("DEQ %d",node);
      
      nxt = ARB_NODE(a,node).dwn;
      
      while (nxt) {
        logdebug("ENQ %d",nxt);
        vecenq(arb_node_t,que,nxt);
        nxt = ARB_NODE(a,nxt).nxt;
      } 
      
      ARB_NODE(a,node).dat = 1;
      
      if (a->lst) {
        ARB_NODE(a,node).dat += ARB_NODE(a,a->lst).dat;
      }
      
      ARB_NODE(a,node).nxt = a->lst;
      ARB_NODE(a,node).upn = DELETED_NODE;
      a->lst = node;
    }

    que = vecfree(que);
  }
  
  return 0;
}

#endif
#line 19 "src/utl_pmx.c"
#ifndef UTL_NOPMX


pmx_t utl_pmx_ ;


//int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch) = NULL;

#define utl_pmx_set_paterror(t) do {if (!utl_pmx_.error) {utl_pmx_.error = t;}} while (0)

#define utl_pmx_FAIL       goto fail

#define utl_pmx_newcap(t) do {                                       \
                            if (utl_pmx_.capnum < utl_pmx_MAXCAPT) {  \
                              utl_pmx_.capt[utl_pmx_.capnum][0] =      \
                              utl_pmx_.capt[utl_pmx_.capnum][1] = (t); \
                              utl_pmx_.capnum++;                      \
                            }                                        \
                          } while(0)


static void utl_pmx_state_reset(void)
{
  utl_pmx_.stack_ptr = 0;
  utl_pmx_.capnum = 0;
}

static int utl_pmx_state_push(const char *pat, const char *txt, int32_t min_n, int32_t max_n, int16_t inv)
{
  utl_pmx_state_s *state;
  
  if (utl_pmx_.stack_ptr >= utl_pmx_MAXCAPT) return 0;
  
  state = utl_pmx_.stack + utl_pmx_.stack_ptr;
  
  state->pat   = pat;
  state->txt   = txt;
  state->min_n = min_n;
  state->max_n = max_n;
  state->n     = 0;
  state->inv   = inv;
  state->cap   = utl_pmx_.capnum;
  
  utl_pmx_newcap(txt);
  utl_pmx_.stack_ptr++;
  
  return 1;
}

static int utl_pmx_state_pop(void)
{
  if (utl_pmx_.stack_ptr == 0) return 0;
  utl_pmx_.stack_ptr--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top(void)
{
  if (utl_pmx_.stack_ptr == 0) return NULL;
  return utl_pmx_.stack + (utl_pmx_.stack_ptr-1);
}

int utl_pmx_len(uint8_t n) {return (int)(pmxend(n)-pmxstart(n));}

// Returns the length in bytes of the character or 0 if it is '\0'
static int32_t utl_pmx_nextch(const char *t, int32_t *c_ptr)
{
  int32_t len = 0;
  
  if (utl_pmx_.utf8) len = utlnextutf8(t, c_ptr);
  else if ((*c_ptr = (uint8_t)(*t)) != 0) len = 1;
  
  return len;
}

static int32_t utl_pmx_gethex(const char *pat, int32_t *c_ptr)
{
  int32_t ch =0;
  int32_t len =0;
  
  while (*pat && (*pat != '>') && !isxdigit(*pat)) {len++; pat++;}
  while (*pat) {
         if ('0'<= *pat && *pat <= '9') ch = (ch << 4) + (*pat -'0');
    else if ('A'<= *pat && *pat <= 'F') ch = (ch << 4) + (*pat -'A'+10);
    else if ('a'<= *pat && *pat <= 'f') ch = (ch << 4) + (*pat -'a'+10);
    else break;
    len++; pat++;
  }
  *c_ptr = ch;
  return len;  
}

static int utl_pmx_isin(const char *pat, const char *pat_end, int32_t ch, int32_t (*nxt)(const char *, int32_t *))
{
  int32_t c1,c2;
  int32_t len;
  
  while (pat<pat_end) {
    c2=0; c1=0;
    len = nxt(pat, &c1);
    if (ch == c1) return 1;
    pat +=len; 
    if (*pat == '-') {
      len = nxt(++pat, &c2);
      if (c1 <= ch && ch <= c2) return 1;
      pat += len; 
    }
  }
  return 0;
}

#define utl_pmx_isin_chars(p,e,c) utl_pmx_isin(p,e,c,utl_pmx_nextch)
#define utl_pmx_isin_codes(p,e,c) utl_pmx_isin(p,e,c,utl_pmx_gethex)

static int32_t utl_pmx_iscapt(const char *pat, const char *txt)
{
  int32_t len = 0;
  uint8_t capnum = 0; 
  const char *cap;
  
  if ('1' <= *pat && *pat <= '9') {
    capnum = *pat - '0';
    _logdebug("capt: %d %d",capnum,utl_pmx_.capnum);
    if (capnum < utl_pmx_.capnum) {
      cap = pmxstart(capnum);
      while (cap < pmxend(capnum) && *cap && (*cap == *txt)) {
        len++; txt++; cap++;
      }
      if (cap < pmxend(capnum)) len = 0;
    }
  }
  return len;
}

void utl_pmx_extend(int(*ext)(const char *, const char *, int, int32_t))
{
  utl_pmx_.ext = ext;
}

#define UTL_PMX_QUOTED 0
#define UTL_PMX_BRACED 1

static int utl_pmx_get_limits(const char *pat, const char *pat_end, const char *txt, int braced,
                             int32_t *c_beg_ptr, int32_t *c_end_ptr, int32_t *c_esc_ptr)
{
  int32_t c_beg = '(';
  int32_t c_end = ')';
  int32_t c_esc = '\0';
  int32_t ch;
  
  _logdebug("BRACE: [%.*s]",pat_end-pat,pat);
  
  if (pat < pat_end) { /* <B()\> <Q""\>*/
    pat += utl_pmx_nextch(pat,&c_esc);
    if (pat < pat_end) {
      c_beg = c_esc; c_esc = '\0';
      pat += utl_pmx_nextch(pat,&c_end);
    }
    if (pat < pat_end) {
      pat += utl_pmx_nextch(pat,&c_esc);
    }
  }
  else {  /* Just <B> or <Q>, try to infer the braces */
    c_beg = '\0';
    (void)utl_pmx_nextch(txt,&ch);
    if (braced) {
           if (ch == '(')    {c_beg=ch; c_end=')';}
      else if (ch == '[')    {c_beg=ch; c_end=']';}
      else if (ch == '{')    {c_beg=ch; c_end='}';}
      else if (ch == '<')    {c_beg=ch; c_end='>';}
    }
    else { // Quoted string
      c_esc = '\\';
           if (ch == '"')    {c_beg=ch; c_end=ch;}
      else if (ch == '\'')   {c_beg=ch; c_end=ch;}
      else if (ch == '`')    {c_beg=ch; c_end=ch;}
      else if (ch == 0x91)   {c_beg=ch; c_end=0x92;}   /* ANSI single quotes */
      else if (ch == 0x93)   {c_beg=ch; c_end=0x94;}   /* ANSI double quotes */
      else if (ch == 0x2018) {c_beg=ch; c_end=0x2019;} /* Unicode single quotes */
      else if (ch == 0x201C) {c_beg=ch; c_end=0x201D;} /* Unicode double quotes */
    }
    if (c_beg=='\0') { /* Valid both as quoted string or braces */
           if (ch == '\xAB') {c_beg=ch; c_end='\xBB';} /* Unicode and ISO-8859-1 "<<" and ">>" */
      else if (ch == '\x8B') {c_beg=ch; c_end='\x9B';} /* Unicode and ISO-8859-1 "<"  and ">"  */
      else if (ch == 0x2039) {c_beg=ch; c_end=0x203A;} /* Unicode Single pointing Angle Quotation */
      else if (ch == 0x2329) {c_beg=ch; c_end=0x232A;} /* Unicode ANGLE BRACKETS */
      else if (ch == 0x27E8) {c_beg=ch; c_end=0x27E9;} /* Unicode MATHEMATICAL ANGLE BRACKETS */
      else if (ch == 0x27EA) {c_beg=ch; c_end=0x27EB;} /* Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS */
      else return 0;
    }
  }
  _logdebug("open:'%d' close:'%d' esc:'%d'",c_beg,c_end,c_esc);
  
  *c_beg_ptr = c_beg;
  *c_end_ptr = c_end;
  *c_esc_ptr = c_esc;
  return 1;
}

static int utl_pmx_get_delimited(const char *pat, const char *txt,int32_t c_beg, int32_t c_end, int32_t c_esc)
{
  int n;
  const char *s;
  int cnt;
  int32_t ch;
  
  s = txt;
  n = utl_pmx_nextch(s,&ch);
  if (n == 0 || ch != c_beg) return 0;
  cnt = 0;
  do {
    s += n;
    n = utl_pmx_nextch(s,&ch);
    if (ch == '\0') return 0;
    _logdebug("BRACE: '%c' cnt:%d",ch,cnt);
    
         if (ch == c_end) { if (cnt == 0) return (s+n)-txt;  else cnt--; }
    else if (ch == c_beg) { cnt++;                                       }
    else if (ch == c_esc) { s += n; n = utl_pmx_nextch(s,&ch);           }
    
  } while (ch);
  utl_pmx_set_paterror(pat);
  
  return s-txt;
  
}

static int utl_pmx_delimited(const char *pat, const char *pat_end, const char *txt, int braced)
{
  int32_t c_beg; int32_t c_end; int32_t c_esc;
  if (!utl_pmx_get_limits(pat,pat_end,txt, braced, &c_beg, &c_end, &c_esc)) return 0;
  return utl_pmx_get_delimited(pat,txt,c_beg,c_end,c_esc);  
}


static int utl_pmx_class(const char **pat_ptr, const char **txt_ptr)
{
  int inv = 0;
  
  const char *pat = *pat_ptr;
  const char *txt = *txt_ptr;
  const char *pat_end;
  
  int32_t len   = 0;
  int32_t n     = 0;
  int32_t min_n = 0;
  int32_t max_n = 0;
  int32_t ch;
  
  _logdebug("class:[%s][%s]",pat,txt);
                
  pat++;  /* skip the '<' */
  
  // {{ Find the end of the pattern
  pat_end=pat;
  while (*pat_end && *pat_end != '>') pat_end++;
  if (pat_end[1] == '>') pat_end++; /* allow just one '>' at the end of a pattern */
  //}}
  
  // {{ Get how many times the match has to occur.
  //      Examples: <2-4l> matches 2,3 or 4 lower case letters
  //                <-4d>  matches from 0 to 4 decimal digits
  //                <2-s>  matches at 2 spaces or more
  //                <3u>   matches exactly 3 upper case letters
  while ('0' <= *pat && *pat <= '9') 
    min_n = (min_n*10) + (*pat++ - '0');
  
  if (*pat == '-') {
    pat++;
    while ('0'<=*pat && *pat <= '9') 
      max_n = (max_n*10) + (*pat++ - '0'); 
    
    if (max_n == 0) max_n = INT32_MAX;
  }
  if (max_n < min_n) max_n = min_n;
  
  if (max_n == 0) {
    switch (*pat) {
      case '*' : min_n = 0; max_n = INT32_MAX; pat++; break;
      case '+' : min_n = 1; max_n = INT32_MAX; pat++; break;
      case '?' : min_n = 0; max_n = 1;         pat++; break;
      default  : min_n = 1; max_n = 1;                break;
    }
  }
  // }}
  
  if (*pat == '!') {inv = 1; pat++;}
  
  // {{ This is for handling repetition of patterns between parenthesis:
  //    Example: '<*>(\\'|<!='>)'   <--  single quoted string (\' as escaped)
  //              <3>(xa|pt)        <--  "ptptpt" or "ptxaxa" or "xaxapt" or ...
  if (pat[0] == '>' && pat[1] == '(') {
    pat += 2;
    if (!utl_pmx_state_push(pat,txt,min_n,max_n,inv)) 
      utl_pmx_set_paterror(pat);
    *pat_ptr = pat;
    return 1;
  }
  // }}
  
  // {{ Matches a pattern n times
  #define utl_W(tst) while ((len = utl_pmx_nextch(txt,&ch)) && ((!tst) == inv) && (n<max_n)) {n++; txt+=len;}
  switch (*pat) {
    case 'a' : utl_W(( utlisalpha(ch) )); break;
    case 's' : utl_W(( utlisspace(ch) )); break;
    case 'u' : utl_W(( utlisupper(ch) )); break;
    case 'l' : utl_W(( utlislower(ch) )); break;
    case 'd' : utl_W(( utlisdigit(ch) )); break;
    case 'k' : utl_W(( utlisblank(ch) )); break;
    case 'x' : utl_W(( utlisxdigit(ch))); break;
    case 'w' : utl_W(( utlisalnum(ch) )); break;
    case 'c' : utl_W(( utliscntrl(ch) )); break;
    case 'g' : utl_W(( utlisgraph(ch) )); break;
    case 'p' : utl_W(( utlispunct(ch) )); break;
    case 'r' : utl_W(( utlisprint(ch) )); break;

    case 'i' : utl_W((ch < 0x80))               ; break;
    
    case '.' : utl_W((ch !='\0' && ch !='\n'))  ; break;

    case '=' : utl_W(utl_pmx_isin_chars(pat+1,pat_end,ch)) ; break;
    case '#' : utl_W(utl_pmx_isin_codes(pat+1,pat_end,ch)) ; break;
    
    case 'L' : while(*txt && *txt != '\r' && *txt != '\n') txt++;
               if (*txt == '\r' && *txt == '\n') txt++;
               n=1;
               break;
    
    case 'N' : utl_W((txt[0]=='\r' ? (txt[1] == '\n'? (len++) : 1)
                                   : (txt[0] == '\n'?  1 : 0)    )) ; break;

    case 'Q' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_QUOTED))); break;
    case 'B' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_BRACED))); break;
    
    case 'I' : utl_pmx_.csens = 0; n=min_n; break;
    case 'C' : utl_pmx_.csens = 1; n=min_n; break;
                            
    case '$' : if (*txt == '\0') n=min_n; break;
    
    case '>' : utl_pmx_set_paterror(pat); return 0;
  
    case '^' : if (inv) utl_pmx_set_paterror(pat); inv = 0;
               utl_W((len=utl_pmx_iscapt(pat+1,txt)));
               break;
  
    case ':' : if (utl_pmx_.ext)
                 utl_W((len=utl_pmx_.ext(pat+1,txt,len,ch)));
               break;
  
    default  : ; //utl_pmx_set_paterror(pat);
  }
  #undef utl_W
  // }}

  // {{ Advance pattern
  while (*pat_end == '>') pat_end++;
  *pat_ptr=pat_end;
  // }}
  
  /* Not enough matches */
  if (n < min_n) return 0;
  
  // {{ Advance matched text
  *txt_ptr = txt;
  // }}
  
  return 1;
}

static const char *utl_pmx_alt_skip(const char *pat)
{
  int paren=0;
  
  while (*pat) {
    switch (*pat++) {
      case '%': if (*pat) pat++; /* works for utf8 as well */
                break;
                
      case '(': paren++;
                utl_pmx_newcap(NULL);
                break;
                
      case ')': if (paren == 0) return (pat-1);
                paren--;
                break;
                
      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;
    }
  }
  return pat;
}

static const char *utl_pmx_alt(const char *pat, const char **txt_ptr)
{
  int paren=0;
  utl_pmx_state_s *state;
  int inv;
  const char *ret = utl_emptystring;
  
  while (*pat) {
    _logdebug("ALT: %s (%d)",pat,utl_pmx_.stack_ptr);
    switch (*pat++) {
      case '%': if (*pat) pat++; /* works for utf8 as well */
                break;
                
      case '(': paren++;
                utl_pmx_newcap(NULL);
                break;
                
      case ')': if (paren > 0) {
                  paren--;
                  break;
                }
                if (utl_pmx_.stack_ptr < 2) {
                  utl_pmx_set_paterror(pat);
                  break;
                }
                
                // {{ If we are here, we have NOT matched what is in the (...) 
                state = utl_pmx_state_top();
                inv = state->inv;
                ret = pat;
                if (inv) { /* It's ok, we WANTED to fail */
                  *txt_ptr = pmxstart(state->cap);  
                  utl_pmx_state_pop();
                  return ret;
                }
                if (state->n >= state->min_n) {       /* It's ok, we matched enough times */
                  utl_pmx_.capt[state->cap][0] = state->txt;
                  utl_pmx_.capt[state->cap][1] = *txt_ptr;
                  utl_pmx_state_pop();
                  return ret;
                }
                // }}
                break;

      case '<': while (*pat && *pat != '>') pat++;
                while (*pat == '>') pat++;
                break;

      case '|': if (paren == 0) {
                  state = utl_pmx_state_top();
                  *txt_ptr = pmxstart(state->cap); 
                  return pat;
                }
                break;
    }
  }
  return utl_emptystring;
}

static const char *utl_pmx_match(const char *pat, const char *txt)
{
  int32_t len;
  int32_t plen;
  int32_t ch;
  int32_t c1;
  int16_t inv =0;
  utl_pmx_state_s *state;
  
  utl_pmx_state_reset();
  utl_pmx_state_push(pat,txt,1,1,0);
  
  while (*pat) {
    _logdebug("[MATCH] %d [%s] [%s]",pmxcount(),pat,txt);
    c1 = 0; plen = 0; 
    switch (*pat) {
      case '(' : pat++;
                 if (*pat == '|') {inv = 1; pat++;}
                 if (!utl_pmx_state_push(pat,txt,1,1,inv)) 
                   utl_pmx_set_paterror(pat);
                 break;
                 
      case '|' : pat = utl_pmx_alt_skip(pat);
                 break;
      
      case ')' : pat++;
                 _logdebug(")->%d",utl_pmx_.stack_ptr);
                 if (utl_pmx_.stack_ptr < 2) {
                   utl_pmx_set_paterror(pat-1); 
                   break;
                 }
                 
                 /* If we are here, we have matched what is in the (...) */
                 state = utl_pmx_state_top();
                 inv = state->inv;
                 if (inv) { /* we shouldn't have matched it :( */
                   utl_pmx_state_pop();
                   utl_pmx_FAIL;
                 }
                 
                 utl_pmx_.capt[state->cap][1] = txt;  
                 state->n++;
                 _logdebug("match #%d min:%d max:%d",state->n,state->min_n, state->max_n);
                 
                 if (state->n < state->max_n) { 
                   utl_pmx_.capt[state->cap][0] = txt;
                   pat = state->pat; /* try to match once more */
                 }
                 else {
                   utl_pmx_.capt[state->cap][0] = state->txt;  
                   utl_pmx_state_pop();
                 }
                 
                 break;
                 
      case '<' : if (!utl_pmx_class(&pat,&txt)) utl_pmx_FAIL;
                 break;

      case '%' : if (pat[1]) plen = 1+utl_pmx_nextch(pat+1, &c1);

      default  : if (c1 == 0) plen = utl_pmx_nextch(pat, &c1);
                 len = utl_pmx_nextch(txt, &ch);
                 if (!utl_pmx_.csens) {
                   ch = utlfoldchar(ch);
                   c1 = utlfoldchar(c1);
                 }
                 if (ch != c1) {
                   _logdebug("FAIL: %d %d",c1,ch);
                   utl_pmx_FAIL;
                 }
                 txt += len;
                 pat += plen;
                 break;
                 
      fail     : pat = utl_pmx_alt(pat, &txt) ; /* search for an alternative */
                 if (*pat == '\0') utl_pmx_.capnum = 0;
                 break;
    }
  }
  utl_pmx_.capt[0][1] = txt;
  
  for (len = utl_pmx_.capnum; len < utl_pmx_MAXCAPT; len++) {
    utl_pmx_.capt[len][0] = utl_pmx_.capt[len][1] = NULL;
  }
  _logdebug("res: %p - %p",utl_pmx_.capt[0][0],utl_pmx_.capt[0][1]);
  return utl_pmx_.capt[0][0];
}

const char *utl_pmx_search(const char *pat, const char *txt, int fromstart)
{
  const char *ret=NULL;
  
  utl_pmx_.error = NULL;
  utl_pmx_.csens = 1;
  
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_.utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_.utf8=0;}
    
  if (*pat == '^')  ret = utl_pmx_match(pat+1,txt);
  else while (!(ret = utl_pmx_match(pat,txt)) && *txt && !fromstart) {
         txt += utl_pmx_.utf8 ? utlnextutf8(txt) : 1;
       }
  _logdebug("ret: %p",ret);
  return ret;
}

const char *utl_pmx_scan(const char *pat, const char *txt, pmxaction_t func, void *aux)
{
  const char *ret =NULL;  
  utl_pmx_.error = NULL;
  utl_pmx_.csens = 1;
 
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_.utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_.utf8=0;}
      
  while (*txt)  {
     if ((ret = utl_pmx_match(pat,txt))) {
       if (func(utl_pmx_.capt[0][0],utl_pmx_.capt[0][1],aux))
         break;
       txt = utl_pmx_.capt[0][1];
     }
     else {
       txt += utl_pmx_.utf8 ? utlnextutf8(txt) : 1;
     }
  }

  return txt;
}

void utl_pmx_save(pmx_t *p)
{ 
  if (p) memcpy(p,&utl_pmx_,sizeof(pmx_t));
  utl_pmx_state_reset();
}

void utl_pmx_restore(pmx_t *p)
{
  if (p) memcpy(&utl_pmx_,p,sizeof(pmx_t));  
}



#endif
#line 20 "src/utl_peg.c"
#ifndef UTL_NOPEG

int utl_peg_str(const char *pat, const char *str)
{ int k = 0;
  if (!(str && *str)) return -1;
  while (*str && *pat && (*str == *pat)) {str++; pat++;k++;}
  return (*pat == '\0'? k: -1);
}

int utl_peg_oneof(const char *pat, const char *str)
{
  if (!(str && *str)) return -1;
  return (strchr(pat,*str)?1:-1);
}

int utl_peg_eol(const char *str)
{
  int ret;
  switch (*str) {
    case '\0' : ret = 0; break;
    case '\n' : ret = 1; break;
    case '\r' : ret = (str[1] == '\n') ? 2 : 1;    
    default   : ret = -1;
  }
  return ret;
}

int utl_peg_wspace(const char *str)
{
  int ret = 0;
  while (1) {
    switch (*str) {
      case '\b' : case '\a':
      case ' ' : case '\t': case '\xA0' : ret++; str++; break;
      default: return ret;
    }
  }
  return ret;
}

int utl_peg_vspace(const char *str)
{
  int ret = 0;
  while (1) {
    switch (*str) {
      case '\r' : case '\n': case '\v' : case '\f': case '\b' : case '\a':
      case ' ' : case '\t': case '\xA0' : ret++; str++; break;
      default: return ret;
    }
  }
  return ret;
}


static peg_t utl_peg_init(peg_t p, const char *s)
{
  if (p) {
    p->start    = s;
    p->pos      = s;
    p->aux      = NULL;
    p->auxi     = 0;
    p->fail     = 0;
    p->errpos   = s;
    p->errrule  = NULL;
    p->errln    = 1;
    p->errcn    = 1;
    p->errmsg   = NULL;
    if (s == utl_emptystring) {
      p->defer = NULL;
      p->mmz   = NULL;
    }
    if (p->defer)  vecclear(p->defer);
             else  p->defer = vecnew(pegdefer_t);
             
    if (p->mmz)  vecclear(p->mmz);
           else  p->mmz = vecnew(utl_peg_mmz_t *);
  }
  return p;
}

peg_t utl_peg_new()
{
  peg_t p;
  p = malloc(sizeof(struct peg_s));
  if (p) {
    p->defer = NULL;
    utl_peg_init(p,utl_emptystring);
  }
  return p; 
}

peg_t utl_peg_free(peg_t parser)
{
  if (parser) {
    if (parser->defer) parser->defer=vecfree(parser->defer);
    free(parser);
  }
  return NULL;
}

const char *utl_peg_defer(peg_t parser, pegaction_t func,const char *from, const char *to)
{
  pegdefer_t defer;
  
  if (!parser->fail) {
    defer.func = func;
    defer.from = from;
    defer.to   = to;
    vecpush(pegdefer_t, parser->defer, defer);
    //logdebug("Deferd: %p %p %p",(void *)func,(void *)from, (void *)to);
  }
  return NULL;
}

const char *utl_peg_back(peg_t parser,const char *rule_name, const char *pos,int32_t dcnt)
{
  int32_t cnt;
  if (parser->errpos <= pos) {
     parser->errpos  = pos;
     parser->errrule = rule_name;
     parser->errmsg = parser->errmsgtmp;
  }
  parser->pos = pos;
  cnt = veccount(parser->defer);
  if (dcnt < cnt) {
    cnt -= dcnt;
    vecdrop(parser->defer,cnt);
  }
  return NULL;  
}

void utl_peg_repeat(peg_t peg_, const char *pegr_, pegsave_t *peg_save)
{
  peg_save->rpt++;
  if (!PEG_FAIL) {
    peg_save->rlen  = PEG_POS - peg_save->rpos;
    peg_save->rpos  = PEG_POS;
    peg_save->rdcnt = PEG_DCNT;
  }
  if (PEG_FAIL || peg_save->rpt >= peg_save->max || peg_save->rlen == 0) {
      if (PEG_FAIL < 0 || (PEG_FAIL = (peg_save->rpt <= peg_save->min))) {
        PEG_BACK(peg_save->pos,peg_save->dcnt);
      }
      else {
        PEG_DCNT = peg_save->rdcnt;
        PEG_POS = peg_save->rpos;
      }
      peg_save->max = 0;
  }
}

static void utl_peg_memoize(peg_t parser, const char *startpos, int32_t startcnt, 
                                                 const char *rule_name, utl_peg_mmz_t *mmz)
{
  int i;
  pegdefer_t *defer;
  
  if (mmz) {
    if (mmz[0].startpos == NULL && mmz[0].startpos == mmz[1].startpos) {
      // First time
      vecpush(utl_peg_mmz_t *, parser->mmz, mmz); // to be cleaned at the end of parsing
      mmz[0].defer = vecnew(pegdefer_t);
      mmz[1].defer = vecnew(pegdefer_t);
      // logdebug("Added: %p",(void *)mmz);
    }
    
    i = (mmz[0].startpos < mmz[1].startpos)? 0:1;
    mmz[i].fail = parser->fail;
    mmz[i].startpos = startpos;
    mmz[i].endpos = parser->pos;
    vecclear(mmz[i].defer);
    while (startcnt < veccount(parser->defer)) {
      defer = vecgetptr(parser->defer,startcnt);
      vecaddptr(mmz[i].defer, defer);
      startcnt++;
    }
    // logdebug("Stored: %s@%p %d %p %d %d",rule_name,mmz[i].startpos,mmz[i].fail,mmz[i].endpos,0,veccount(mmz[i].defer));
  }
}

static void utl_peg_cleanmmz(peg_t parser)
{
  utl_peg_mmz_t *mmz;
  
  while (veccount(parser->mmz) > 0) {
    mmz = vectop(utl_peg_mmz_t *,parser->mmz,NULL);
    mmz[0].startpos = NULL;
    mmz[1].startpos = NULL;
    mmz[0].defer = vecfree(mmz[0].defer);
    mmz[1].defer = vecfree(mmz[1].defer);
    // logdebug("Cleared: %p",(void *)mmz);
    vecdrop(parser->mmz);
  }
}

static int utl_peg_gotmmz(peg_t parser, const char *startpos, int32_t startcnt, const char *rule_name, utl_peg_mmz_t *mmz)
{
  pegdefer_t *defer;
  
  if (mmz) {
    for (int i=0; i<2; i++) {
      if (mmz[i].startpos == parser->pos) {
        parser->fail = mmz[i].fail;
        parser->pos  = mmz[i].endpos;
        for (int k = 0; k < veccount(mmz[i].defer); k++) {
          defer = vecgetptr(mmz[i].defer,k);
          vecaddptr(parser->defer,defer);
        }
        // logdebug("Restor: %s@%p %d %p %d %d",rule_name,startpos,parser->fail,parser->pos,startcnt,veccount(parser->defer));
        return 1;
      }
    }
  }
  return 0;
}

void utl_peg_ref(peg_t parser, const char *rule_name, pegrule_t rule, utl_peg_mmz_t *mmz)
{
  const char *tmp = parser->pos;
  int32_t cnt = veccount(parser->defer);
  // int i = 0;
  // pegdefer_t *defer;
  
  if (!parser->fail) {
    //logdebug("Start:  %s@%p",rule_name,parser->pos);
    /* check result */
    if (!utl_peg_gotmmz(parser, tmp, cnt, rule_name, mmz)) {
      rule(parser,rule_name);
      if (parser->fail) {
        (void)utl_peg_back(parser,rule_name,tmp,cnt);
      }
      //logdebug("Result: %s@%p %d %p %d %d",rule_name,tmp,parser->fail,parser->pos,cnt,veccount(parser->defer));
      /* store result */
      utl_peg_memoize(parser,tmp,cnt,rule_name,mmz);
    }
  }
}

static int peg_defer_func_NULL(const char *from, const char *to, void *aux)
{ return 1; }

static void utl_peg_seterrln(peg_t parser)
{
  const char *s;
  
  if (*parser->errpos) {
    s = parser->start;
    parser->errln = 1;
    parser->errcn = 1;
    parser->errptr = s;
    while (*s && s < parser->errpos) {
      if (s[0] == '\r' && s[1] == '\n') s++;
      if (*s == '\r' || *s == '\n') {
        parser->errptr=s+1;
        parser->errln++;
        parser->errcn=0;
      }
      s++;
      parser->errcn++;
    }  
  }
}

static void utl_peg_execdeferred(peg_t parser)
{
  pegdefer_t defer;
  pegdefer_t defer_NULL = {peg_defer_func_NULL, NULL, NULL};
  
  //logdebug("Deferred actions: %d",veccount(parser->defer));
  defer = vecfirst(pegdefer_t, parser->defer, defer_NULL);
  while (defer.func != peg_defer_func_NULL) {
    //logdebug("Execd: %p %p %p",(void *)defer.func,(void *)defer.from, (void *)defer.to);
    if (defer.func(defer.from, defer.to, parser->aux))
      break;
    defer = vecnext(pegdefer_t, parser->defer, defer_NULL);
  }
}

int utl_peg_parse(peg_t parser, pegrule_t start_rule, utl_peg_mmz_t *mmzptr, 
                const char *txt,const char *rule_name, void *aux)
{
  if (parser && start_rule && txt) {
    utl_peg_init(parser,txt);
    parser->aux = aux;
    utl_peg_ref(parser, rule_name, start_rule, NULL);

    if (!parser->fail) {
      if (parser->errpos <= parser->pos) {
        parser->errpos  = parser->pos;
        parser->errrule = rule_name;
        parser->errmsg = parser->errmsgtmp;
      }
      utl_peg_execdeferred(parser);
    }
    utl_peg_seterrln(parser);
    utl_peg_cleanmmz(parser);
    return !parser->fail;
  }
  return 0;
}

#endif
