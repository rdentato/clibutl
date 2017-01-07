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


** ]]] */

//<<<//

#ifndef UTL_NOTRY

typedef struct utl_jmp_buf_s {
  jmp_buf jmp;
  int     err;
  struct utl_jmp_buf_s *prev;
} utl_jmp_buf;

extern utl_jmp_buf *utl_jmp_list; // Defined in utl_hdr.c

#define try  for (utl_jmp_buf utl_jmp = {.err=0, .prev = utl_jmp_list}; \
                  (utl_jmp_list = &utl_jmp) && utl_jmp.err >= 0; \
                  utl_jmp_list = utl_jmp.prev, utl_jmp.err = -1) \
              if ((utl_jmp.err = setjmp(utl_jmp.jmp))== 0)
                 
#define catch(x) else if (utl_jmp.err == (x)) 
  
#define catchall else
                 
#define throw(x) do {int x_ = (x); if (x_ && utl_jmp_list) longjmp(utl_jmp_list->jmp,x_); } while (0)

#endif
//>>>//