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
#if 0
/* 
   S = upper* digit
*/

pegrule(S1) {
  pegdo(prt) {
    pegmore { pegupper; }
    pegdigit;
  }
}

pegrule(S2) {
  pegmore {
    pegdo(prt) {
      pegupper;
    }
  }
  pegdo(prt) {
    pegdigit;
  }
}

/*
   S = upper S | digit
   
*/

pegrule(S3) {
  pegdo(prt) {
    pegalt { 
      pegor {
        pegupper;
        pegref(S3);
      }
      pegor {
        pegdigit;
      }
    }
  }
}

pegrule(S4) {
  pegalt{
    pegor {
      pegupper;
      pegdo(prt) {
        pegref(S4);
      }
    }
    pegor {
      pegdigit;
    }
  }
}

pegrule(S5) {
  // PIPPO | PLUTO
  pegstr("P");
  pegalt {
    pegor{ pegstr("IPPO"); }
    pegor{ pegstr("LUTO"); }
  }
}

/*
   A = '(' A ')' | ('+'|'-')? B
   B = NUM
*/

pegrule(A) {
  pegalt {
    pegeither {
      pegstr("("); pegref(A); pegstr(")");
    }
    pegor {
      pegopt{
        pegoneof("+-");
      }
      pegref(B);
    }
  }
}

pegrule(B) {
  pegmore { pegdigit; }
}

int prt(const char *from, const char *to, void *aux)
{
  logprintf("(%.*s)",(int)(to-from),from);
  return 0;
}
#endif



int main(int argc, char *argv[])
{
#if 0  
  char *q;
  peg_t pg;
  
  logopen("l_peg2.log","w");
 
  pg = pegnew();
 
  q = "ABCD4";
  
  logcheck(pegparse(pg,S1,q));
  logcheck(*(pg->pos) == '\0');
  
  logcheck(pegparse(pg,S2,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);

  logcheck(pegparse(pg,S3,q));

  logcheck(pegparse(pg,S4,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  
  q = "ABCD";
  logcheck(!pegparse(pg,S2,q));
  logprintf("q:%p p:%p m:%p c:%d line:%d col:%d rule:%s ",q,pg->pos,pg->errpos,*pg->errpos,pg->errln, pg->errcn,pg->errrule);
  
  q = "PIPPO";
  logcheck(pegparse(pg,S5,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  
  q = "PLUTO";
  logcheck(pegparse(pg,S5,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  
  q = "PLUKA";
  logcheck(!pegparse(pg,S5,q));
  logprintf("q:%p p:%p m:%p c:%d line:%d col:%d rule:%s ",q,pg->pos,pg->errpos,*pg->errpos,pg->errln, pg->errcn,pg->errrule);
  
  q="34";
  logcheck(pegparse(pg,A,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  logprintf("match: %.*s",(int)(pg->pos - q),q);
  
  q="(34)";
  logcheck(pegparse(pg,A,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  logprintf("match: %.*s",(int)(pg->pos - q),q);
  
  q="(34)-43+65";
  logcheck(pegparse(pg,A,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  logprintf("match: %.*s",(int)(pg->pos - q),q);
  
  q="3+4";
  
  logcheck(pegparse(pg,expr,q));
  logprintf("q:%p p:%p m:%p c:%d",q,pg->pos,pg->errpos,*pg->errpos);
  logprintf("match: %.*s",(int)(pg->pos - q),q);

  fprintf(stderr,"\n****\n");
  char *e[] = {"3+2","3-4*2",NULL};
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
  #endif
  exit(0);
}
