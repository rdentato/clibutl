# utl

This library provides functions that are, hopefully, useful during
the developemnt phase. 

The functions are related to:

 - [*Logging*](https://github.com/rdentato/clibutl/blob/master/doc/log.md)
 - [*Finite State Machines*](https://github.com/rdentato/clibutl/blob/master/doc/fsm.md)
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
 
 
## How to use `clibutl`

 All you need is in the `dist` directory:

 - Copy `utl.c` and `utl.h` in a place that is suitable for your project
 - Include the `utl.h` header in your source files.
 - Compile and link `utl.c` with the other files.


## The *selector* symbols
   
  You may want to entirely exclude a portion of the library because
it is of no interest for your project. To do so you may define
one or more of the following symbols:

 - `UTL_NOLOG` will make the logging functions `logxxx` unavailble.
 
 - `UTL_NOFSM` will make the finite state machines `fsmxxx` constructors unavailble.
 
 - `UTL_NOPMX` will make the pattern matching functions `pmxyyy` unavailble.
 
 - `UTL_NOVEC` will make the containers functions (`vecxxx`, `stkxxx`, `bufxxx`, ...) unavailble.

## Documentation
  The `doc` directory contains the full documentation of the library functions