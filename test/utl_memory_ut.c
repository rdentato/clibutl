/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This sofwtare is distributed under the terms of the BSD license:
**   http://creativecommons.org/licenses/BSD/
**   http://opensource.org/licenses/bsd-license.php 
*/

#define UTL_C
#define UTL_MEMCHECK
#define UTL_UNITTEST

#include "utl.h"

#define lg utlLog

int main (int argc, char *argv[])
{
  char *ptr_a;
  // char *ptr_b;
  int valid;
  int k;
  
  logTestPlan(lg,"utl unit test: memory check") {
  
    logLevel(lg,"Info");
    
    ptr_a = malloc(32); 
    logEQint(lg,utlMemValid,utlMemCheck(ptr_a) );
    logEQint(lg,32, utlMemAllocated);
    
    free(ptr_a);
    logNEint(lg, utlMemValid, utlMemCheck(ptr_a));
    logEQint(lg,0,utlMemAllocated);
    
    free(ptr_a);
    logNEint(lg,utlMemValid,utlMemCheck(ptr_a));
    
    ptr_a = malloc(0);
    logEQint(lg,utlMemValid,utlMemCheck(ptr_a) );
    
    free(ptr_a); 
    ptr_a = calloc(8,4);
    logEQint(lg, utlMemValid,utlMemCheck(ptr_a) );
    
    for (k=0,valid=0; k<32; k++) valid += ptr_a[k];
    logEQint(lg,0, valid);
    free(ptr_a); 
    
    
    ptr_a = malloc(16);  
    logEQint(lg,utlMemValid,utlMemCheck(ptr_a));
    logEQint(lg,16,utlMemAllocated);
    
    ptr_a[16] = '\0'; 
    logEQint(lg,utlMemOverflow ,utlMemCheck(ptr_a) );
    free(ptr_a);   
  }
  return 0;
}