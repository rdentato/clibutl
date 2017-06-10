
# PMX
         ______  _________  ___ 
        /  __  \/      \  \/  /  
       /  /_/  /  / /  /\    \  
      /  _____/__/_/__//__/\__\ 
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
  - `&#x2329; &#x232A;` Unicode ANGLE BRACKETS 
  - `&#x27E8; &#x27E9;` Unicode MATHEMATICAL ANGLE BRACKETS 
  - `&#x27EA; &#x27EB;` Unicode MATHEMATICAL DOUBLE ANGLE BRACKETS 

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
  
  
  - `:`' : if (utl_pmx_ext)
    

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
  - `(<l>)<^1>` matches a couple of identical lower case characters.

## Recognizers
                
  - `<D>`  A possibly signed integer: `32`, `-15`, `+7`
  - `<F>`  A possibly signed decimal number (including integers): `3.2`, `-.5`, `+7`
  - `<X>`  An hexadecimal number: `F32A`, `2012`, `4aeF`
  - `<L>`  The rest of the line up to the newline character(s) (see below)
