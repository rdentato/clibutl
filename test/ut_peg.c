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


  /*
  expr   := term ('+' | '-') expr | term
  term   := fact ('*' | '/') term | fact
  fact   := '(' expr ')' | num
  */
pegrule(expr) {
  pegswitch {
    pegcase {
      pegref(term);
      pegaction(op) {
        pegoneof("+-");
      }
      pegref(expr);
    }
    pegcase {
      pegref(term);
    }
  }
}

pegrule(term) {
  pegswitch {
    pegcase {
      pegref(fact);
      pegaction(op) {
        pegoneof("*/");
      }
      pegref(term);
    }
    pegcase {
      pegref(fact);
    }
  }
}

pegrule(fact) {
  pegswitch {
    pegcase {
      pegaction(PUSH);
      pegstr("("); pegref(expr); pegstr(")");
      pegaction(POP);
    }
    pegcase {
      pegaction(arg) {
        pegstar{ pegdigit; } 
      }
    }
  }
}


int main(int argc, char *argv[])
{
  char *q;
  peg_t pg;
  
  logopen("l_peg.log","w");
  pg = pegnew();
  
  q = "ABCDE+FEG-43+AA";
  fprintf(stderr,"\nexpr: '%s'\n",q);
  logcheck(pegparse(pg,S,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  logprintf("match: %.*s",(int)(pg->pos - q),q);

  q = "ABCDE + FEG - 43+AA";
  fprintf(stderr,"\nexpr: '%s'\n",q);
  logcheck(pegparse(pg,S,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  logprintf("match: %.*s",(int)(pg->pos - q),q);
  
  fprintf(stderr,"\n****\n");
  char *e[] = {"3+2","3-4*2","(3-2)*(4/2)",NULL};
  char **qe;
  qe = e; 
  for (q = *qe; q; q=*(++qe)) {
    fprintf(stderr,"\n%s\n",q);
    logcheck(pegparse(pg,expr,q));
    logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
    logprintf("match: %.*s",(int)(pg->pos - q),q);
  }
  
  pg = pegfree(pg);
  logclose();
  exit(0);
}
