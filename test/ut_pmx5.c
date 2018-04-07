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


int main(int argc, char *argv[])
{
  const char *s;

  logopen("l_pmx5.log","w");
 
  s = pmxsearch("ABC","abc");
  logcheck(!s);
  
  s = pmxsearch("<utf><2u>","ES");
  logcheck(s);

  s = pmxsearch("A<I>BC","Abc");
  logcheck(s);
  
  s = pmxsearch("A<I>BC<C>d","Abcd");
  logcheck(s);
  
  s = pmxsearch("A<I>BC<C>d","AbcD");
  logcheck(!s);
  
  s = pmxsearch("<utf><2u>","ÈÆ");
  logcheck(s);

  s = pmxsearch("<utf><2a>","ÈÆ");
  logcheck(s);
      
  s = pmxsearch("<utf><2a>","èæ");
  logcheck(s);
      
  s = pmxsearch("<utf><2l>","èæ");
  logcheck(s);

  s = pmxsearch("<utf><I>èÆ","Èæ");
  logcheck(s);

  s = pmxmatch("<?>(a|b)c","ac");
  logcheck(s);

  s = pmxmatch("<?>(a|b)c","bc");
  logcheck(s);
  
  s = pmxmatch("<?>(a|b)c","c");
  logcheck(s);
  loginfo("%.*s",pmxlen(0),pmxstart(0));
  
  logclose();
  exit(0);
}
