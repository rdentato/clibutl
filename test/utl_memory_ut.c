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

#define lg logStderr

int main (int argc, char *argv[])
{
  char *ptr_a;
  // char *ptr_b;
  int valid;
  int k;
  
  utlMemLog = lg;
  
  logTestPlan(lg,"utl unit test: memory check") {
  
    logLevel(utlMemLog,"Info");
    
    ptr_a = malloc(32); 
    logTestEQint(lg,"Check Valid after malloc()",utlMemValid,utlMemCheck(ptr_a) );
    logTestEQint(lg,"Allocated memory is 32",32, utlMemAllocated);
    
    free(ptr_a);
    logTestNEint(lg,"Check invalid after free", utlMemValid, utlMemCheck(ptr_a));
    logTestEQint(lg,"Allocated memory is 0",0,utlMemAllocated);
    
    free(ptr_a);
    logTestNEint(lg,"Check invalid again",utlMemValid,utlMemCheck(ptr_a));
    
    ptr_a = malloc(0);
    logTestEQint(lg,"Check Valid after malloc(0)",utlMemValid,utlMemCheck(ptr_a) );
    
    free(ptr_a); 
    ptr_a = calloc(8,4);
    logTestEQint(lg,"Allocated 8x4 ", utlMemValid,utlMemCheck(ptr_a) );
    
    for (k=0,valid=0; k<32; k++) valid += ptr_a[k];
    logTestEQint(lg,"Memory is clear",0, valid);
    free(ptr_a); 
    
    
    ptr_a = malloc(16);  
    logTestEQint(lg,"Check Valid after malloc()",utlMemValid,utlMemCheck(ptr_a));
    logTestEQint(lg,"Allocated memory is 16",16,utlMemAllocated);
    
    ptr_a[16] = '\0'; 
    logTestEQint(lg,"Check invalid after overrun",utlMemOverflow ,utlMemCheck(ptr_a) );
    free(ptr_a);   
  }
  return 0;
}