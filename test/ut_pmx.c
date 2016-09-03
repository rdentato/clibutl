#define UTL_MAIN
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

int main(int argc, char *argv[])
{
  char *s;
  char *p;
  char *q;
  
  logopen("t_pmx.log","w");
 
  p = "aersrx";
  
  s = pmxsearch("a",p);
  logcheck(s==p);
  
  s = pmxsearch("r",p);
  logcheck(s==p+2);

  s = pmxsearch("<!e>r",p);
  if (!logcheck(s && s==p+3)) logprintf("matched at %lu",s-p);
  
  p = "x    tar";
  s = pmxsearch("<+s>",p);
  logcheck(s && pmxcount()==1);

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
   
  q="F#";   s=pmxsearch(p,q);  prtnote(q);
  q="G";     s=pmxsearch(p,q);  prtnote(q);
  q="Fbq";    s=pmxsearch(p,q);  prtnote(q);
  q="Cq3";    s=pmxsearch(p,q);  prtnote(q);
  q="Cbbq/2";  s=pmxsearch(p,q);  prtnote(q);
  
  s=pmxsearch(p,"H");
  logcheck(!s);
  
  logclose();
  exit(0);
}