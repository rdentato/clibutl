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

/* 
   S =  (('+'|'-')? V)+
   V = upper+ | digit+
*/

pegrule(S) {
  pegmore {
    pegref(SPC);
    pegaction(op) {
      pegopt{pegoneof("+-");}
    }
    pegref(SPC);
    pegaction(arg) {
      pegref(V);
    }
  }
}

pegrule(V){
  pegref(SPC);
  pegalt {
    pegor{ pegmore {pegupper;}}
    pegor{ pegmore {pegdigit;}}
  }  
}

pegrule(SPC) {
  pegstar{pegoneof(" \t");}
}


int op(const char *from, const char *to, void *aux)
{
  fprintf(stderr,"op: %c",*from);
  return 0;
}

int arg(const char *from, const char *to, void *aux)
{
  fprintf(stderr,"arg: %.*s",(int)(to-from),from);
  return 0;
}

int main(int argc, char *argv[])
{
  char *q;
  peg_t pg;
  
  logopen("l_peg.log","w");
  loglevel("*");
  
  pg = pegnew();
  
  q = "ABCDE+FEG-43+AA";
  
  pg = pegfree(pg);
  logclose();
  exit(0);
}
