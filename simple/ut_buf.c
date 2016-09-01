#define UTL_MAIN
#include "utl.h"
#include <math.h>

#define TESTFILE "buf.tst"
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
  
  logopen("ut_buf.log","w");
 
  
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
  logclose();
  exit(0);
}