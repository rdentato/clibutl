# utl

This library provides functions that are, hopefully, useful during
the developemnt phase. 

The functions are related to:

 - [*Logging*](#logging)
 - [*Finite State Machines*](#finite-state-machines)
 - *Memory check*
 - *Containers*
 - *Pattern matching* over string

The objective is to provide simple and reasonably
easy to use solutions to recurring problems that could be replaced
by more complex solutions if needed. For instance, the logging
functions are limited to write on files, for production environment
they can be replaced by some other logging (e.g. something with
multiple level of logging or using syslogd, ...).

The point is to avoid having to think about trivial things (again, like
logging) and directly jump into your problem. 
 
 
## How to use utl

 - Include the `utl.h` header in your source files.
 - Compile and link `utl.c` with the other files.

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

## Documentation
  The `doc` directory contains the full documentation of the library functions