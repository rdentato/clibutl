#include "utl.h"

int isvowel(char *pat,char *txt, int len, int32_t ch)
{
  
  char *w1 = "AEIOUaeiou";
  /* Unicode code points */
  char *w2 = "\xC0\xC1\xC2\xC3\xC4\xC5\xC6\xC8\xC9\xCA\xCB\xCC\xCD\xCE\xCF"
             "\xD1\xD2\xD3\xD4\xD5\xD6\xD8\xD9\xDA\xDB\xDC\xE0\xE1\xE2\xE3"
             "\xE4\xE5\xE6\xE8\xE9\xEA\xEB\xEC\xED\xEE\xEF\xF2\xF3\xF4\xF5"
             "\xF6\xF8\xF9\xFA\xFB\xFC";
  
  switch (len) {
    case 1 :   if (!strchr(w1,ch)) len = 0;  break;
    case 2 :   if (!strchr(w2,ch)) len = 0;  break;  /* utf8*/
    default:   len = 0;
  }
  return len;
}

int iscons(char *pat,char *txt, int len, int32_t ch)
{
  
  char *w1 = "BCDFGHJKLMNPQRSTVWXYZ" 
             "bcdfghjklmnpqrstvwxyz" ;
  /* Unicode code points */
  char *w2 = "\xC7\xD0\xD1\xDD\xDE\xE7\xF0\xF1\xFD\xFE\xFF";
  
  switch (len) {
    case 1 :   if (!strchr(w1,ch)) len = 0;  break;
    case 2 :   if (!strchr(w2,ch)) len = 0;  break;  /* utf8*/
    default:   len = 0;
  }
  return len;
}

int isid(char *pat,char *txt, int len, int32_t ch)
{
  char *id = txt;
  char *c = "0123456789_ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  
  if (strchr(c+10,*id)) {
    do {
      id++;
    } while (strchr(c,*id));
  }
  return id-txt;
}

int isprime(char *pat,char *txt, int len, int32_t ch)
{
  long int n;
  char *s=txt;
  uint8_t p[] = {  2,   3,   5,   7,  11,  13,  17,  19,  23,  29,  31,  37,  41,  43,
                  47,  53,  59,  61,  67,  71,  73,  79,  83,  89,  97, 101, 103, 107,
                 109, 113, 127, 131, 137, 139, 149, 151, 157, 163, 167, 173, 179, 181,
                 191, 193, 197, 199, 0};
  
  n = strtol(txt,&s,10);
  if (n == 0 || n >= 200) return 0;
  if (!strchr((char*)p,n)) return 0;
  logdebug("prime: %p %p %s",txt,s,txt);
  return (s-txt);             
}

int isladder(char *pat,char *txt, int len, int32_t ch)
{
  char *s = txt;
  
  while (s[0] > s[1]) s++;
  return 0;
}

int isCV(char *pat,char *txt, int len, int32_t ch)
{
  if (*pat == 'C') return iscons(pat,txt,len,ch);
  if (*pat == 'V') return isvowel(pat,txt,len,ch);
  if (*pat == 'I') return isid(pat,txt,len,ch);
  if (*pat == 'P') return isprime(pat,txt,len,ch);
  return 0;
}


int main(int argc, char *argv[])
{
  char *s;
  char *p;
/*  char *q;
  */
  logopen("l_pmx4.log","w");
 
  s = pmxsearch("x<+:>y","xaey");
  logcheck(!s);
  
  pmxextend(isvowel);
  s = pmxsearch("x<+:>y","xaey");
  logcheck(s);
  
  s = pmxsearch("<utf>x<+:>y","xaèey");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  pmxextend(isCV);
  s = pmxsearch("x<+:V>y","xaey");
  logcheck(s);
  
  s = pmxsearch("x<+:V>y","xaèey");  /* We are already in UTF-8 mode */
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("a<+:C>e","ae");
  logcheck(!s);

  s = pmxsearch("a<+:C>e","agtre");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("a<+:C>e","axçþe33");  /* We are already in UTF-8 mode */
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("<iso>","");
  
  s = pmxsearch("<:I>"," = 3+4");
  logcheck(!s);

  s = pmxsearch("<:I>"," = 3+fn(x)");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("<B>","asds");
  logcheck(!s);
  
  s = pmxsearch("<B>","af(23)ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<B>","af(2(763))ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<B>","af(2(763)ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<B>","af(2(763)))ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<B>","af{23}ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<B>","af{2{763}}ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<B>","af<23>ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}
  
  s = pmxsearch("<B<>>","af{2<763>}ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}
  
  s = pmxsearch("<utf><B«»>","af{2«763»}ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}
  
  s = pmxsearch("<utf><B>","af{2«763»}ds");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}
  
  s = pmxsearch("<Q>","x'pippo'");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}

  s = pmxsearch("<utf><Q>","x“pippo”");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}
    
  p = "33";
  s = pmxsearch("<:P>","33");
  logcheck(s && s==p+1);

  s = pmxsearch("<:P>","31");
  if (logcheck(s)) {logprintf("MATCH: [%.*s] (err:%s)",pmxlen(0),pmxstart(0),pmxerror());}
    
  

  
  logclose();
  exit(0);
}
