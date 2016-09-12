/* 
**  (C) 2014 by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
**     
**                     ___   __
**                  __/  /_ /  )
**          ___  __(_   ___)  /
**         /  / /  )/  /  /  /  Minimalist
**        /  /_/  //  (__/  /  C utility 
**       (____,__/(_____(__/  Library
** 
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024
char buf[BUFSIZE];
int main(int argc, char *argv[])
{
  int prt=0;
  int k;
  FILE *f;
  for (k=1; k<argc; k++) {
    if ((f=fopen(argv[k],"r"))) {
      while (fgets(buf,BUFSIZE,f)) {
        if (strncmp(buf,"//>>>//",7) == 0) prt = 0;
        if (prt) fputs(buf,stdout);    
        if (strncmp(buf,"//<<<//",7) == 0) prt = 1;
      }
      fclose(f);
    }
  }
  exit(0);
}