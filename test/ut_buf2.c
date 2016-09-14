#define UTL_MAIN
#include "utl_single.h"
#include <math.h>

#define TESTFILE "t_buf.tmp"
FILE *createtestfile()
{
  FILE *f;
  
  f=fopen(TESTFILE,"wb");
  if (!f) return 0;
  
  fprintf(f,"ABCDEF\nGHIJKL\n123456");
  fclose(f);
  f=fopen(TESTFILE,"rb");
  return f;
}

int main(int argc, char *argv[])
{
  
  buf_t b;
  char c;
  FILE *f;
  
  logopen("t_buf.log","w");
 
  
  b = bufnew();
  logassert(b);
  
  bufsetc(b,3,'a');
  c=bufgetc(b,3);
  logcheck (c=='a');
  
  logassert((f=createtestfile()));
  bufread(b,0,4,f);
  logcheck(buf(b)[4] == '\0');
  logcheck(strncmp(buf(b),"ABCD",4) == 0);
  
  bufreadall(b,buflen(b),f);
  logcheck(buflen(b) == 20);
  logassert(buf(b)[buflen(b)] == '\0');
  
  logprintf("buf=[%s]",buf(b));

  fseek(f,0,SEEK_SET);
  
  while (bufreadln(b,0,f)) {
    logprintf("[%s]",buf(b));
  }
  fclose(f);
  
  
  bufsets(b,0,"WXYZ");
  logcheck(bufgetc(b,4) == '\0');
  logcheck(buflen(b) == 4);
  logcheck(strcmp(buf(b),"WXYZ")==0);
  
  bufsets(b,2,"CDE");
  logcheck(bufgetc(b,5) == '\0');
  logcheck(buflen(b) == 5);
  logcheck(strcmp(buf(b),"WXCDE")==0);
  
  bufinsc(b,2,'A');
  logcheck(bufgetc(b,6) == '\0');
  logcheck(buflen(b) == 6);
  if (!logcheck(strcmp(buf(b),"WXACDE")==0)) {
    logprintf("   [%s]",buf(b));
  }

  bufinss(b,3,"]|[");
  logcheck(buflen(b) == 9);
  logcheck(strcmp(buf(b),"WXA]|[CDE")==0);

  bufsets(b,buflen(b),"]...");
  logcheck(buflen(b) == 13);
  logcheck(strcmp(buf(b),"WXA]|[CDE]...")==0);

  bufinss(b,0,"...[");
  logcheck(buflen(b) == 17);
  logcheck(strcmp(buf(b),"...[WXA]|[CDE]...")==0);

  logprintf("   [%s]",buf(b));
  bufdel(b,0,2);
  logcheck(buflen(b) == 14);
  if (!logcheck(strcmp(buf(b),"[WXA]|[CDE]...")==0)) {
    logprintf("   [%s]",buf(b));
  }
  
  bufdel(b,11,10000);
  logcheck(buflen(b) == 11);
  logcheck(strcmp(buf(b),"[WXA]|[CDE]")==0);
  
  bufdel(b,5,5);
  logcheck(buflen(b) == 10);
  if (!logcheck(strcmp(buf(b),"[WXA][CDE]")==0)) {
    logprintf("   [%s]",buf(b));
  }
  
  logclose();
  exit(0);
}
