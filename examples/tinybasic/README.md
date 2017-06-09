# A TinyBasic to C translator.

This program converts a TinyBasic program into C and tries to compile (if the cc compiler
is available).

    Usage: tbc [-o prg | -c] src
       -o prg  generate prg
       -c      generate C source code (on stdout)

  It could be improved in many ways, please consider that this is basically an example of 
how to use the PEG facilities offered by clibutl.

  The parser code is very close to the original grammar, making easier to understand and
modify it.
       
## The language
  A good source of information about this seminal programming language can be found on 
Wikpedia:  https://en.wikipedia.org/wiki/Tiny_BASIC

  This implementation follows quite closely the original grammar. The most notable exception
being the instruction POKE (and its companion function PEEK()) that allow writing and reading
from a byte array of 1024 elements.
  
      progam ::= line*
       
      line ::= number statement
      
      statement ::= "PRINT" pr-expr-list  /
                    "INPUT" in-var-list   /
                    "LET"? var "=" expr /
                    "GOTO" expr  /
                    "GOSUB" expr  /
                    "NEXT" /
                    "RETURN" /
                    "IF" expr relop expr "THEN" statement /
                    "END" /
                    "REM" /
                    "POKE" expr "," expr 
      
      var ::= "A" | "B" | ... | "Z"
      string ::= '"' char* '"'
      
      pr-expr-list ::= (string|expression) ((','/';') (string|expression) )* ';'?
      in-var-list ::= var (',' var)*
      
      expr = (term ('+' / '-'))* term
      term = (fact ('*' / '/'))* fact
      fact = '-' fact / '(' expr ')' / num / var / fun
      fun = 'RND' '(' expr ')' / 'PEEK' '(' expr ')' 
      relop ::= '<' ('>' | '=')? | '>' '='? | '='
      number ::= digit digit*         
      digit ::= '0' | '1' | '2' | '3' | ... | '8' | '9'
       