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

#define UTL_MEMCHECK
#define UTL_MAIN
#include "utl_single.h"

/* DISCLAIMER malloc() (and friends) are casted just to allow the code to
   be compiled by a C++ compiler.
*/

int main (int argc, char *argv[])
{
  char *ptr_a;
  // char *ptr_b;
  int valid;
  int k;
 
  logopen("l_mem2.log","w");
  
 
  ptr_a = (char *)malloc(32); 
  logcheck(memVALID == memcheck(ptr_a));
  logcheck(32 == memused());
  
  free(ptr_a);
  logcheck(memVALID != memcheck(ptr_a));
  logcheck(0 == memused());
  
  free(ptr_a);
  logcheck(memVALID != memcheck(ptr_a));
  
  ptr_a = (char*)malloc(0);
  logcheck(memVALID == memcheck(ptr_a));
  
  free(ptr_a); 
  ptr_a = (char*)calloc(8,4);
  logcheck(memVALID == memcheck(ptr_a) );
  
  for (k=0,valid=0; k<32; k++) valid += ptr_a[k];
  logcheck(0 == valid);
  free(ptr_a); 
  
  ptr_a = (char*)malloc(16);  
  logcheck(memVALID == memcheck(ptr_a));
  logcheck(16 == memused());
  
  ptr_a[16] = '\0'; 
  logcheck(memOVERFLOW == memcheck(ptr_a) );
  free(ptr_a);   

  logclose();
  return 0;
}
