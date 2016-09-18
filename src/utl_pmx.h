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


//<<<//
#ifndef UTL_NOPMX

#define utl_pmx_MAXCAPT 16

extern int(*utl_pmx_ext)(char *pat, char *txt, int, int32_t ch);

extern char     *utl_pmx_capt[utl_pmx_MAXCAPT][2];
extern uint8_t   utl_pmx_capnum                  ;
extern char     *utl_pmx_error                   ;

#define pmxsearch(r,t)  utl_pmx_search(r,t)
#define pmxstart(n)    (utl_pmx_capt[n][0])
#define pmxend(n)      (utl_pmx_capt[n][1])
#define pmxcount()     (utl_pmx_capnum)
#define pmxlen(n)       utl_pmx_len(n)
#define pmxerror()     (utl_pmx_error)
#define pmxextend(f)   (utl_pmx_ext = f)

char *utl_pmx_search(char *pat, char *txt);
size_t utl_pmx_len(uint8_t n);
void utl_pmx_extend(int(*ext)(char *, char *,int, int32_t));

#endif
//>>>//
