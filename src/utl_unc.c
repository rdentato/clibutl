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
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFSIZE 1024
static char buf[BUFSIZE];
int main(int argc, char *argv[])
{
  int prt=0;
  int ln=0;
  int k;
  FILE *f;
  for (k=1; k<argc; k++) {
    if ((f=fopen(argv[k],"r")) != NULL) {
      ln = 1;
      prt= 0;
      while (fgets(buf,BUFSIZE,f)) {
        ln++;
        if (strncmp(buf,"//>>>//",7) == 0) prt = 0;
        if (prt) fputs(buf,stdout);    
        if (strncmp(buf,"//<<<//",7) == 0) {
          fprintf(stdout,"#line %d \"%s\"\n",ln,argv[k]);
          prt = 1;
        }
      }
      fclose(f);
    }
  }
  exit(0);
}
