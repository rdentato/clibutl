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

typedef struct utl_jb_s {
  jmp_buf           jmp;
  struct utl_jb_s  *prv;
  int               err;
  int32_t           flg;
} utl_jb_t;

extern utl_jb_t *utl_jmp_list; // Defined in utl_hdr.c

#define try  for ( utl_jb_t utl_jb = {.flg=0, .err=0, .prv=utl_jmp_list}; \
                   !utl_jb.flg && (utl_jb.flg=1) && (utl_jmp_list=&utl_jb); \
                   utl_jb.flg = utl_jb.flg == 1? (utl_jmp_list = utl_jb.prv, 1):1) \
              if ((utl_jb.err = setjmp(utl_jb.jmp))== 0)
                 
#define catch(x) else if ((utl_jb.err == (x)) && (utl_jmp_list = utl_jb.prv, utl_jb.flg = 2)) 
  
#define catchall else if ((utl_jb.err > 0) && (utl_jmp_list = utl_jb.prv, utl_jb.flg = 2)) 
                 
#define throw(x)  do {int ex_ = x; if (ex_ > 0 && utl_jmp_list) longjmp(utl_jmp_list->jmp,ex_); } while (0)

#define thrown()  utl_jb.err
  
#define rethrow() throw(thrown())

#endif
//>>>//