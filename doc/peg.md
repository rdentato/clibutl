
# PEG

   ```

           ______  ____  ______ 
          /  __  \/ __ \/  __  \
         /  /_/  / ____/  (_/  /
        /  _____/\____/\ __   / 
       /  /            ___/  /
      (__/            (_____/

   ```
    
## Parser Expression Grammars

  Usually PEG tools convert a description of the grammar into
source code that implements a parser.

  Clibutl takes a different approach and allows the define the
parser directly into the source code.

  The key functions are:
  
    - `pegnew()`       Creates a new parser (of type `peg_t`)
    - `pegfree()`      Destroy the parse
    - `pegparse(parser,rule,string[,aux])` Parses the string using the specified parser

## Expression composition
