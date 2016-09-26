#line 1 "src/utl_hdr.c"
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
**        /  /_/  //  (__/  /  C utility 
**       (____,__/(_____(__/  Library
** 
 [[[

# utl

This library provides functions that are, hopefully, useful during
the developemnt phase. 

The functions are related to:

 - *Logging* (also for *Unit Testing* and *Debugging*) 
 - *Memory check*
 - *Finite State Machines*
 - *Containers*. 
 - *Pattern matching* over string

The objective is to provide a simple and easy to use solution to
recurring problems. These simple functions can be later replaced
by something better (more flexible, more performant, ...) if needed.

The point is to avoid having to think about trivial things (like
logging) and directly jump into your problem. 
 
 
## How to use utl

 The `dist` directory contains all that is needed to add `utl` to 
your project.
 
### `utl.h` and `utl.c` 

 This is the "traditional" (and preferred) way. Just copy these
two file in your project and:

 - include the `utl.h` header in your source files.
 - compile and link `utl.c` with the other files.

### `utl_single.h`  
 
  Alternatively, you can do without `utl.c` simply by
compiling one of your source files with the 
symbol *`UTL_MAIN`* defined (either by definining it 
before including the header, or via a compiler switch
like "`-DUTL_MAIN`"). A good candidate is the
file where your `main()` function is.

## The *selector* symbols

You may want to exclude entirely a portion of the library because
it is of no interest for your project. Leaving it would only 
pollute your namespace (and could possibly stop you using
`utl.h` altogether).

 - `NDEBUG` will cause:
   - `logdebug()` and `logassert()` to perform no action;
   - `logcheck()` to perform no action and always return `1`;
   - Memory checking functions will be disabled (even if `UTL_MEMCHECK`
     is defined).

 - `UTL_NOLOG` will make the logging functions `logxxx` unavailble.
 
 - `UTL_NOFSM` will make the finite state machines `fsmxxx` constructors unavailble.
 
 - `UTL_NOPMX` will make the pattern matching functions `pmxyyy` unavailble.
 
 - `UTL_NOVEC` will make the containers functions (`vecxxx`, `stkxxx`, `bufxxx`, ...) unavailble.

 - `UTL_MEMCHECK` will instrument the dynamic memory functions (`malloc()`, `free()`, ...) 
   to log their activities and perform additional checks. It also enables logging
   even if `UTL_NOLOG` is defined.

** ]]]

*/
#include "utl.h"
#define UTL_MAIN

//<<<//
char *utl_emptystring = "";

int   utl_ret(int x)      {return x;}
void *utl_retptr(void *x) {return x;}
//>>>//

#line 1 "src/utl_log.c"
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


[[[
# Logging

         __
        /  )
       /  /______   ______
      /  //  __  \ /  __  \
     /  //  /_/  //  /_/  /
    (__/ \______/ \___   /
                   __/  /
                  (____/


Logging functions are just wrappers around the `fprintf()` function.
They print a message on a file (by default `stderr`) adding a timestamp
before the message and a newline `\n` at the end.
To make things simple, there is only one level of logging and only one log file
at the time.

### Log messages

  - `int logprintf(char *format, ...);`
     is the main function. On executing:  
     
     ```C  
       logprintf("Warp speed to %d.",count);
       logprintf("Moving on now.")
     ```
     
     the following lines will be printed (assuming `count` value is 4):
     
     ```
       2016-09-03 11:33:01 Warp speed to 4.
       2016-09-03 11:33:01 Moving on now. 
     ```
     On success it will return the number of characters written; on error
     it will return -1. 
     
  - `FILE *logopen(char *fname, char *mode);`
    sets the logging file (i.e. the 
    file the messages will be printed to). The `mode` parameters can be `"w"`,
    to start a new file, or `"a"` to append to an existing file. If anything
    else is passed as parameter, it will default to `"a"`.
    On success, the function returns the pointer to the log file. On failure
    it will return NULL and the log file will be reverted to `stderr`.
    To signal that the log file has been opened, the message `"LOG START"` is
    printed.
    
  - `void logclose(void);`
    Closes the current log files and revert back to `stderr`. Before closing it,
    the message `"LOG STOP"` is printed. Together with the `"LOG START"` message
    printed on opening the file, this is an easy way to determine how much time
    passed between 

### Testing

A simple way to create unit test (or applying
[Test Driven Development](https://en.wikipedia.org/wiki/Test-driven_development))
is to use the following functions:

  - `int logcheck(int test);`
    evaluates the test expression (whose result must be non-zero if the test passed
    and zero if it failed) and writes the result on the log file.

  - `void logassert(int test);`
    same as logcheck() but exits (with `abort()`)on failure. Should be used when
    the test could reveal that something went horribly wrong.

For eaxmple, the following code:

   ```C
     x = get_number_from(outer_space);
     y = get_number_from(inner_space);
     logcheck(x > 100.);
     logassert(y > 0.);
     logcheck(x/y > 0.33);
   ```
will print, assuming `x=35.2` and `y=3.0`:

   ```
     2016-09-03 11:33:01 CHK FAIL (x > 100.) spacing.c 34
     2016-09-03 11:33:01 CHK PASS (y > 0.) spacing.c 35
     2016-09-03 11:33:01 CHK PASS (x/y > 0.33) spacing.c 36
   ```

If, instead, we had `x=145.7` and `y=0.0`, we would have had:

   ```
     2016-09-03 11:33:01 CHK PASS (x > 100.) spacing.c 34
     2016-09-03 11:33:01 CHK FAIL (y > 0.) spacing.c 35
     2016-09-03 11:33:01 CHK EXITING ON FAIL
   ```
    
and the program would have been terminated with exit code 1 to  prevent the
division by zero to happen on the next line.

The fact that `logcheck()` returns the result of the test can be used to get more
information in case of a failure:

   ```C
     y = get_number_from(inner_space);
     if (!logcheck(x > 100.)) {
       logprintf("          x: %.1f",x);
     }
     logassert(y != 0.);
     logcheck(x/y > 0.33);
   ```

   will result in:
    
   ```
      2016-09-03 11:33:01 CHK FAIL (x > 100.) spacing.c 34
      2016-09-03 11:33:01           x: 35.2
   ```

   At the end of the log, the function `logclose()` will print the number of
failures and the number of checks performed.

   ```
      2016-09-18 20:47:04 CHK #KO: 2 (of 23)
   ```
   
### Debugging

While using a symbolic debugger like `gdb` is the *"right thing to do"*, there
are times when you need some more traces of the execution of your program to really
get what is going on (and catch that damned bug!). Insted of using `printf()` or
`logprintf()` (which you can always do, of course) you can use the 

  - `int logdebug(char *format, ...);`
  
function that behaves exactly as the `logprintf()` function but will do nothing if
the `NDEBUG` symbol is defined.

This way you can easily differentiate between normal log messages and messages that
are there just for debugging purposes.

### Temporarily disable logging

There are times when you don't want some log message to be generated or some check
to be performed. This is esepcially true for debugging messages that tend to fill
up the log file with a lot of information you may no longer need.

You could delete the call to the logging function altogether but you might need
them again afterward and it would be a waste to have to write them again.

For cases like this, the following functions are defined:

   - `int   _logprintf(char *format, ...);`
   - `int   _logdebug(char *format, ...);`
   - `int   _logcheck(int test);`
   - `void  _logassert(int test);`
   - `FILE *_logopen(char *fname, char *mode);`
   - `void  _logclose(void);`

that do nothing (`_logcheck()` will always return 1 as if the test passed).

Technically speaking I'm in violation of the C standard here (section 7.1.3):
identifiers starting with underscore are reserved for use as identifiers
with file scope.  I've tried many times to get rid of the initial underscore
(e.g. by using `X` or `X_`) but i still believe this is the best choice and
the worst that could happen is that they clash with some other library.
By the way, this is a risk that still must be taken into consideration for
any other identifier, so I'm not feeling particularly pressed on changing it.
  
** ]]] */

//<<<//
#ifndef UTL_NOLOG
#ifdef UTL_MAIN

static FILE *utl_log_file = NULL;
static uint32_t utl_log_check_num   = 0;
static uint32_t utl_log_check_fail  = 0;

int utl_log_close(char *msg)
{
  int ret = 0;
  
  if (utl_log_check_num) {
    logprintf("CHK #KO: %d (of %d)",utl_log_check_fail,utl_log_check_num);
    utl_log_check_fail = 0;
    utl_log_check_num = 0;
  }
  if (msg) logprintf(msg);
  if (utl_log_file && utl_log_file != stderr) ret = fclose(utl_log_file);
  utl_log_file = NULL;
  return ret;
}

FILE *utl_log_open(char *fname, char *mode)
{
  char md[2];
  md[0] = (mode && *mode == 'w')? 'w' : 'a';
  md[1] = '\0';
  utl_log_close(NULL);
  utl_log_file = fopen(fname,md);
  logprintf("LOG START");
  utl_log_check_num = 0;
  utl_log_check_fail = 0;
  return utl_log_file;
}

int utl_log_printf(char *format, ...)
{
  va_list    args;
  char       log_tstr[32];
  time_t     log_time;
  int        ret = 0;
  struct tm *log_time_tm;
  
  if (!utl_log_file) utl_log_file = stderr;
  if (time(&log_time) < 0) ret = -1;
  if (ret >= 0 && !(log_time_tm = localtime(&log_time))) ret = -1;
  if (ret >= 0 && !strftime(log_tstr,32,"%Y-%m-%d %H:%M:%S",log_time_tm)) ret =-1;
  if (ret >= 0) ret = fprintf(utl_log_file,"%s ",log_tstr);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;

  va_start(args, format);
  if (ret >= 0) ret = vfprintf(utl_log_file, format, args);
  va_end(args);

  if (ret >= 0 && (fputc('\n',utl_log_file) == EOF)) ret = -1;
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  return ret;
}

int utl_log_check(int res, char *test, char *file, int32_t line)
{
  logprintf("CHK %s (%s) %s:%d", (res?"PASS":"FAIL"), test, file, line);
  if (!res) utl_log_check_fail++;
  utl_log_check_num++;
  return res;
}

void utl_log_assert(int res, char *test, char *file, int32_t line)
{
  if (!utl_log_check(res,test,file,line)) {
    logprintf("CHK EXITING ON FAIL");
    logclose();
    abort();
  }
}

#endif
#endif
//>>>//
#line 1 "src/utl_mem.c"
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
**        /  /_/  //  (__/  /  C utility 
**       (____,__/(_____(__/  Library
**

**      ______   ______  ______ 
**     /      \ / ___  )/      \
**    /  / /  //   ___//  / /  /
**   (__/_/__/ \_____/(__/_/__/ 
**   
**   
*/

#define UTL_MEM
#include "utl.h"

//<<<//
#ifndef UTL_NOMEM
#ifdef UTL_MAIN

#ifndef memINVALID
#define memINVALID    -2
#define memOVERFLOW   -1
#define memVALID       0
#define memNULL        1
#endif

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

size_t utl_mem_used(void) {return utl_mem_allocated;}

#endif
#endif
//>>>//
#line 1 "src/utl_vec.c"
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


#include "utl.h"

//<<<//
#ifndef UTL_NOVEC
#ifdef UTL_MAIN

static int16_t utl_vec_makeroom(vec_t v,uint32_t n)
{
  uint32_t new_max = 1;
  uint8_t *new_vec = NULL;

  if (n < v->max) return 1;
  new_max = v->max;
  while (new_max <= n) new_max += (new_max / 2);  /*  (new_max *= 1.5) instead of (new_max *= 2) */
  new_vec = realloc(v->vec, new_max * v->esz);
  if (!new_vec) return 0;
  v->vec = new_vec;  v->max = new_max;
  return 1;
}

static int16_t utl_vec_makegap(vec_t v, uint32_t i, uint32_t l)
{
 
  if (!utl_vec_makeroom(v,i+l+1)) return 0;
  
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


vec_t utl_vec_new(uint16_t esz)
{
  vec_t v = NULL;
  uint32_t sz = sizeof(vec_s)+(esz-sizeof(uint32_t));
  
  v = malloc(sz);
  if (v) {
    memset(v,0,sz);
    v->esz = esz;
    v->max = vec_MIN_ELEM;
    v->vec = malloc(v->max * esz);
    vecunsorted(v);
    if (!v->vec) { free(v); v = NULL;}
  }
  return v;
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

void *utl_vec_get(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;
  
  if (i < v->cnt) {
    elm = v->vec + (i*v->esz);
    memcpy(&(v->elm),elm,v->esz);
  }
  return elm;
}

void *utl_vec_set(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (utl_vec_makeroom(v,i)) {
    elm = v->vec + (i*v->esz);
    memcpy(elm, &(v->elm), v->esz);
    if (i>=v->cnt) v->cnt = i+1;
    vecunsorted(v);
  }
  return elm;
}

void *utl_vec_ins(vec_t v, uint32_t i)
{
  uint8_t *elm=NULL;

  if (i == UINT32_MAX) i = v->cnt;
  if (utl_vec_makegap(v,i,1)) elm = utl_vec_set(v,i);
  vecunsorted(v);
  return elm;
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

size_t utl_vec_read(vec_t v,uint32_t i, size_t n,FILE *f)
{
  if (utl_vec_makeroom(v,i+n+1)) {
    n = fread(v->vec + (i*v->esz), v->esz, n, f);
    if (v->cnt < i+n) v->cnt = i+n;
    vecunsorted(v);
  }
  return 0;
}

size_t utl_vec_write(vec_t v, uint32_t i, size_t n, FILE *f)
{
  if (i+n> v->max) n = v->max - i;
  return fwrite(v->vec+(i*v->esz),v->esz,n,f);
}

void utl_vec_sort(vec_t v, int (*cmp)(void *, void *))
{
  if (vecissorted(v)) return;
  if (cmp) v->cmp = cmp;
  if (v->cmp) {
    if (v->cnt > 1) qsort(v->vec,v->cnt,v->esz,(int (*)(const void *, const void *))(v->cmp));  
    vecsorted(v);
  }
}

void *utl_vec_search(vec_t v)
{
  if (v->cmp) {
    utl_vec_sort(v,NULL);
    return bsearch(&(v->elm),v->vec,v->cnt,v->esz,(int (*)(const void *, const void *))(v->cmp));
  }
  return NULL;
}

char utl_buf_get(buf_t b, uint32_t n)
{
  char *s = vecget(char,b,n);
  return s?*s:'\0';
}

size_t utl_buf_read(buf_t b, uint32_t i, uint32_t n, FILE *f)
{
  size_t r = vecread(b,i,n,f);
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

char *utl_buf_readln(buf_t b, uint32_t i, FILE *f)
{
  int c;
  uint32_t n = i;
  
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

char *utl_buf_sets(buf_t b, uint32_t i, char *s)
{
  char *r = buf(b)+i;
  while (*s) bufsetc(b,i++,*s++);
  bufsetc(b,i,'\0');
  b->cnt=i;
  return r;
}

char *utl_buf_inss(buf_t b, uint32_t i, char *s)
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

#endif
#endif
//>>>//
#line 1 "src/utl_pmx.c"
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

** [[[

# PMX
         ______   ______ ___  ___ 
        /  __  \ /      \\  \/  /  
       /  /_/  //  / /  / \    \  
      /  _____//__/_/__/ /__/\__\ 
     /  /
    (__/

## Pattern matching

  PMX is a library for strings pattern matching. It uses a set of patterns
that is different from the more commonly used regular expressions.

  One difference with RE is that PMX expressions are never ambigous. This
allows a faster matching algorithm at the expense of some expressive power: 
there are regular expressions that can't be matched in pmx but, at the same
time, there are pmx expressions that can't be matched with regular expressions.

  A key difference with Regular Expressions, is that PMX patterns are 
always *greedy* and match as much of the string as they can. It is important
to remind this when you get unexpected results in your match.

  Since RE are very well known, I'll refer sometimes to them to explain
how pmx pattern match.  

  The syntax of pmx has been intentionally chosen to be different from RE
to avoid any confusion. 

## API

 These are the available functions:

 - `char *pmxsearch(char *pat,char *txt)`
        Search the pattern `pat` in the text `txt`. Returns a pointer to the
        first matched text or NULL;
        
 - `char *pmxstart(int n)`
        Start of the captured text for sub expression `n` (0 for the entire match).
        
 - `char *pmxend(int n)`
        End of the captured text for sub expression `n` (0 for the entire match).

 - `size_t pmxlen(int n)`
        Length of the captured text for sub expression `n` (0 for the entire match).
        
 - `int pmxcount()` How many sub-expressions have been matched
 
 - `char *pmxerror()`
        Pointer to first error in the pattern (or to an empty string if no error)
        
 - `void pmxextend(int(*ext)(char *, char *,int, int32_t))`
        Set the function `ext` to match the `<:>` recognizer.

## Patterns

  Every character is a pattern that matches itself with the following
exceptions:

  - `<` Starts a pmx recognizer.
  - `>` Ends a pmx recognizer.
  - `(` Starts a sub-expression (capture).
  - `)` Ends a sub-expression (capture).
  - `|` Separates alternatives.
  - `%` Escapes next character (`%(` is the character `(` itself)

## Alternatives

  To match one of alternative patterns use the '|' character. For example:
  
  - `a(b|c)|d` matches `ab` `ac` or `d` 
   
## Recognizer modifiers
   
  - `!`      negates the recognizer: `<!d>` is any character that is not a digit
  - `*`      matches 0 or more times 
  - `+`      matches 1 or more times 
  - `?`      matches 0 or 1 times 
  - *n*      matches exactly *n* times
  - *n*-     matches at least *n* times
  - -*m*     matches at most *m* times
  - *n*-*m*  matches from *n* to *m* times
  
  Modifiers can be applied to sub-expressions:
  
  - `<3>(ab|xy)` matches "`abxyab`" or "`xyxyxy`" or "`xyabab`" etc..
  
  there must be no space between `>` and `(`.
  
  Note that the meaning of `!` depends on the usage:
  - for characters class, it will match a character that is not in that class
  - for a sub-expressions, the match will be empty
  
  For example:
  - `a<!d><!d>`          will match any 3 character string starting 'a' follewed by two non-digit character
  - `a<!>(bc|de)<2l>` will match any 3 lower case letter string starting with `a` except `abc` and `ade`
  
  In the first example, `<!d>` consumes one character in the text, in the second example, `<!>(bc|de)` does
not consume any text.
  
## Class recognizers

  These patterns match classes of strings that are commonly used.

  These will use the corresponding `C` functions.
 
  - `a`  (isalpha)	matches an alphabetic character
  - `s`  (isspace)	matches a space character
  - `u`  (isupper)	matches an uppercase character
  - `l`  (islower)	matches a lowercase character
  - `d`  (isdigit)	matches a digit.
  - `x`  (isxdigit) matches an hexadecimal digit.
  - `w`  (isalnum)	matches an alphanumeric character
  - `c`  (iscntrl)	matches a control character
  - `g`  (isgraph)	matches a graphic character, excluding the space character.
  - `p`  (ispunct)	matches a punctuation character
  - `r`  (isprint)	matches a printable character, including the space character.
  - `i`  (isascii)  matches a character whose code is between 0 and 127

  These are defined within `pmx`:

  - `h`  (hspace)   matches an horizontal space (` `, `\t` and `nbsp`)
  - `.`  (any)      matches any character except `\0` and `\n` .
  - `$`  (eot)      matches the end of the text
  - `N`  (eol)      matches the end of the line that can either be `\n` or `\r\n`
  - `Q`  (quoted)   matches a quoted string with `\` as escape character
  - `B`  (braced)   matches a text enclosed in (balanced) braces.

  The `<Q>` and `<B>` patterns allow for the definition of the delimiters and the
escape character. If used by themselves they will try to "guess" from the text the
delimiters.

  For `<B>`:
  - `( )`               Parenthesis
  - `[ ]`               Square brackets
  - `{ }`               Curly braces
  - `< >`               Angular braces
  - `« »`               French Guillemot
  - `&#2329; &#232A;`   Unicode ANGLE BRACKETS 
  - `&#27E8; &#27E9;`   Unicode MATHEMATICAL ANGLE BRACKETS 
  - `&#27EA; &#27EB;`   Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS 

  For `<Q>`
  - `" "`               Double quotes
  - `' '`               Single quotes
  - `` ` ` ``           Back quotes
  - `« »`               French Guillemot
  - `&#2018; &#2019;    Unicode single quotes 
  - `&#201C; &#201D;    Unicode double quotes 
  
  Examples:
  
  - `<3d>`         matches 3 digits
  - `<3!d>`        matches 3 non-digits
  - `<+>(ab|cd)`   matches strings like: "abab" "abcdab" "cdabcdcd" etc
  - `<-3=xyz>`     matches "xy" "xx" "zyx" "xxx" "z" ""
  - `0<d>`         matches two digits numbers that start with `0`: `04`, `09`, `00`
  - `<B>L`         matches strings like `(bcd)L`, `{b{cd}}L`, `<<<bcd>ab>x>L`
  - `<Q>`          matches strings like `"bcd"`, `"BA\"XY"`, `'art'`
  - `<utf><Q«»@>`  matches strings like `«bcd»`, `«BA@»XY»`, `«art»`

## User defined character classes

  It is possible to match a character that is in in a given set as follows:

 - `<=...>`  characters list
 - `<#...>`  characters codes (in hex)
  
 Example:
 
 - `<=A-Ga-g>`     matches one of the following characters: `ABCDEFGabcdefg`
 - `<!=xyz>`       matches anything except the characters `x`, `y` and `z`
 - `<#0370-03FF>`  matches Greek and Coptic characters
 - `<=-+>`         matches `+` or `-` (must be the first character in the set)
 - `<=])}>>`       matches a closing bracket including `>` (must be the last one)

  
## Extending recognizers
  
  Sometimes you might want to recognize a pattern that is better described with
a C function. You can do so by defining a function with the following prototype:

  ```C
  int mypatterns(char *pat,char *txt, int len, int32_t ch)
  ```
and enabling it by executing `pmxextend(mypatterns)`. The arguments are:
  
   - `pat` a pointer at the pattern, right after `:`
   - `txt` a pointer to the text to match
   - `len` the length of the representation of the first character in `txt`
   - `ch`  the Unicode codepoint of the first character
   
  If the pattern matches, the function must return the length of the consumed text
in `txt`. If it doesn't match it must return 0. attern is to be matched: 
 
  The function wil be invoked any time the recognizer `<:>` is to be matched.
  
  This short example should (hopefully) clarify better how it works.
  
  ```C
    
  ```
  
  
  
  
  ```
                  len |--|  
                       __ ch 
    "...<:X>..."  "....xx."
          ^pat         ^txt
  ```
  
  
  
    
    
## Text encoding
  
  By default, pmx assumes that strings are encoded with some 8-bit scheme (e.g. ISO-8859-x)
but it can also handle UTF-8 encoded strings.  To specify which encoding is expected, put
at the beginning of the pattern one of the following:

  - `<iso>` consider 8 bit per character (eg. ISO-8859-1)
  - `<utf>` Assume UTF-8 encoding
  
  The encoding is saved across multiple pmxsearch() calls:
  
  ```C
   pmxsearch("<utf>","");          // Just to enable UTF-8
   pmxsearch("<=àèìòù>",txt);      // Text will be considered to be UTF8 encoded
   pmxsearch("<iso><=aeiou>",txt); // Text will be considered to be ISO-8859-x encoded
   pmxsearch("<=bcdef>",txt);      // Text will be considered to be ISO-8859-x encoded
  ```

  Sometimes the encoding seems not to be relevant, but it is important to specify it
the encoding to avoid unexpected results. Assuming the text is in UTF-8:
  
  ```C
   // These will give the same results (three bytes):
   pmxsearch("<utf>àx","uàx");  
   pmxsearch("<iso>àx","uàx");  
     
   // These will give two different match:
   pmxsearch("<utf><.>x","uàx");  // will match three bytes
   pmxsearch("<iso><.>x","uàx");  // will match two bytes
  ```

## Back references

  - `<^n>`' will match the text captured by the sub-expression `n`

  Examples:
  - `(<l>)<^1>' matches a couple of identical lower case characters.

## Recognizers
                
  - `<D>`  A possibly signed integer: `32`, `-15`, `+7`
  - `<F>`  A possibly signed decimal number (including integers): `3.2`, `-.5`, `+7`
  - `<X>`  An hexadecimal number: `F32A`, `2012`, `4aeF`
  - `<L>`  The rest of the line up to the newline character(s) (see below)

*** ]]]

*/

#include "utl.h"

//<<<//
#ifndef UTL_NOPMX
#ifdef UTL_MAIN

int(*utl_pmx_ext)(char *pat, char *txt, int, int32_t ch) = NULL;

char     *utl_pmx_capt[utl_pmx_MAXCAPT][2] = {{0}} ;
uint8_t   utl_pmx_capnum                   =   0   ;
char     *utl_pmx_error                    = NULL  ;


#define utl_pmx_set_paterror(t) do {if (!utl_pmx_error) {utl_pmx_error = t;}} while (0)

static int utl_pmx_utf8 = 0;

#define utl_pmx_FAIL       goto fail

#define utl_pmx_newcap(t) do {                                       \
                            if (utl_pmx_capnum < utl_pmx_MAXCAPT) {  \
                              utl_pmx_capt[utl_pmx_capnum][0] =      \
                              utl_pmx_capt[utl_pmx_capnum][1] = (t); \
                              utl_pmx_capnum++;                      \
                            }                                        \
                          } while(0)

typedef struct {
  char *pat;
  char *txt;
  int32_t min_n;
  int32_t max_n;
  int32_t n;
  int16_t inv;
  int16_t cap;
} utl_pmx_state_s;

utl_pmx_state_s utl_pmx_stack[utl_pmx_MAXCAPT];
uint8_t utl_pmx_stack_ptr = 0;

static void utl_pmx_state_reset()
{
  utl_pmx_stack_ptr = 0;
  utl_pmx_capnum = 0;
}

static int utl_pmx_state_push(char *pat, char *txt, int32_t min_n, int32_t max_n, int16_t inv)
{
  utl_pmx_state_s *state;
  
  if (utl_pmx_stack_ptr >= utl_pmx_MAXCAPT) return 0;
  
  state = utl_pmx_stack + utl_pmx_stack_ptr;
  
  state->pat   = pat;
  state->txt   = txt;
  state->min_n = min_n;
  state->max_n = max_n;
  state->n     = 0;
  state->inv   = inv;
  state->cap   = utl_pmx_capnum;
  
  utl_pmx_newcap(txt);
  utl_pmx_stack_ptr++;
  
  return 1;
}

static int utl_pmx_state_pop()
{
  if (utl_pmx_stack_ptr == 0) return 0;
  utl_pmx_stack_ptr--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top()
{
  if (utl_pmx_stack_ptr == 0) return NULL;
  return utl_pmx_stack + (utl_pmx_stack_ptr-1);
}

size_t utl_pmx_len(uint8_t n) {return pmxend(n)-pmxstart(n);}

static int utl_pmx_get_utf8(char *txt, int32_t *ch)
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
      s++; if ( *s < 0x90 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_1) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len4_2) {
      s++; if ( *s < 0x80 || 0x8f < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len3_1);
    }
    
    fsmSTATE(len3_0) {
      s++; if ( *s < 0xA0 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len3_1) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }

    fsmSTATE(len3_2) {
      s++; if ( *s < 0x80 || 0x9f < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(len2);
    }
    
    fsmSTATE(len2) {
      s++; if ( *s < 0x80 || 0xbf < *s) fsmGOTO(invalid);
      val = (val << 6) | (*s & 0x3F);
      fsmGOTO(end);
    }
    
    fsmSTATE(invalid) {val = first; len = 1;}
    
    fsmSTATE(end)   { }
  }
  if (ch) *ch = val;
  return len;
}

// Returns the length in bytes of the character or 0 if it is '\0'
static int32_t utl_pmx_nextch(char *t, int32_t *c_ptr)
{
  int32_t len = 0;
  
  if (utl_pmx_utf8) len = utl_pmx_get_utf8(t, c_ptr);
  else if ((*c_ptr = (uint8_t)(*t))) len = 1;
  
  return len;
}

static int32_t utl_pmx_gethex(char *pat, int32_t *c_ptr)
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

static int utl_pmx_isin(char *pat, char *pat_end, int32_t ch, int32_t (*nxt)(char *, int32_t *))
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

static int32_t utl_pmx_iscapt(char *pat, char *txt)
{
  int32_t len = 0;
  uint8_t capnum = 0; 
  char *cap;
  
  if ('1' <= *pat && *pat <= '9') {
    capnum = *pat - '0';
    _logdebug("capt: %d %d",capnum,utl_pmx_capnum);
    if (capnum < utl_pmx_capnum) {
      cap = pmxstart(capnum);
      while (cap < pmxend(capnum) && *cap && (*cap == *txt)) {
        len++; txt++; cap++;
      }
      if (cap < pmxend(capnum)) len = 0;
    }
  }
  return len;
}

void utl_pmx_extend(int(*ext)(char *, char *,int,int32_t))
{
  utl_pmx_ext = ext;
}

#define UTL_PMX_QUOTED 0
#define UTL_PMX_BRACED 1

static int utl_pmx_get_limits(char *pat, char *pat_end, char *txt,int braced,
                             int32_t *c_beg_ptr, int32_t *c_end_ptr, int32_t *c_esc_ptr)
{
  int32_t c_beg='('; int32_t c_end=')'; int32_t c_esc='\0';
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
    (void)utl_pmx_nextch(txt,&ch);
    if (braced) {
           if (ch == '(')    {c_beg=ch;  c_end=')';}
      else if (ch == '[')    {c_beg=ch;  c_end=']';}
      else if (ch == '{')    {c_beg=ch;  c_end='}';}
      else if (ch == '<')    {c_beg=ch;  c_end='>';}
      else if (ch == '\xAB') {c_beg=ch;  c_end='\xBB';} /* Unicode and ISO-8859-1 "<<" and ">>" */
      else if (ch == 0x2329) {c_beg=ch;  c_end=0x232A;} /* Unicode ANGLE BRACKETS */
      else if (ch == 0x27E8) {c_beg=ch;  c_end=0x27E9;} /* Unicode MATHEMATICAL ANGLE BRACKETS */
      else if (ch == 0x27EA) {c_beg=ch;  c_end=0x27EB;} /* Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS */
      else return 0;
    }
    else {
      c_esc = '\\';
           if (ch == '"')    {c_beg=ch;  c_end=ch;}
      else if (ch == '\'')   {c_beg=ch;  c_end=ch;}
      else if (ch == '`')    {c_beg=ch;  c_end=ch;}
      else if (ch == '\xAB') {c_beg=ch;  c_end='\xBB';} /* Unicode and ISO-8859-1 "<<" and ">>" */
      else if (ch == 0x2018) {c_beg=ch;  c_end=0x2019;} /* Unicode single quotes */
      else if (ch == 0x201C) {c_beg=ch;  c_end=0x201D;} /* Unicode double quotes */
      else return 0;
    }
  }
  _logdebug("open:'%d' close:'%d' esc:'%d'",c_beg,c_end,c_esc);
  
  *c_beg_ptr = c_beg;
  *c_end_ptr = c_end;
  *c_esc_ptr = c_esc;
  return 1;
}

static int utl_pmx_get_delimited(char *pat, char *txt,int32_t c_beg, int32_t c_end, int32_t c_esc)
{
  int n;
  char *s;
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

static int utl_pmx_delimited(char *pat, char *pat_end, char *txt,int braced)
{
  int32_t c_beg; int32_t c_end; int32_t c_esc;
  if (!utl_pmx_get_limits(pat,pat_end,txt, braced, &c_beg, &c_end, &c_esc)) return 0;
  return utl_pmx_get_delimited(pat,txt,c_beg,c_end,c_esc);  
}


static int utl_pmx_class(char **pat_ptr, char **txt_ptr)
{
  int inv = 0;
  
  char *pat = *pat_ptr;
  char *txt = *txt_ptr;
  char *pat_end;
  
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
    case 'a' : utl_W(isalpha(ch))               ; break;
    case 's' : utl_W(isspace(ch))               ; break;
    case 'u' : utl_W(isupper(ch))               ; break;
    case 'l' : utl_W(islower(ch))               ; break;
    case 'd' : utl_W(isdigit(ch))               ; break;
    case 'k' : utl_W(isblank(ch))               ; break;
    case 'x' : utl_W(isxdigit(ch))              ; break;
    case 'w' : utl_W(isalnum(ch))               ; break;
    case 'c' : utl_W(iscntrl(ch))               ; break;
    case 'g' : utl_W(isgraph(ch))               ; break;
    case 'p' : utl_W(ispunct(ch))               ; break;
    case 'r' : utl_W(isprint(ch))               ; break;

    case 'i' : utl_W((ch < 0x80))               ; break;
    
    case 'h' : utl_W((ch == ' ' ||
                      ch =='\t' || ch == 0xA0)) ; break;
    
    case '.' : utl_W((ch !='\0' && ch !='\n'))  ; break;

    case '=' : utl_W(utl_pmx_isin_chars(pat+1,pat_end,ch)) ; break;
    case '#' : utl_W(utl_pmx_isin_codes(pat+1,pat_end,ch)) ; break;
    
    case 'N' : utl_W((txt[0]=='\r'
                            ? (txt[1] == '\n'? (len++) : 1)
                            : (txt[0] == '\n'?  1 : 0)    )) ; break;

    case 'Q' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_QUOTED))); break;
    case 'B' : utl_W((len=utl_pmx_delimited(pat+1,pat_end,txt, UTL_PMX_BRACED))); break;
                            
    case '$' : if (*txt == '\0') n=min_n; break;
    
    case '>' : utl_pmx_set_paterror(pat); return 0;
  
    case '^' : if (inv) utl_pmx_set_paterror(pat); inv = 0;
               utl_W((len=utl_pmx_iscapt(pat+1,txt)));
               break;
  
    case ':' : if (utl_pmx_ext)
                 utl_W((len=utl_pmx_ext(pat+1,txt,len,ch)));
               break;
  
    default  : ; //utl_pmx_set_paterror(pat);
  }
  #undef utl_W
  // }}

  // {{ Advance pattern
  while (*pat_end == '>') pat_end++;
  *pat_ptr=pat_end;
  // }}
  
  if (n < min_n) return 0;
  
  // {{ Advance matched text
  *txt_ptr = txt;
  // }}
  
  return 1;
}

static char *utl_pmx_alt_skip(char *pat)
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

static char *utl_pmx_alt(char *pat, char **txt_ptr)
{
  int paren=0;
  utl_pmx_state_s *state;
  int inv;
  char *ret = utl_emptystring;
  
  while (*pat) {
    _logdebug("ALT: %s (%d)",pat,utl_pmx_stack_ptr);
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
                if (utl_pmx_stack_ptr < 2) {
                  utl_pmx_set_paterror(pat);
                  break;
                }
                
                // {{ If we are here, we have NOT matched what is in the (...) 
                state = utl_pmx_state_top();
                inv = state->inv;
                ret = pat;
                if (inv) {
                  *txt_ptr = pmxstart(state->cap);  /* It's ok, we WANTED to fail */
                  utl_pmx_state_pop();
                  return ret;
                }
                if (state->n >= state->min_n) {       /* It's ok, we matched enough times */
                  utl_pmx_capt[state->cap][0] = state->txt;
                  utl_pmx_capt[state->cap][1] = *txt_ptr;
                  utl_pmx_state_pop();
                  return ret;
                }
                //else ret = utl_emptystring;                /* We didn't expect to fail */
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
    }
  }
  return utl_emptystring;
}

static char *utl_pmx_match(char *pat, char *txt)
{
  int32_t len;
  int32_t ch;
  int32_t c1;
  int16_t inv =0;
  utl_pmx_state_s *state;
  
  utl_pmx_state_reset();
  utl_pmx_state_push(pat,txt,1,1,0);
  
  while (*pat) {
    logdebug("match %d [%s] [%s]",pmxcount(),pat,txt);
    c1 = 0; 
    switch (*pat) {
      case '(' : pat++;
                 if (*pat == '|') {inv = 1; pat++;}
                 if (!utl_pmx_state_push(pat,txt,1,1,inv)) 
                   utl_pmx_set_paterror(pat);
                 break;
                 
      case '|' : pat = utl_pmx_alt_skip(pat);
                 break;
      
      case ')' : pat++;
                 _logdebug(")->%d",utl_pmx_stack_ptr);
                 if (utl_pmx_stack_ptr < 2) {
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
                 
                 utl_pmx_capt[state->cap][1] = txt;  
                 state->n++;
                 _logdebug("match #%d min:%d max:%d",state->n,state->min_n, state->max_n);
                 
                 if (state->n < state->max_n) { 
                   utl_pmx_capt[state->cap][0] = txt;
                   pat = state->pat; /* try to match once more */
                 }
                 else {
                   utl_pmx_capt[state->cap][0] = state->txt;  
                   utl_pmx_state_pop();
                 }
                 
                 break;
                 
      case '<' : if (!utl_pmx_class(&pat,&txt)) utl_pmx_FAIL;
                 break;

      case '%' : if (pat[1]) len = utl_pmx_nextch(++pat, &c1);

      default  : if (c1 == 0) len = utl_pmx_nextch(pat, &c1);
                 len = utl_pmx_nextch(txt, &ch);
                 if (ch != c1) {
                   _logdebug("FAIL: %d %d",c1,ch);
                   utl_pmx_FAIL;
                 }
                 txt += len;
                 pat += len;
                 break;
                 
      fail     : pat = utl_pmx_alt(pat, &txt) ; /* search for an alternative */
                 if (*pat == '\0') utl_pmx_capnum = 0;
                 break;
    }
  }
  utl_pmx_capt[0][1] = txt;
  
  for (len = utl_pmx_capnum; len < utl_pmx_MAXCAPT; len++) {
    utl_pmx_capt[len][0] = utl_pmx_capt[len][1] = NULL;
  }
  _logdebug("res: %p - %p",utl_pmx_capt[0][0],utl_pmx_capt[0][1]);
  return utl_pmx_capt[0][0];
}

char *utl_pmx_search(char *pat, char *txt)
{
  char *ret=NULL;
  
  utl_pmx_error = NULL;
  
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_utf8=0;}
    
  if (*pat == '^')  ret = utl_pmx_match(pat+1,txt);
  else while (!(ret = utl_pmx_match(pat,txt)) && *txt) {
         txt += utl_pmx_utf8 ? utl_pmx_get_utf8(txt, NULL) : 1;
       }
  _logdebug("ret: %p",ret);
  return ret;
}
#endif
#endif
//>>>//
