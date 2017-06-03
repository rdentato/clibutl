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

void prtnote(char *s)
{
  /* "^(<=A-G>)(<?=b#>)(<?=whqest>)(<*d>)(/<+d>|)" */   
  if (pmxcount() == 0)   logprintf("'%-6s' no note",s);
  else logprintf("'%-6s' Pitch:%.*s Accid:%.*s Len:%.*s Mul:%.*s Div:%.*s",s,
            pmxlen(1),pmxstart(1),
            pmxlen(2),pmxstart(2),
            pmxlen(3),pmxstart(3),
            pmxlen(4),pmxstart(4),
            pmxlen(5),pmxstart(5)
           );
}

int trc(const char *from, const char *to, void *aux)
{
  logtrace("Scan: %c",*from);
  return 0;
}

int main(int argc, char *argv[])
{
  const char *s;
  
  logopen("l_pmx.log","w");
  loglevel("I,T");
  
  s=pmxsearch("田","電田説");
  logcheck(s);
  
  s=pmxsearch("(田|","電田説");
  logcheck(s);

  s=pmxsearch("<utf>えxもa","電田説モ");
  logcheck(!s);
  
  s=pmxsearch("<utf>田(もa|)","電田説モ");
  logcheck(s);
  
  s=pmxsearch("<iso>(<.>)a","電a");
  logcheck(s && pmxlen(1) == 1);

  s=pmxsearch("<utf>(<.>)a","電a");
  logcheck(s && pmxlen(1) == 3);

  s=pmxsearch("<+.>","レモ デンタト");
  logcheck(s);

  s=pmxsearch("<utf><+.>","レモ デンタト");
  logcheck(s);

  s=pmxsearch("<+.>","z電");
  logcheck(s);
  
  s=pmxsearch("<utf>電田説モ","電田説モ");
  logcheck(s);
  
  s=pmxsearch("<+.>","z電");
  logcheck(s);

  logwatch ("A","B","C","!<d>") {
    s = pmxscan("<u>","1B2A3Y4",trc,NULL);
  }
  
  logclose();
  exit(0);
}
