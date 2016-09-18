
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
  