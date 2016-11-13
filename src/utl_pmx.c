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


** [[[

# PMX
         ______   ______ ___  ___ 
        /  __  \ /      \\  \/  /  
       /  (_/  //  ) )  / )    (  
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
  
  there must be no spaint isid(char *pat,char *txt, int len, int32_t ch)
{
  char *id = txt;
  char *c = "0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  
  if (strchr(c+10,*id)) {
    do {
      id++;
    } while (strchr(c,*id));
  }
  return id-txt;
}

int isprime(char *pat,char *txt, int len, int32_t ch)
{
  long int n;
  char *s=txt;
  uint8_t p[] = {  2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,  43,
                  47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97, 101, 103, 107,
                 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
                 191, 193, 197, 199, 0};
  
  n = strtol(txt,&s,10);
  if (n == 0 || n >= 200) return 0;
  if (!strchr((char*)p,n)) return 0;
  logdebug("prime: %p %p %s",txt,s,txt);
  return (s-txt);             
}ce between `>` and `(`.
  
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
  - `« »`               French double Guillemet
  - `‹ ›`               French single Guillemet
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
  
    #include "utl.h"
  
    int isid(char *pat,char *txt, int len, int32_t ch)
    {
      char *id = txt;
      char *c = "0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
      
      if (strchr(c+10,*id)) {
        do {
          id++;
        } while (strchr(c,*id));
      }
      return id-txt;
    }
    
    int isprime(char *pat,char *txt, int len, int32_t ch)
    {
      long int n;
      char *s=txt;
      uint8_t p[] = {
          2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37, 41,  43,
         47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97, 101, 103, 107,
        109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
        191, 193, 197, 199, 0
      };
      
      n = strtol(txt,&s,10);
      if (n == 0 || n >= 200) return 0;
      if (!strchr((char*)p,n)) return 0;
      return (s-txt);             
    }
    
    int isCV(char *pat,char *txt, int len, int32_t ch)
    {
      if (*pat == 'I') return isid(pat,txt,len,ch);
      if (*pat == 'P') return isprime(pat,txt,len,ch);
      return 0;
    }

    char *buf[256];
    int line_num;
    int main(int argc, char *argv[])
    {
      line_num = 0;
      // Read a file and print the lines that start with a prime number 
      // or with an identifier. 
      while (fgets(buf,256,stdin)) {
        line_num++;
        if (pmxsearch("^<*s>(<:P>)",buf)) 
          printf("LINE %d STARTS WITH A PRIME NUMBER: (%.*s)",pmxlen(1),pmxstart(1));
        else if (pmxsearch("^<*s>(<:I>)",buf))
          printf("LINE %d STARTS WITH AN IDENTIFIER: (%.*s)",pmxlen(1),pmxstart(1));
      }
    }
  ```
  
  In the example above, matching "a prime number (less than 200)" is something that can't
be reasonably done without an external function.  

  

    
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

int(*utl_pmx_ext)(const char *pat, const char *txt, int, int32_t ch) = NULL;

const char *utl_pmx_capt[utl_pmx_MAXCAPT][2] = {{0}} ;
uint8_t     utl_pmx_capnum                   =   0   ;
const char *utl_pmx_error                    = NULL  ;


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
  const char *pat;
  const char *txt;
  int32_t min_n;
  int32_t max_n;
  int32_t n;
  int16_t inv;
  int16_t cap;
} utl_pmx_state_s;

utl_pmx_state_s utl_pmx_stack[utl_pmx_MAXCAPT];
uint8_t utl_pmx_stack_ptr = 0;

static void utl_pmx_state_reset(void)
{
  utl_pmx_stack_ptr = 0;
  utl_pmx_capnum = 0;
}

static int utl_pmx_state_push(const char *pat, const char *txt, int32_t min_n, int32_t max_n, int16_t inv)
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

static int utl_pmx_state_pop(void)
{
  if (utl_pmx_stack_ptr == 0) return 0;
  utl_pmx_stack_ptr--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top(void)
{
  if (utl_pmx_stack_ptr == 0) return NULL;
  return utl_pmx_stack + (utl_pmx_stack_ptr-1);
}

int utl_pmx_len(uint8_t n) {return (int)(pmxend(n)-pmxstart(n));}

static int utl_pmx_get_utf8(const char *txt, int32_t *ch)
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
static int32_t utl_pmx_nextch(const char *t, int32_t *c_ptr)
{
  int32_t len = 0;
  
  if (utl_pmx_utf8) len = utl_pmx_get_utf8(t, c_ptr);
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

void utl_pmx_extend(int(*ext)(const char *, const char *, int, int32_t))
{
  utl_pmx_ext = ext;
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
      else if (ch == 0x2039) {c_beg=ch; c_end=0x203A;} /* Unicode single quotes */
      else if (ch == 0x27E8) {c_beg=ch; c_end=0x27E9;} /* Unicode MATHEMATICAL ANGLE BRACKETS */
      else if (ch == 0x27EA) {c_beg=ch; c_end=0x27EB;} /* Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS */
    }
    else { // Quoted string
      c_esc = '\\';
           if (ch == '"')    {c_beg=ch; c_end=ch;}
      else if (ch == '\'')   {c_beg=ch; c_end=ch;}
      else if (ch == '`')    {c_beg=ch; c_end=ch;}
      else if (ch == 0x2018) {c_beg=ch; c_end=0x2019;} /* Unicode single quotes */
      else if (ch == 0x201C) {c_beg=ch; c_end=0x201D;} /* Unicode double quotes */
    }
    if (c_beg=='\0') {
           if (ch == '\xAB') {c_beg=ch; c_end='\xBB';} /* Unicode and ISO-8859-1 "<<" and ">>" */
      else if (ch == 0x2329) {c_beg=ch; c_end=0x232A;} /* Unicode ANGLE BRACKETS */
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
    case 'a' : utl_W(( ch<=0xFF && isalpha(ch) )); break;
    case 's' : utl_W(( ch<=0xFF && isspace(ch) )); break;
    case 'u' : utl_W(( ch<=0xFF && isupper(ch) )); break;
    case 'l' : utl_W(( ch<=0xFF && islower(ch) )); break;
    case 'd' : utl_W(( ch<=0xFF && isdigit(ch) )); break;
    case 'k' : utl_W(( ch<=0xFF && isblank(ch) )); break;
    case 'x' : utl_W(( ch<=0xFF && isxdigit(ch))); break;
    case 'w' : utl_W(( ch<=0xFF && isalnum(ch) )); break;
    case 'c' : utl_W(( ch<=0xFF && iscntrl(ch) )); break;
    case 'g' : utl_W(( ch<=0xFF && isgraph(ch) )); break;
    case 'p' : utl_W(( ch<=0xFF && ispunct(ch) )); break;
    case 'r' : utl_W(( ch<=0xFF && isprint(ch) )); break;

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
                if (inv) { /* It's ok, we WANTED to fail */
                  *txt_ptr = pmxstart(state->cap);  
                  utl_pmx_state_pop();
                  return ret;
                }
                if (state->n >= state->min_n) {       /* It's ok, we matched enough times */
                  utl_pmx_capt[state->cap][0] = state->txt;
                  utl_pmx_capt[state->cap][1] = *txt_ptr;
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
  int32_t ch;
  int32_t c1;
  int16_t inv =0;
  utl_pmx_state_s *state;
  
  utl_pmx_state_reset();
  utl_pmx_state_push(pat,txt,1,1,0);
  
  while (*pat) {
    _logdebug("[MATCH] %d [%s] [%s]",pmxcount(),pat,txt);
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

const char *utl_pmx_search(const char *pat, const char *txt, int fromstart)
{
  const char *ret=NULL;
  
  utl_pmx_error = NULL;
  
       if (strncmp(pat,"<utf>",5) == 0) {pat+=5; utl_pmx_utf8=1;}
  else if (strncmp(pat,"<iso>",5) == 0) {pat+=5; utl_pmx_utf8=0;}
    
  if (*pat == '^')  ret = utl_pmx_match(pat+1,txt);
  else while (!(ret = utl_pmx_match(pat,txt)) && *txt && !fromstart) {
         txt += utl_pmx_utf8 ? utl_pmx_get_utf8(txt, NULL) : 1;
       }
  _logdebug("ret: %p",ret);
  return ret;
}

#endif
#endif
//>>>//
