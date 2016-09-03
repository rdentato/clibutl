# utl

This library provides functions that are, hopefully, useful during
the developemnt phase. 

The functions are related to:

 - *Logging*. Mainly to be used for *testing* and *debugging* purpose 
 - *Memory check*
 - [*Finite State Machines*](#FSM)
 - *Containers*. 
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


## Logging

Logging functions are just wrappers around the `fprintf()` function.
They print a message on a file (by default `stderr`) adding a timestamp
before the message and a newline `\n` at the end.
To make things simple, there is only one level of logging and only one log file
at the time.

### Log messages

  - `int logprintf(char *format, ...);`
     is the main function. On executing:  
     
     ```C  
       logprintf("Warp speed to %d.",count);
       logprintf("Moving on now.")
     ```
     
     the following lines will be printed (assuming `count` value is 4):
     
     ```
       2016-09-03 11:33:01 Warp speed to 4.
       2016-09-03 11:33:01 Moving on now. 
     ```
  - `void logopen(char *fname, char *mode);`
    sets the logging file (i.e. the 
    file the messages will be printed to). The `mode` parameters can be `"w"`,
    to start a new file, or `"a"` to append to an existing file. If anything
    else is passed as parameter, it will default to `"a"`.
    If the file cannot be opened the log file will be reverted to `stderr`.
    To signal that the log file has been opened, the message `"LOG START"` is
    printed.
    
  - `void logclose(void);`
    Closes the current log files and revert back to `stderr`. Before closing it,
    the message `"LOG STOP"` is printed. Together with the `"LOG START"` message
    printed on opening the file, this is an easy way to determine how much time
    passed between 

### Testing

A simple way to create unit test (or applying
[Test Driven Development](https://en.wikipedia.org/wiki/Test-driven_development))
is to use the following functions:

  - `int logcheck(int test);`
    evaluates the test expression (whose result must be non-zero if the test passed
    and zero if it failed) and writes the result on the log file.

  - `void logassert(int test);`
    same as logcheck() but exit on failure. Should be used when the test revealed
    that something went horribly wrong.

For eaxmple, the following code:

   ```C
     x = get_number_from(outer_space);
     y = get_number_from(inner_space);
     logcheck(x > 100.);
     logassert(y > 0.);
     logcheck(x/y > 0.33);
   ```
will print, assuming `x=35.2` and `y=3.0`:

   ```
     2016-09-03 11:33:01 CHK FAIL (x > 100.) spacing.c 34
     2016-09-03 11:33:01 CHK PASS (y > 0.) spacing.c 35
     2016-09-03 11:33:01 CHK PASS (x/y > 0.33) spacing.c 36
   ```

If, instead, we had `x=145.7` and `y=0.0`, we would have had:

    ```
      2016-09-03 11:33:01 CHK PASS (x > 100.) spacing.c 34
      2016-09-03 11:33:01 CHK FAIL (y > 0.) spacing.c 35
      2016-09-03 11:33:01 CHK EXITING ON FAIL
    ```
    
and the program would have been terminated with exit code 1 to  prevent the
division by zero to happen on the next line.

The fact that `logcheck()` returns the result of the test can be used to get more
information in case of a failure:

   ```C
     y = get_number_from(inner_space);
     if (!logcheck(x > 100.)) {
       logprintf("          x: %.1f",x);
     }
     logassert(y > 0.);
     logcheck(x/y > 0.33);
   ```

   will result in:
    
   ```
      2016-09-03 11:33:01 CHK FAIL (x > 100.) spacing.c 34
      2016-09-03 11:33:01           x: 35.2
   ```

### Debugging

While using a symbolic debugger like `gdb` is the *"right thing to do"*, there
are times when you need some more traces of the execution of your program to really
get what is going on (and catch that damned bug!). Insted of using `printf()` or
`logprintf()` (which you can always do, of course) you can use the 

  - `int logdebug(char *format, ...);`
  
function that behaves exactly as the `logprintf()` function but will do nothing if
the `NDEBUG` symbol is defined.

This way you can easily differentiate between normal log messages and messages that
are there just for debugging purposes.

### Temporary disabled functions

There are times when you don't want some log message to be generated or some check
to be performed. This is esepcially true for debugging messages that tend to fill
up the log file with a lot of information you may no longer need.

You could delete the call to the logging function altogether but you might need
them again afterward and it would be a waste to have to write them again.

For cases like this, the following functions are defined:

   - `int  _logprintf(char *format, ...);`
   - `int  _logdebug(char *format, ...);`
   - `int  _logcheck(int test);`
   - `void _logassert(int test);`
   - `void _logopen(char *fname, char *mode);`
   - `void _logclose(void);`

that do nothing (`_logcheck()` will always return 1 as if the test passed).

Technically speaking I'm in violation of the C standard here (section 7.1.3):
identifiers starting with underscore are reserved for use as identifiers
with file scope.  I've tried many times to get rid of the initial underscore
(e.g. by using `X` or `X_`) but i still believe this is the best choice and
the worst that could happen is that they clash with some other library.
By the way, this is a risk that still must be taken into consideration for
any other identifier, so I'm not feeling particularly pressed on changing it.
  
<a name="FSM">
## Finite State Machines

Many piece of software are better understood (and desigend) as
[Finite State Machines](https://en.wikipedia.org/wiki/Finite-state_machine).

There are many way to represent a FSM in C. Most common methods are:
  - Using a `state` variable and a `switch{...}` within a loop. 
  - Using a table of pointers to functions that will be invoked to
    perform the actions and move from a state to another.
    
These methods, however, hide the structure of the FSM itself which is better
understood as a graph with nodes representing states and arcs representing 
transitions. 

The following macros implement a technique explained by Tim Cooper in the
the article [*Goto? Yes Goto!*](http://ftp.math.utah.edu/pub/tex/bib/complang.html#Cooper:1991:GYG)
published on the May 1991 issue of the *Computer Language* magazine.

The main advantage is to directly implement the FSM transition diagram
to the point that is extremely easy to draw the diagram from the code (which 
is not an easy task with the other methods).

The code for each state (which performs the actions for that state,
read the next input, etc. ) is enclosed in a `fsmSTATE(statename)` macro;
to move from a state to another the `fsmGOTO(statename)` is used.

The following is an example to show how easy is to relate the code to 
the transition diagram.

   ```C
     fsm {
       fsmSTATE(start_of_the_day) {
         num_cust = 0;
         fsmGOTO(gate_closed);
       }
       
       fsmSTATE(gate_closed) {
         if (ticket valid(getticket())) fsmGOTO(gate_opened);
         if (time_to_close() fsmGOTO(closed_for_the_day);
         fsmGOTO(gate_closed);          
       }
       
       fsmSTATE(gate_opened) {
         if (customer_passed()) {
           num_cust++;
           if (num_cust == 100) fsmGOTO(closed_for_the day);
           if (time_to_close()) fsmGOTO(closed_for_the day);
           fsmGOTO(gate_closed);
         }
         fsmGOTO(gate_opened);
       }
       
       fsmSTATE(closed_for_the_day) {
         printf("%d customer served", num_cust);
       }
     }
   ```
   
   ``` 
                                    ,--------------.
                                   v                \    
          start_of_the_day --->  gate_closed  ---> gate_opened --.
                                    \   \            ^           /
                                     \   `-----------'          /
                                      \                        V
                                       '------> closed_for_the_day
   ```

The original article had many suggestions on how to extended this
basic idea. However, after so many years, I still think that this
is the best way to represent FSM in C code.