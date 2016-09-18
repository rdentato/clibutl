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


### Patterns

  Every character is a pattern that matches itself with the following
exceptions:

  - `<` Starts a pmx recognizer.
  - `>` Ends a pmx recognizer.
  - `(` Starts a sub-expression (capture).
  - `)` Ends a sub-expression (capture).
  - `|` Separates alternatives.
  - `%` Escapes next character (`%(` is the character `(` itself)


### Character class recognizers

  These patterns match characters that are within commonly used sets.

  - `a`  (isalpha)	matches an alphabetic character
  - `c`  (iscntrl)	matches a control character
  - `d`  (isdigit)	matches a digit. Not locale-specific.
  - `g`  (isgraph)	matches a graphic character, excluding the space character.
  - `i`  (isascii) matches a character whose code is between 0 and 127
  - `k`  (isblank)	matches a blank character 
  - `l`  (islower)	matches a lowercase character
  - `p`  (ispunct)	matches a punctuation character
  - `q`  (isalnum)	matches an alphanumeric character
  - `r`  (isprint)	matches a printable character, including the space character.
  - `s`  (isspace)	matches a space character
  - `u`  (isupper)	matches an uppercase character
  - `w`  (isword)) matches an alphanumeric character or and underscore ''|_|'
  - `x`  (isxdigit) matches an hexadecimal digit. Not locale-specific.
  - `.`  matches any character except `\0` and `\n` .
  

### Recognizer modifiers
   
  - `!`      negates the recognizer: `<!d>` is any character that is not a digit
  - `*`      matches 0 or more times 
  - `+`      matches 1 or more times 
  - `?`      matches 0 or 1 times 
  - *n*      matches exactly *n* times
  - *n*-     matches at least *n* times
  - -*m*     matches at most *m* times
  - *n*-*m*  matches from *n* to *m* times
  
 Examples:
  - `<3d>`   matches 3 digits
  - `<3!d>`  matches 3 non-digits
  - `<+>(ab|cd)`
             matches strings like: "abab" "abcdab" "cdabcdcd" etc
  - `<-3=xyz>` matches "xy" "xx" "zyx" "xxx" "z" ""
  
### Text encoding
  
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
  

### User defined character classes

  It is possible to match a character that is in in a given set as follows:

 - `<=...>`  characters list
 - `<#...>`  characters codes (in hex)
  
 Example:
 
 - `<=A-Ga-g>`     matches one of the following characters: `ABCDEFGabcdefg`
 - `<!=xyz>`       matches anything except the characters `x`, `y` and `z`
 - `<#0370-03FF>`  matches Greek and Coptic characters
 - `<=-+>`         matches `+` or `-` (must be the first character in the set)
 - `<=])}>>`       matches a closing bracket including `>` (must be the last one)
 
  
### Alternatives

  To match one of alternative patterns use the '|' character. For example:
  
  - `a(b|c)|d` matches `ab` `ac` or `d` 
 ..
   
  If the characters '&', '>' or '$' are part of the strings specified,
they must be preceeded by '&'.  For example:

 .['|<$a&$b$c&>d>'] matches ''|a$b|' or  ''|c>d|'.
 ..
   
  
.%% Submatching captures
''''''''''''''''''''''''
  Submatches can be denoted using parenthesis. There can be up to 9 
submatches. Submatches can be nested. Examples:

  .['|(&f)(<?$in$cm>)|]    matches "'|2.0cm|", "'|-3in|", "'|4.1|" etc.;
                           saves the number as capture 1 and the unit of
                           measure as capture 2.
                        
   ['|=((<?=+&->)<+d>))|]  matches "'|=-5|", "'|=42|", "'|=+1|" etc.;
                           saves the signed number as capture 1 and the
                           sign as capture 2;
  .. 

.%% Back references
'''''''''''''''''''
  It is possible to refer to captured test with '|<1>|, '|<2>|, ...,  '|<9>|.
  
  For example: '|(<u>)<1>| matches the doubling of an upper case character as
'|XX| or '|AA|.  
  

.%% Recognizers
'''''''''''''''
  Recognizers are patterns that match a specific object (for example
"'|&q|" for a quoted string).
  
  .['|&b|'/xy/] Braced string. Matches a string enclosed by the characters
               /x/ and /y/.
               
               If /x/ and /y/ are different there must be a balanced number
               of delimiters to complete the match.  For example the 
               pattern "'|%b()|" matches the entire string
               "'|(defun f (x) (+ x 1))|".
               
               If /x/ and /y/ are equal, the match stops at the first occurence
               of the delimiter character.
               
               The recognires '|&b| honours the escape character set with '|&e|
               (see below)
                
   ['|&d|]     A possibly signed integer (e.g. : "'|32|", "'|-15|", "'|+7|")
   ['|&f|]     A possibly signed decimal number. It will also match integers
               (e.g. : '|3.2|, '|-.5|, '|+7|).
               
   ['|&l|]     The rest of the line up to the newline character(s) (see below)
   ['|&n|]     The end of the line. Matches any of "'|\r\n|", "'|\r|" or "'|\n|"
   ['|&q|]     A single or double quoted string (including the quotes). To
               match just one type of quoted string use the "'|&b|" recognizer
               (e.g. "'|&b'`'|" for single quoted strings).
 
               The recognizer '|&q| honours the escape character set with '|&e|
               (see below).
               
   ['|&x|]     An hexadecimal number (e.g. "'|C1A0|")
  ..




.% pmx API '<pmxapi>
""""""""""

   pmx functions use objects of type '{pmx_t} to keep track of 
what's has been matched. From and API perspective, it has to be treated
as an '/opaque/ pointer; inner details can be found in the
'<pmx.c=pmx.c#matchinfo> file.

.%% Matching on a string
''''''''''''''''''''''''

  The simplest way to match a string against a pattern is to use the
'{=pmxMatchStr()} function.
  Upon a successuful match, it returns a pointer of type '|pmx_t| that
can be used to retrieve information on the match. 
  If no the string desn't match, '|NULL| is returned.


.%% Got a match '<gettxt>
'''''''''''''''
  Upon a successful match, the following functions will retrieve details
of the match. Their first argument is of type '{pmxMatch_t} and is 
typically the value returned by a '{pmxMatchStr()} function call.
 
  .['{=pmxMatched(mtc)}] 
                       Which sub pattern matched. Assuming a pattern is
                       made of a sequence of "'|...&`|...&`|...|", this
                       function will return which of the alternatives 
                       matched. The alternatives are numbered starting
                       with 1. It returns 0 if none matched.
                       
   ['{=pmxLen(mtc,n)}]
                       The length of the capture '{n}. The entire match is
                       considered the capture 0.
                         
   ['{=pmxStart(mtc,n)}]
                       The start of the capture '{n}. The entire match is
                       considered the capture 0.
                       This is an integer offset from the start of the string
                       that was passed to '{pmxMatchStr()}'. 
                         
   ['{=pmxEnd(mtc,n)}]
                       The end of the capture '{n}. The entire match is
                       considered the capture 0.
                       This is an integer offset from the start of the string
                       that was passed to '{pmxMatchStr()}'. 

   ['{=pmxToken(mtc)}]
                       Returns the token associated with the alternative
                       that matched.
                       It can also return two special values:
                       .['{=pmxTokNONE}]  No token associated or no match (0x00).
                        ['{=pmxTokEOF}]   The string to match was empty (0x7F).
                       ..
                       Tokens can be associated to each alternative. 
                       The pattern "'|&|'/x/" where '/x/ is an ASCII
                       character greater or equal to 0x80 set the token
                       value to '/x/.                         
  ..  


.%% pmx switch
~~~~~~~~~~~~~~~~~

  The macro '{=pmxSwitch()} implements an '/extension/ of the C '|switch|
  control structure.

.v
   #define T_LETTERS  xF1     <--- tokens are in the form '|x|'/HH/
   #define T_NUMBERS  xF2          where HH is an hex number >= 128
   #define T_OTHER    xFE
   
   pmxSwitch (s,     <- this '*must* be a '|char *| variable
     pmxTokSet("<+=0-9>",T_NUMBERS)            <--- Note: '*NO* semicolon   
     pmsTokSet("<+=A-Z>",T_LETTERS)               at the end of lines     
     pmsTokSet("<+=a-z>",T_LETTERS)            <---                       
     pmsTokSet("<.>",T_OTHER)     
   ) {
     pmxTokCase(TK_LETTERS):
       printf("LETTERS: %.*s\n",pmxTokLen(0),pmxTokStart(0));
       break;

     pmxTokCase(TK_NUMBERS):
       printf("NUMBERS: %.*s\n",pmxTokLen(0),pmxTokStart(0));
       break;
   }   
..

  This behaves like a switch (with '{=pmxTokCase} used instead of
'|case|).

  The '|s| argument must be a char * variable that points to to thext
to be scanned. Upon successful match, it will be advanced to the end of
the match.

  There are two special tokens that are always defined:
  
  .['|pmxTokEOF|]    returned at the end of the text to parse;
   ['|pkxTokNONE|]   returned if none of the defined patterns matches
  ..

.%% Callback Scanners
~~~~~~~~~~~~~~~~~~~~~

  The function '{=pmxScanStr()} can be used to repeatedly match a pattern
against a text and call a function each time a match is found.
  
  The pattern is usually made of a list of expression joined with the '|&`||' 
operator.
  
  On a successful match, the callback function is called passing the matching
results as parameters.  

  If the callback function returns zero, the pattern is matched again
starting from the first character after the end of the previous match, 
otherwise  the scanner stops and '{pmxScanStr()} returns that value. 

  Look at the '<pmxshell.pmx#> for an example of how to use this function. 

  Callback functions must be of type '{=pmxScanFun_t types}. 

  Within a call back, you can pass the '|txt| and '|mtc| arguments to the
functions to get the <matched text=gettxt>.

.%% Loop scanners
'''''''''''''''''''

  To ease the definition of more complex scanners, the following
alternative is provided. The syntax makes use of conventions that 
need to be abided to make everything work.

  Basically, one define a set of tokens, associate one or more 
pmx patterns to them and then specify the actions to perform when a
token is encountered in the text. 
  
.v
   #define T_LETTERS  xF1     <--- tokens are in the form '|x|'/HH/
   #define T_NUMBERS  xF2          where HH is an hex number >= 128
   #define T_OTHER    xFE
   
   pmxScannerBegin("Text to be scanned ....")  <--- Note: '*NO* semicolon
                                                    at the end of lines
     pmxTokSet("<+=0-9>",T_NUMBERS)              <---
     pmsTokSet("<+=A-Z>",T_LETTERS)
     pmsTokSet("<+=a-z>",T_LETTERS)
     pmsTokSet("<.>",T_OTHER)
     
   pmxScannerSwitch    <--- This sections is similar to a
                            regular switch() statement
     pmxTokCase(TK_LETTERS):
       printf("LETTERS: %.*s\n",pmxTokLen(0),pmxTokStart(0));
       continue;

     pmxTokCase(TK_NUMBERS):
       printf("NUMBERS: %.*s\n",pmxTokLen(0),pmxTokStart(0));
       continue;
       
     default: continue; 
      
   pmxScannerEnd;      <--- Note: the semicolon at the end!!
   
..

  Scanning will stop as long as the end of text is reached, no match
is found or a break is executed within the '|pmxTokSwitch| section. 



.% Tools
""""""""
  The '|examples| directory contains two useful tools for pmx.
  
  .[pmxshell]   an interactive shell that allows to define a set of 
                pmx patterns and test strings against them.  It is 
                intended to be used to ensure that a set of patterns
                behaves as it is supposed to.
               
   [pmx2c]      a preproprocessor that modifies the way the switch
                statment works allowing cases that are pmx patterns
                rather than constants. 
  ..
  
  You should read about them to better understand how to use pmx.
  


.% Summmary
===========

.[]
  ['|<?|'/.../'|>|]  matches 0 or 1 times
  ['|<+|'/.../'|>|]  matches 1 or more times
  ['|<*|'/.../'|>|]  matches 0 or more times
  ['|<=|'/.../'|>|]  matches characters in set
  ['|<!|'/.../'|>|]  matches characters not in set
  ['|<$|'/.../'|$|'/.../'|>|]  matches one of the strings
  ['|<a>|]  (isalpha)	matches an alphabetic character
  ['|<c>|]  (iscntrl)	matches a control character
  ['|<d>|]  (isdigit)	matches a digit. Not locale-specific.
  ['|<g>|]  (isgraph)	matches a graphic character, excluding the space character.
  ['|<i>|]  (isascii) matches a character whose code is between 0 and 127
  ['|<k>|]  (isblank)	matches a blank character 
  ['|<l>|]  (islower)	matches a lowercase character
  ['|<p>|]  (ispunct)	matches a punctuation character
  ['|<q>|]  (isalnum)	matches an alphanumeric character
  ['|<r>|]  (isprint)	matches a printable character, including the space character.
  ['|<s>|]  (isspace)	matches a space character
  ['|<u>|]  (isupper)	matches an uppercase character
  ['|<w>|]  (isword)) matches an alphanumeric character or and underscore ''|_|'
  ['|<x>|]  (isxdigit) matches an hexadecimal digit. Not locale-specific.
  ['|<.>|]  matches any character (except ''|\0|').
  ['|<1>|]  matches capture 1
  ['|<2>|]  matches capture 2
  ['|<3>|]  matches capture 3
  ['|<4>|]  matches capture 4
  ['|<5>|]  matches capture 5
  ['|<6>|]  matches capture 6
  ['|<7>|]  matches capture 7
  ['|<8>|]  matches capture 8
  ['|<9>|]  matches capture 9
  ['|>|]    at the start of the pattern matches everywhere in the text
            at the end of the pattern, matches the end of the text
 ..
..

.[]
   ['|&:||]    expressione lengt
   ['|&`||]    Alternate expression
   ['|&E|]     Clear the escape character.
   ['|&G|]    Set the /goal/ of the match.
   ['|&I|]    Stop case insensitive match.
   ['|&b|'/xy/] Braced string.                
   ['|&d|]     A possibly signed integer
   ['|&e|'/x/] Set the the escape character.
   ['|&f|]     A possibly signed decimal number.               
   ['|&i|]     Start case insensitive match.
   ['|&k|]     A sequence of blanks (space or tab)
   ['|&l|]     The rest of the line up to the newline character(s)
   ['|&n|]     The end of the line.
   ['|&q|]     A single or double quoted string (including the quotes).
   ['|&r|]     Matches the beginning of a line ()
   ['|&s|]     A sequence of spaces (space, tab, newlines, vertical tabs, etc)
   ['|&w|]     An hexadecimal number.
   ['|&x|]     An hexadecimal number.
  ..

..

]]]

*/

#include "utl.h"

//<<<//
#ifndef UTL_NOPMX
#ifdef UTL_MAIN

int(*utl_pmx_ext)(char *pat, char *txt, int, int32_t ch) = NULL;

char     *utl_pmx_capt[utl_pmx_MAXCAPT][2] = {{0}} ;
uint8_t   utl_pmx_capnum                   =   0   ;
char     *utl_pmx_error                    = NULL  ;


#define utl_pmx_set_paterror(t) do {if (utl_pmx_error == utl_emptystring) {utl_pmx_error = t;}} while (0)

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
uint8_t utl_pmx_stack_cnt = 0;

static void utl_pmx_state_reset()
{
  utl_pmx_stack_cnt = 0;
  utl_pmx_capnum = 0;
}

static int utl_pmx_state_push(char *pat, char *txt, int32_t min_n, int32_t max_n, int16_t inv)
{
  utl_pmx_state_s *state;
  
  if (utl_pmx_stack_cnt >= utl_pmx_MAXCAPT) return 0;
  
  state = utl_pmx_stack + utl_pmx_stack_cnt;
  
  state->pat   = pat;
  state->txt   = txt;
  state->min_n = min_n;
  state->max_n = max_n;
  state->n     = 0;
  state->inv   = inv;
  state->cap   = utl_pmx_capnum;
  
  utl_pmx_newcap(txt);
  utl_pmx_stack_cnt++;
  
  return 1;
}

static int utl_pmx_state_pop()
{
  if (utl_pmx_stack_cnt == 0) return 0;
  utl_pmx_stack_cnt--;
  return 1;
}

static utl_pmx_state_s *utl_pmx_state_top()
{
  if (utl_pmx_stack_cnt == 0) return NULL;
  return utl_pmx_stack + (utl_pmx_stack_cnt-1);
}

size_t utl_pmx_len(uint8_t n) {return pmxend(n)-pmxstart(n);}

static int utl_pmx_get_utf8(char*t, int32_t *ch)
{
  int len = 0;
  int32_t val;
  val=0;
  
       if ((*t & 0x80) == 0x00) { val =  *t        ; len = 1; }
  else if ((*t & 0xE0) == 0xC0) { val = (*t & 0x1F); len = 2; }
  else if ((*t & 0xF0) == 0xE0) { val = (*t & 0x0F); len = 3; }
  else if ((*t & 0xF8) == 0xF0) { val = (*t & 0x07); len = 4; }
  
  switch (len) {  /* WARNING: falls through! */
    case 4: if ((*++t & 0xC0) != 0x80) {len=0; break;}
            val = (val << 6) | (*t & 0x3F);
    case 3: if ((*++t & 0xC0) != 0x80) {len=0; break;}
            val = (val << 6) | (*t & 0x3F);
    case 2: if ((*++t & 0xC0) != 0x80) {len=0; break;}
            val = (val << 6) | (*t & 0x3F);
  }
  if (len && ch) *ch = val;
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
  
    default  : utl_pmx_set_paterror(pat);
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
                if (utl_pmx_stack_cnt < 2) {
                  utl_pmx_set_paterror(pat);
                  break;
                }
                
                // {{ If we are here, we have NOT matched what is in the (...) 
                state = utl_pmx_state_top();
                inv = state->inv;
                ret = pat;
                if (inv) *txt_ptr = pmxstart(state->cap);  /* It's ok, we WANTED to fail */
                else if (state->n >= state->min_n) {       /* It's ok, we matched enough times */
                  utl_pmx_capt[state->cap][0] = state->txt;
                  utl_pmx_capt[state->cap][1] = *txt_ptr;
                }
                else ret = utl_emptystring;                /* We didn't expect to fail */
                // }}

                utl_pmx_state_pop();
                return ret;

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
                 _logdebug(")->%d",utl_pmx_stack_cnt);
                 if (utl_pmx_stack_cnt < 2) {
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
                 if (ch != c1) utl_pmx_FAIL;
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
  
  utl_pmx_error = utl_emptystring;
  
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
