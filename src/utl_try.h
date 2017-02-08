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
 Exceptions are integers between 0 and 15
 
  #define OUTOFMEM    1
  #define WRONGINPUT  2
  #define INTERNALERR 3
  
  try {
     ... code ...
     if (something_failed) throw(execption_num)  // must be > 0  and < 15
     some_other_func(); // you can trhow exceptions from other functions too 
     ... code ...
  }  
  catch(OUTOFMEM) {
     ... code ...
  }
  catch(WRONGINPUT,INTERNALERR) {
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
  const char       *fn;
  int               ln;
  int               ex;
  int16_t           flg;
  int16_t           id;
} utl_jb_t;

extern utl_jb_t *utl_jmp_list; // Defined in utl_hdr.c

#define UTL_TRY_INIT  {.flg=0, \
                       .prv=utl_jmp_list, \
                       .fn=utl_emptystring, \
                       .ln=0, \
                       .id=0}

#define try  for ( utl_jb_t utl_jb = UTL_TRY_INIT; \
                  !utl_jb.flg && (utl_jmp_list=&utl_jb); \
                   utl_jmp_list=utl_jb.prv, utl_jb.flg=1) \
              if ((utl_jb.ex = setjmp(utl_jb.jmp)) == 0)

#define UTL_CATCH_TEST(x,y,w,z) ((1<<(x))|(1<<(y))|(1<<(w))|(1<<(z)))

#define utl_catch(x,y,w,z) else if ( (utl_jb.ex  & UTL_CATCH_TEST(x,y,w,z)) \
                                  && (utl_jmp_list=utl_jb.prv, utl_jb.flg=1)) 

#define catch(...) utl_catch(utl_expand(utl_arg0(__VA_ARGS__,16)),\
                             utl_expand(utl_arg1(__VA_ARGS__,16,16)),\
                             utl_expand(utl_arg2(__VA_ARGS__,16,16,16)),\
                             utl_expand(utl_arg3(__VA_ARGS__,16,16,16,16)))

#define catchall else for ( utl_jmp_list=utl_jb.prv; \
                           !utl_jb.flg; \
                            utl_jb.flg=1) 

#define utl_throw(x,y)  do { \
                          int ex_ = x; \
                          if (ex_ > 0 && utl_jmp_list) {\
                            utl_jmp_list->fn = __FILE__; \
                            utl_jmp_list->ln = __LINE__;\
                            utl_jmp_list->id = y;\
                            longjmp(utl_jmp_list->jmp,ex_); \
                          }\
                        } while (0)

#define throw(...) utl_throw(1<<(utl_expand(utl_arg0(__VA_ARGS__,0)) & 0xF),\
                                 utl_expand(utl_arg1(__VA_ARGS__,0,0)))\

#define rethrow()    utl_throw(utl_jb.ex,utl_jb.id)

#define thrown()     utl_unpow2(utl_jb.ex)

#define thrownid()   utl_jb.id
#define thrownfile() utl_jb.fn
#define thrownline() utl_jb.ln

#endif
//>>>//