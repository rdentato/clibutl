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
*/
//>>>//
/*
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
//<<<//
#include "utl.h"
#define UTL_MAIN

const char *utl_emptystring = "";

int   utl_ret(int x)      {return x;}
void *utl_retptr(void *x) {return x;}

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


//>>>//

