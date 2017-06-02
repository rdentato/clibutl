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
/* returns log2(n) assuming n is 2^m */
int utl_unpow2(int n)
{ int r;

  r  =  (n & 0xAAAA) != 0;
  r |= ((n & 0xCCCC) != 0) << 1;
  r |= ((n & 0xF0F0) != 0) << 2;
  r |= ((n & 0xFF00) != 0) << 3;
  return r;
}



/*
  Dealing with text encoding is a complex business. The most basic
issue for pmx is to deal with lower/upper case characters.

  Even just restricting to the main scripts that have the lower/upper
case distinction (Latin, Greek and Cyrillic) and the major encodings
(Unicode, ISO/IEC, Windows code pages, ...) would result in providinge
something that could be of little use for somebody and of no use for many.

  So, I went for the easiest solution: the Latin-1
characters in the iso-8859-1 and Unicode Latin-1 supplement.
In other words: the characters encoded in a single byte.

  We need to extend the functions `islower()`, `isupper()`, `isalpha()`,
`isalnum()` to include the letters in the range 0xA0-0xFF.

  I've decided to not include the "numeric" caharacters for
superscript or fractions, It seeems counterintuitive to me that
`isdigit(0xBD); // 1/2` returns true. 
  
  To represent this encoding, we need four bits for each character:

    xxxx
    \\\
     \\\_____ isupper 
      \\_____ islower
       \_____ isdigit     
       
  This allows using a table with 128 bytes rather than 256.
*/

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


//>>>//

