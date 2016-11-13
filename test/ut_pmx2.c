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

void prtnote(const char *s)
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

int main(int argc, char *argv[])
{
  const char *s;
  const char *p;
  const char *q;
  
  logopen("l_pmx2.log","w");
 
  p = "AaBer33444sr3x";
 
  s = pmxsearch("e",p);
  logcheck(s);
 
  s = pmxsearch("<l>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
 
  s = pmxsearch("<l><+l>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
 
  s = pmxsearch("<l><.><l>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
 
  s = pmxsearch("<+=b-z34>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  
  
  p = "Aa電ersrx";
  s = pmxsearch("<utf><l><.><l>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s = pmxsearch("<utf><+=b-z>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s = pmxsearch("<utf><=è-ë>","eé");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s = pmxsearch("<utf><+!=è-ëà>","eòùé");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s = pmxsearch("r",p);
  logcheck(s==p+6);

  s = pmxsearch("<!=e>r",p);
  if (!logcheck(s && s==p+7)) logprintf("matched at %lu",(unsigned long)(s-p));
 
  
  p = "x    tar";
  s = pmxsearch("<+s>",p);
  logcheck(s);
  if (!logcheck(pmxcount()==1)) {
    logprintf("count: %d",pmxcount());
  }

  s = pmxsearch("<+s>(<*l>)",p);
  logcheck(s && pmxcount()==2);
  logcheck(pmxstart(0)==p+1);
  logcheck(pmxstart(1)==p+5);
  
  p = "x    ";
  s = pmxsearch("<+s>(<*l>)",p);
  logcheck(s && pmxcount()==2);
  logcheck(pmxstart(0)==p+1);
  logcheck(pmxstart(1)==p+5);
  
  s = pmxsearch("<s><$>",p);
  logcheck(s && pmxcount()==1);
  logcheck(pmxstart(0)==p+4);
  
  s = pmxsearch("<s>(<s><$>)",p);
  logcheck(s && pmxcount()==2);
  logcheck(pmxstart(1)==p+4);
  
  p = "a <i>  ";
  s = pmxsearch("%>",p);
  logcheck(s && pmxcount()==1);
  logcheck(pmxstart(0)==p+4);

  p = "abc2b34qwe";
  s = pmxsearch("b|3",p);
  logcheck(s && pmxcount()==1);
  logcheck(pmxstart(0)==p+1);

  s = pmxsearch("3|b",p);
  logcheck(s && pmxcount()==1);
  logcheck(pmxstart(0)==p+1);
  logcheck(pmxend(0)==p+2);
 
  s = pmxsearch("b(<l>|<d>)",p);
  logcheck(s);
  logcheck(strncmp(s,"bc",2)==0);

  s = pmxsearch("b(<l>|<+d>)",s+1);
  logcheck(s);
  if(!logcheck(strncmp(s,"b34",3)==0)) {
    logprintf("%.*s",(int)pmxlen(0),pmxstart(0));
  }

  p ="17cm";
  s = pmxsearch("<+d>(cm|in|)",p);
  logcheck(s);
  logcheck(strncmp(s,"17cm",4)==0);

  p ="17xxcm";
  s = pmxsearch("<+d>(cm|in|)",p);
  logcheck(s);
  logcheck(strncmp(s,"17",2)==0);
  
  s = pmxsearch("<+d>(cm|in)",p);
  logcheck(!s);
  
  p ="17in";
  s = pmxsearch("<+d>(cm|in|)",p);
  logcheck(s);
  logcheck(strncmp(pmxstart(1),"in",pmxlen(1))==0);

  
  p = "1234abcde3123";
  s = pmxsearch("<l><*d><l>",p);
  logcheck(s);
  logcheck(strncmp(s,"ab",2)==0);
  
  s = pmxsearch("<d><*l><d>",p);
  logcheck(s);
  logcheck(strncmp(s,"12",2)==0);

  s = pmxsearch("<d>((<+l>)<d>)",p);
  logcheck(s);
  logcheck(strncmp(s,"4abcde3",7)==0);
  logcheck(strncmp(pmxstart(1),"abcde3",pmxlen(1))==0);

  p="^(<=A-G>)(<*=b#>)(<?=whqest>)(<*d>)(/<+d>|)";
   
  q="F#";      s=pmxsearch(p,q);  logcheck(s); prtnote(q);
  q="G";       s=pmxsearch(p,q);  logcheck(s); prtnote(q);
  q="Fbq";     s=pmxsearch(p,q);  logcheck(s); prtnote(q);
  q="Cq3";     s=pmxsearch(p,q);  logcheck(s); prtnote(q);
  q="Cbbq/2";  s=pmxsearch(p,q);  logcheck(s); prtnote(q);
  
  s=pmxsearch(p,"H");
  logcheck(!s);
  
  p="ABho";
  s=pmxsearch("<+!u>",p);
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
 
  s=pmxsearch("<iso>(a|(b c)|(d))","d");
  logcheck(s);
  logtrace("matched: %d (%.*s) (%.*s) (%.*s) (%.*s)",pmxcount(),pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1),pmxlen(2),pmxstart(2),pmxlen(3),pmxstart(3));

  s=pmxsearch("(a|b|c|d)","d");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("é","aèbéc");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("田","電田説");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("(田|","電田説");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("<utf>えxもa","電田説モ");
  logcheck(!s);
  
  s=pmxsearch("<utf>田(もa|)","電田説モ");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<iso>(<.>)a","電a");
  logcheck(s && pmxlen(1) == 1);

  s=pmxsearch("<utf>(<.>)a","電a");
  logcheck(s && pmxlen(1) == 3);

  s=pmxsearch("<+.>","レモ デンタト");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("<utf><+.>","レモ デンタト");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("<+.>","z電");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<utf>電田説モ","電田説モ");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
 
  s=pmxsearch("<+#41-4F>","abcABC");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("<utf><+#E0 F2>","aàòb");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }

  s=pmxsearch("<N>","ab\ncd");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<l><+N><l>","ab\n\r\ncd");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<l><*N><l>","ab\n\r\ncd");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<3l>","AbcDef");
  logcheck(!s);
  
  s=pmxsearch("<3l>","Abcdef");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<3-l>","Abcdef");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<3l>","AbcDef");
  logcheck(!s);
  
  s=pmxsearch("<3-4l>","Abcdef");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("<3-4l>","AbcDef");
  logcheck(!s);
  
  s=pmxsearch("<2-3l>","AbcDefg");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("a(b|(cd|CD))","acd");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("a(b|(cd|CD))","aCD");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  s=pmxsearch("a(b|(cd|CD))","acD");
  logcheck(!s);
  
  s=pmxsearch("a(b|(cd|CD|))","acD");
  if (logcheck(s)) {logprintf("matched: %.*s",pmxlen(0),pmxstart(0)); }
  
  logclose();
  exit(0);
}
