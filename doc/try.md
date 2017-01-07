# Exceptions

 Simple implementation of try/catch. try blocks can be nested.
 Exceptions are just integers > 0:
 
    #define OUTOFMEM   100
    #define WRONGINPUT 200
    
    try {
       ... code ...
       if (something_failed) throw(execption_num)  // must be > 0 
       some_other_func(); // you can trhow exceptions from other functions too 
       ... code ...
    }  
    catch(OUTOFMEM) {
       ... code ...
    }
    catch(WRONGINPUT) {
       ... code ...
    }
    catchall {  // if not present, the exception will be ignored
       ... code ...
    }

# Examples

  The usage is simple and it's explained in the  
[test program](https://github.com/rdentato/clibutl/blob/master/test/ut_try.c)
  
   