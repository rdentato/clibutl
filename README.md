# clibutl

Small C library

## How to use utl

Include the `utl.h` header in your source files.
Compile and link `utl.c` with the other files.

Alternatively, you can do without `utl.c` simply by
compiling one of your source files with the 
symbol *`UTL_MAIN`* defined (either by definining it 
before including the header, or via a compiler switch
like "`-DUTL_MAIN`"). A good candidate is the
file where your `main()` function is.


