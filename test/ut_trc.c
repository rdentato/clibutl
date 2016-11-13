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
*/

#include "utl.h"


int main(int argc, char *argv[])
{
  logopen("l_trc.log","w");
  
  logtracewatch("pluto") {
    logtrace("pluto");
  }

  logtracewatch("event<*s>1", "event2") {
    logtrace("event 1");
    logtrace("event 2");
    logtrace("event 1");
  }
  
  logtracewatch("pippo", "<not>pluto") {
    logtrace("pluto");
    logtrace("pippo");
  }
  /* this is to recognize that 2 fails were expected */
  if (utl_log_check_fail >= 2) utl_log_check_fail -= 2;
  logclose();
  exit(0);
}
