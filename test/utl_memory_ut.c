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

int main (int argc, char *argv[])
{
  char *ptr_a;
  char *ptr_b;
  int valid;
  int k;
  
  TSTPLAN("utl unit test: memory check") {
    utlMemLog = logOpen("memory.log","w");
    logLevel(utlMemLog,"Info");

    TSTSECTION("malloc") {
      TSTGROUP("malloc") {
        TSTCODE { ptr_a = malloc(32); }
        TSTEQINT("Check Valid after malloc()",utlMemValid,utlMemCheck(ptr_a) );
        TSTEQINT("Allocated memory is 32",32, utlMemAllocated);
        
        TSTCODE { free(ptr_a); }
        TSTNEQINT("Check invalid after free", utlMemValid, utlMemCheck(ptr_a));
        TSTEQINT("Allocated memory is 0",0,utlMemAllocated);
        
        TSTCODE { free(ptr_a); }
        TSTNEQINT("Check invalid again",utlMemValid,utlMemCheck(ptr_a));
        
        TSTCODE { ptr_a = malloc(0); }
        TSTEQINT("Check Valid after malloc(0)",utlMemValid,utlMemCheck(ptr_a) );
        TSTCODE { free(ptr_a); }
      }
      
      TSTGROUP("calloc") {
        TSTCODE { ptr_a = calloc(8,4); }
        TSTEQINT("Allocated 8x4 ", utlMemValid,utlMemCheck(ptr_a) );
        TSTCODE {for (k=0,valid=0; k<32; k++) valid += ptr_a[k];} 
        TSTEQINT("Memory is clear",0, valid);
        TSTCODE { free(ptr_a); }
      }
      
      TSTGROUP("overrun memory") {
        TSTCODE { ptr_a = malloc(16);  }
        TSTEQINT("Check Valid after malloc()",utlMemValid,utlMemCheck(ptr_a));
        TSTEQINT("Allocated memory is 16",16,utlMemAllocated);
        
        TSTCODE { ptr_a[16] = '\0'; }
        TSTEQINT("Check invalid after overrun",utlMemOverflow ,utlMemCheck(ptr_a) );
        TSTCODE { free(ptr_a); }
      
      }
    }
    TSTNOTE("Check the file 'memory.log' to see the log of traced allocations");
  }
  
  logClose(utlMemLog);
}