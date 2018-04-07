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

#define UTL_NET
#include "utl.h"

 
int handlemsg(char *msg, int len)
{
  if (strncmp("ZERO",msg,4) == 0) return 0;
  
  if (strncmp("BYE",msg,3))  {
    sprintf(msg,"200 OK\r\n");
    return 8;
  }

  return -1;
}


int main(int argc, char *argv[])
{
  
  logopen("l_srv.log","w");
  loglevel("*");
  
  utlserver(12345,handlemsg);
  
  logclose();
  exit(0);
}
