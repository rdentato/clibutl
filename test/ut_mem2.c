/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_MEMCHECK
#define UTL_MAIN
#include "utl_single.h"


int main (int argc, char *argv[])
{
  char *ptr_a;
  // char *ptr_b;
  int valid;
  int k;
 
  logopen("t_mem.log","w");
  
 
  ptr_a = malloc(32); 
  logcheck(memVALID == memcheck(ptr_a));
  logcheck(32 == memused());
  
  free(ptr_a);
  logcheck(memVALID != memcheck(ptr_a));
  logcheck(0 == memused());
  
  free(ptr_a);
  logcheck(memVALID != memcheck(ptr_a));
  
  ptr_a = malloc(0);
  logcheck(memVALID == memcheck(ptr_a));
  
  free(ptr_a); 
  ptr_a = calloc(8,4);
  logcheck(memVALID == memcheck(ptr_a) );
  
  for (k=0,valid=0; k<32; k++) valid += ptr_a[k];
  logcheck(0 == valid);
  free(ptr_a); 
  
  ptr_a = malloc(16);  
  logcheck(memVALID == memcheck(ptr_a));
  logcheck(16 == memused());
  
  ptr_a[16] = '\0'; 
  logcheck(memOVERFLOW == memcheck(ptr_a) );
  free(ptr_a);   

  logclose();
  return 0;
}