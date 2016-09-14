#include "utl.h"

int main(int argc, char *argv[])
{
  char *s;
/*
  char *p;
  char *q;
  */
  logopen("t_pmx.log","w");
 
  s = pmxsearch("a|b","xa");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("a|b","xb");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
 
  s = pmxsearch("x(|a|b)<l>","xcd");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("x(|a|b)<l>","xad");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}

  s = pmxsearch("x(|a|b)<l>","xbd");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}
  
  s = pmxsearch("<d>(|in|cm)<2l>","--4px--");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("<d>(|in|cm)<2l>","--4in--");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}
 
  s = pmxsearch("<d>(|in|cm)<2l>","--4cm--");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}
 
  s = pmxsearch("<d><!>(in|cm)<2l>","--4px--");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s = pmxsearch("<d><!>(in|cm)<2l>","--4in--");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}
 
  s = pmxsearch("<d><!>(in|cm)<2l>","--4cm--");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}
 
  s = pmxsearch("<d>(|)<2l>","--4cm--");
  if (logcheck(!s)) {logprintf("NOMATCH: %d",pmxlen(1));}
 

  s = pmxsearch("<d>()<2l>","--4cm--");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}

  s = pmxsearch("<d>)<2l>","--4cm--");
  logcheck(s);
  
  logprintf("*************************");
  
  s= pmxsearch("<2>(a)","abcaad");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s= pmxsearch("<d><?>(cm|in)<l>","a3p");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}

  s= pmxsearch("<d><?>(cm|in)<l>","a3cmp");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s= pmxsearch("<d><?>(cm|in)<l>","a3inp");
  if (logcheck(s)) {logprintf("MATCH: %.*s",pmxlen(0),pmxstart(0));}
  
  s= pmxsearch("<d><?>(cm|in|<2l>)<l>","a3pxp");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}
  
  s= pmxsearch("<utf>«<*>(\\»|<!=»>)»","a«ABC»x");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}
  
  s= pmxsearch("<utf>«<*>(\\»|<!=»>)»","a«A\\»BC»x");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}
  
  s= pmxsearch("<utf>«<*>(\\»|<!=»>)»","a«»x");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}
  
  s= pmxsearch("(<l>)x<^1>","axa");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}
  
  s= pmxsearch("(<l>)x<^1>","axb");
  logcheck(!s);

  s= pmxsearch("(<l><d>)x<2^1>","a3xa3a3");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}

  s= pmxsearch("(<l><d>)x<2^1>","a3xa3");
  logcheck(!s);

  s= pmxsearch("(<l><d>)x<2^1>","a3xa3b3xb3b3");
  if (logcheck(s)) {logprintf("MATCH: %.*s (%.*s)",pmxlen(0),pmxstart(0),pmxlen(1),pmxstart(1));}

  s= pmxsearch("(<l>)x<^2>","a3xa3");
  logcheck(!s);

  logclose();
  exit(0);
}
