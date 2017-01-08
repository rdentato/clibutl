# utl [![Build Status](https://travis-ci.org/rdentato/clibutl.svg?branch=master)](https://travis-ci.org/rdentato/clibutl)

  This library provides functions that could, hopefully, speed up the
developemnt of a C program.

The functions are related to:

 - [*Logging*](https://github.com/rdentato/clibutl/blob/master/doc/log.md)
 - [*Finite State Machines*](https://github.com/rdentato/clibutl/blob/master/doc/fsm.md)
 - *Memory check*
 - *Containers*
 - [*Pattern matching*](https://github.com/rdentato/clibutl/blob/master/doc/pmx.md) over string
 - [*Try/Catch* exceptions](https://github.com/rdentato/clibutl/blob/master/doc/try.md)

They provide simple and reasonably easy to use solutions to recurring
problems.

  Even if I made any effort to make them reasonably efficient, I favoured
ease of use and simplicity above everything else. You may use them during
development and replaced them later if something more efficient is needed.

  For instance, the containers functions can help you shape the most
appropriate data structures for your problem. Once done, you can replace
them with something faster or more compact.

  The point is to avoid having to think about trivial things (like
logging, for instance) and directly jump into solving your problem. 
 
 
## How to use `clibutl`

 The only requirement for clibutl to work is compliancy to C99 standard.
I've tested it with gcc, clang, g++, Pelles C and Microsoft C/C++

 All you need is in the `dist` directory:

 - Copy `utl.c` and `utl.h` in a place that is suitable for your project
 - Include the `utl.h` header in your source files.
 - Compile and link `utl.c` with the other files.

 The documentation is in the `doc` directory and I've also opened a [blog](https://clibutl.blogspot.it/)
where I'll post about c-libutl from time to time.
 
## Hot to re-build `clibutl`

  There two building mechanisms supported:
  
  - A `makefile` (for Linux and Mingw). Targets are:
  
      - `all`     : Compile
      - `clean`   : Cleanup the directories from build leftovers
      - `dist`    : Compile and  the distribution directory
      - `runtest` : Compile and run the tests
    
  - A batch script (`build.bat`) for Windows. Accepts the following options:
  
      - `clean`   : Cleanup the directories from build leftovers
      - `test`    : Just run the tests (no compilation)
      - `MSC`     : Compile and run tests (Microsoft C/C++)
      - `PCC`     : Compile and run tests (Pelles C 32bit)
      - `PCC64`   : Compile and run tests (Pelles C 64bit)

  The "official" development environment for `clibutl` is bash and
the GNU tool chain (gcc, make, ...). Tested with `gcc` and `clang`.

 
## The *selector* symbols
   
  You may want to entirely exclude a portion of the library because
it is of no interest for your project. To do so you may define
one or more of the following symbols before including `utl.h`:

 - `UTL_NOLOG` will make the logging functions `logxxx` unavailble.
 
 - `UTL_NOTRY` will make the try/catch exceptions unavailble.
 
 - `UTL_NOFSM` will make the finite state machines `fsmxxx` constructors unavailble.
 
 - `UTL_NOPMX` will make the pattern matching functions `pmxyyy` unavailble.
 
 - `UTL_NOVEC` will make the containers functions (`vecxxx`, `bufxxx`, ...) unavailble.

## Documentation
  The `doc` directory contains the full documentation of the library functions 
