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

pegrule (testalt) {
  pegalt {
    pegeither {
      pegaction (countA1) {
        pegstar { pegstr("A"); }
      }
      pegaction (countB) {
        pegstr("B");
      }
    }
    pegor {
      pegaction (countA2) {
        pegstar { pegstr("A"); }
      }
      pegaction (countX) {
        pegstr("X");
      }
    }
  }
}

int countA1(const char *from, const char *to, void *aux)
{
  logprintf("CountA1: %ld",(long int)(to-from));
  return 0;
}

int countA2(const char *from, const char *to, void *aux)
{
  logprintf("CountA2: %ld",(long int)(to-from));
  return 0;
}
int countB(const char *from, const char *to, void *aux)
{
  logprintf("CountB: %ld",(long int)(to-from));
  return 0;
}

int countX(const char *from, const char *to, void *aux)
{
  logprintf("CountX: %ld",(long int)(to-from));
  return 0;
}

int main(int argc, char *argv[])
{
  //const char *q;
  //const char *p;
//  int ret;
  peg_t pg;
  
  logopen("l_peg3.log","w");
  loglevel("*");
  
  logcheck((pg = pegnew())); 
  
  logcheck(pegparse(pg,testalt,"AAAB"));
  logcheck(pegparse(pg,testalt,"AAAAAAX"));
  
  pg = pegfree(pg);
  logclose();

  exit(0);
}
