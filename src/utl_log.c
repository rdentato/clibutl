/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                        ___   __
**                     __/  /_ /  )
**             ___  __(_   ___)  /
**            /  / /  )/  /  /  /
**           /  (_/  //  (__/  / 
**          (____,__/(_____(__/
**    https://github.com/rdentato/clibutl
**


[[[
# Logging

   ```
            __
           /  )
          /  /______   ______
         /  //  __  \ /  __  \
        /  //  (_/  //  (_/  /
       (__/ \______/ \___   /
                      __/  /
                     (____/
   ```

Logging functions are just wrappers around the `fprintf()` function.
They print a message on a file (by default `stderr`) adding a timestamp
before the message and a newline `\n` at the end.
To make things simple, there is only one level of logging and only one log file
at the time.

## Log messages

  - `void logprintf(char *format, ...);`
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
      
     
  - `FILE *logopen(char *fname, char *mode);`
    sets the logging file (i.e. the 
    file the messages will be printed to). The `mode` parameters can be `"w"`,
    to start a new file, or `"a"` to append to an existing file. If anything
    else is passed as parameter, it will default to `"a"`.
    On success, the function returns the pointer to the log file. On failure
    it will return NULL and the log file will be reverted to `stderr`.
    To signal that the log file has been opened, the message `"LOG START"` is
    printed.
    
  - `void logclose(void);`
    Closes the current log files and revert back to `stderr`. Before closing it,
    the message `"LOG STOP"` is printed. 

## Tracing & debugging

  While using a symbolic debugger like `gdb` is the *"right thing to do"*, there
are times when you need some more traces of the execution of your program to really
get what is going on (and catch that damned bug!). 

  The simplest way would be to use the `logdebug()` that will disappear if compiled 
with `NDEBUG` defined:

  - `void logdebug(char *format, ...);`

  Using `logdebug()` is recommended when, during developement, one wants to better
understand why a specific function is not working as expected.

  In more complex situations, where more functions work together and one wants to 
check that their interaction is working fine, using `logtrace()` could be a better 
option. It works with the `logtracewatch()` function (see next section) to help
checking everything works as expected.

  - `void logtrace(char *format, ...);`
  
function that works as the `logprintf()` function but will produce lines like:

    ...
      2016-11-05 14:08:57 TRC Freeing a block of 0 bytes (0) test/ut_mem.c:49
    ...
	
  The `TRC` tag identify tracing messages, the `DBG` tag identify the debugging
messages. This can be useful to filter a log file (for example with `grep` or 
`sed`) in search of speficic messages.
  
  If `NDEBUG` is defined while compiling, tracing and debugging will be disabled.
  if `UTLNOTRACE` is defined, only tracing messages are disabled.

	
## Unit Testing

A simple way to create unit test is to use the following functions:

  - `int logcheck(int test);`
    evaluates the test expression (whose result must be non-zero if the test passed
    and zero if it failed) and writes the result on the log file.

  - `void logassert(int test);`
    same as logcheck() but exits (with `abort()`)on failure. Should be used when
    the test could reveal that something went horribly wrong.

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
     2016-09-03 11:33:01 CHK FAIL (x > 100.) spacing.c:34
     2016-09-03 11:33:01 CHK PASS (y > 0.) spacing.c:35
     2016-09-03 11:33:01 CHK PASS (x/y > 0.33) spacing.c:36
   ```

If, instead, we had `x=145.7` and `y=0.0`, we would have had:

   ```
     2016-09-03 11:33:01 CHK PASS (x > 100.) spacing.c 34
     2016-09-03 11:33:01 CHK FAIL (y > 0.) spacing.c 35
     2016-09-03 11:33:01 CHK EXITING ON FAIL
   ```
    
and the program would have been terminated calling `abort()`  1 to  prevent the
division by zero to happen on the next line.

The fact that `logcheck()` returns the result of the test can be used to get more
information in case of a failure:

   ```C
     y = get_number_from(inner_space);
	 x = 135.2;
     if (!logcheck(x > 100.)) {
       logprintf("          x: %.1f",x);
     }
     logassert(y != 0.);
     logcheck(x/y > 0.33);
   ```

   will result in:
    
   ```
      2016-09-03 11:33:01 CHK FAIL (x > 100.)? spacing.c:34
      2016-09-03 11:33:01           x: 135.2
   ```

  At the end of the log, the function `logclose()` will print the number of
failures and the number of checks performed.

   ```
      2016-09-18 20:47:04 CHK #KO: 2 (of 23)
   ```

  If the symbol `NDEBUG` is defined, `logassert()` will perform no action;
and `logcheck()` will perform no action and always return `1`;

  You can also monitor if a specific tracing messages appears (or not) when
executing a block of code:

  - `logtracewatch(char *pat1, char *pat2, ...) { ... }`

  The patterns are `pmx` expressions.

  For example, in:
  
   ```C
     34: logwatch ("closed:","<not>unable to open:") {
     35:   f = carefulopen("existingfile");
     36:   carefulparse(f);
     37:   carefulclose(f);
     38: }
     39: logwatch ("parse: null file pointer","unable to open:") {
     40:   f = carefulopen("notexistingfile");
     41:   carefulparse(f);
     42:   carefulclose(f);
     43: }
   ```
    
we have set up the three functions so that they emit different messages. The first
test checks that when an existing file is used, everything is ok. The second test
checks that when the file can't be opened the functions behave properly.

  Using `logcheck` and `logtracewatch` one can define tests that use two very
different approaches:

   - `logcheck` is a way to reason about the expect results of a function.
     This allows for tests that directly checks the state of the program after a
	 function is executed.
     The drawback is that if a function is restructured, an entire set of tests may
	 become invalid and will need to be rewritten. For example a function that returned
	 an `unsigned int` may now return a `double`.
  
   - `logtracewatch` allows to indirectly verify the behaviour of a function without
     having to know about the inner details. This creates tests that are more resilient
	 to refactoring but might.
	 
  I borrowed this concept of *tracing test* from an interesting blog article by
[Kartik Agaram](http://akkartik.name/post/tracing-tests).
  
## Stopwatch

A simple function to measure the time spent in a portion of code:

  - `logclock { ... }`
     Measure the time needed to execute the portion of code between braces. Uses
     the `clock()` function whose resolution varies from system to system. On my
     Windows 10 machine is in milliseconds, in my Linux box is in microseconds.

  For example, this fragment:

   ```C
     294: k = max;
     295: logclock {
     296:   t=in;
     297:   while (k--) {
     298:     do {
     299:       l = utf8_cp(t,&c); t+=l;
     300:     } while (l>0);
     301:   }
     302: }
   ```

will generate a messages in the log file similar to this one:

   ```
     2016-09-27 21:22:44 CLK  34 (s/1000) test/ut_utf.c:295
   ```

which says that the block starting on line 295 of the file "test/ut_utf.c" took 34
milliseconds to execute.

  This is not a substitute for profiling! It's just a quick way to check if a
block of code is taking more time than expected.

## Temporarily disable logging

There are times when you don't want some log message to be generated or some check
to be performed. This is esepcially true for debugging messages that tend to fill
up the log file with a lot of information you may no longer need.

You could delete the call to the logging function altogether but you might need
them again afterward and it would be a waste to have to write them again.

For cases like this, the following functions are defined:

   - `void  _logprintf(char *format, ...);`
   - `void  _logdebug(char *format, ...);`
   - `void  _logtrace(char *format, ...);`
  -  '      _logtracewatch(char *pat1, char *pat2, ...) { ... }`
   - `int   _logcheck(int test);`
   - `void  _logassert(int test);`
   - `FILE *_logopen(char *fname, char *mode);`
   - `void  _logclose(void);`
   - `      _logclock {...}`

that do nothing (`_logcheck()` will always return 1 as if the test passed).

Technically speaking I'm in violation of the C standard here (section 7.1.3):
"identifiers starting with underscore are reserved for use as identifiers
with file scope".  I've tried many times to get rid of the initial underscore
(e.g. by using `X` or `X_`) but i still believe this is the best choice and
the worst that could happen is that they clash with some other library.
By the way, this is a risk that still must be taken into consideration for
any other identifier, so I'm not feeling particularly pressed on changing it.
  
** ]]] */

//<<<//
#ifndef UTL_NOLOG
#ifdef UTL_MAIN

FILE *utl_log_file = NULL;
uint32_t utl_log_check_num   = 0;
uint32_t utl_log_check_fail  = 0;

char *utl_log_watch[1] = {""};

int utl_log_close(const char *msg)
{
  int ret = 0;
  
  if (utl_log_check_num) {
    logprintf("CHK #KO: %d (of %d)",utl_log_check_fail,utl_log_check_num);
    utl_log_check_fail = 0;
    utl_log_check_num = 0;
  }
  if (msg) logprintf("%s",msg);
  if (utl_log_file && utl_log_file != stderr) ret = fclose(utl_log_file);
  utl_log_file = NULL;
  return ret;
}

FILE *utl_log_open(const char *fname, const char *mode)
{
  char md[4];
  md[0] = (mode && *mode == 'w')? 'w' : 'a';
  md[1] = '+';
  md[2] = '\0';
  utl_log_close(NULL);
  utl_log_file = fopen(fname,md);
  logprintf("LOG START");
  utl_log_check_num = 0;
  utl_log_check_fail = 0;
  return utl_log_file;
}

int utl_log_time(void)
{
  char       log_tstr[32];
  time_t     log_time;
  struct tm *log_time_tm;
  int        ret = 0;
  
  if (!utl_log_file) utl_log_file = stderr;
  if (time(&log_time) == ((time_t)-1)) ret = -1;
  if (ret >= 0 && !(log_time_tm = localtime(&log_time))) ret = -1;
  if (ret >= 0 && !strftime(log_tstr,32,"%Y-%m-%d %H:%M:%S",log_time_tm)) ret =-1;
  if (ret >= 0) ret = fprintf(utl_log_file,"%s ",log_tstr);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  
  return ret;
}

int utl_log_check(int res, const char *test, const char *file, int32_t line)
{
  int ret = 0;
  ret = utl_log_time();
  
  if (ret >= 0) ret = fprintf(utl_log_file,"CHK %s (%s)? %s:%d\n", (res?"PASS":"FAIL"), test, file, line);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  if (!res) utl_log_check_fail++;
  utl_log_check_num++;
  return res;
}

void utl_log_assert(int res, const char *test, const char *file, int32_t line)
{
  if (!utl_log_check(res,test,file,line)) {
    logprintf("CHK EXITING ON FAIL");
    logclose();
    abort();
  }
}

void utl_log_trc_check(char *buf, char *watch[], const char *file, int32_t line)
{ 
  int k=0;
  char *p;
  int expected = 0;
  int res = 0;
  _logprintf("XXX %s",buf);
  for (k=0; k<UTL_LOG_WATCH_SIZE; k++) {
    if (watch[k]) {
      p = watch[k];
      if (p[0] == '\0') break;
      expected = !((p[0] == '<') && (p[1] == 'n') && (p[2] == 'o') && (p[3] == 't') && (p[4] == '>'));
      if (!expected) p+=5;
    	res = pmxsearch(p,buf) != NULL;
      if (res) {
        utl_log_check(expected,watch[k],file,line);
        if (expected) watch[k] = NULL;
      }
    }
  }
}

void utl_log_trc_check_last(char *watch[], const char *file, int32_t line)
{ 
  /* The  only tests in `watch[]` should be the ones with `<not>` at the beginning */
  int k;
  int expected = 0;
  char *p;
  
  for (k=0; k<UTL_LOG_WATCH_SIZE;k++) {
    if (watch[k]) {
      p = watch[k];
      if (p[0] == '\0') break;
      expected = !((p[0] == '<') && (p[1] == 'n') && (p[2] == 'o') && (p[3] == 't') && (p[4] == '>'));
      utl_log_check(!expected,watch[k],file,line);
    }
  }
}

#endif
#endif
//>>>//
