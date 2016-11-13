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
  logopen("l_logassert.log","w");
  
  utl_log_check_fail--; /* DIRTY TRICK */
  logassert(5>10);
  utl_log_check_fail=0; /* SHOULD NEVER HAPPEN */
  logassert(utl_ret(0));
  logcheck(3/utl_ret(0) == 0);
  logclose();
}
