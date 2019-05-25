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
#include <math.h>

#define TESTFILE "l_buf.tmp"
FILE *createtestfile(char *content)
{
  FILE *f;
  
  f=fopen(TESTFILE,"wb");
  if (!f) return 0;
  
  fprintf(f,"%s",content);
  fclose(f);
  f=fopen(TESTFILE,"rb");
  return f;
}

int main(int argc, char *argv[])
{
  
  buf_t b=NULL;
  char c;
  FILE *f =NULL; 
  
  logopen("l_buf.log","w");

#if 0
  logfeature("Manage flexible strings.") {
    logactor("Programmer");
    logwants("an easier way to manipulate buffers of characters");
    loginorderto("speed up development time and reduce errors");
  }
#endif

  logscenario("Create a buffer") {
    loggiven("no buffer have been created so far");
    logwhen("a new buffer is created")
      b = bufnew();
    logthen("the returned buffer is not NULL") 
      logassert(b);
  }
  
  logscenario("Setting/Retrieving single characters.") {
    logexamples( { int pos; char chr;}, 
                {{ 0      , 'a'      },
                 { 3      , 'b'      },
                 { 123    , 'c'      } }) {
      loggiven("an existing buffer")
        logcheck(b);
      logwhen("the character '%c' is stored at position %d", logexample.chr, logexample.pos) 
        bufsetc(b, logexample.pos, logexample.chr);
      logthen("it should be retrived from that postion") {
        c = bufgetc(b, logexample.pos);
        logcheck (c == logexample.chr);
      }
    }
  }
  
  char *file_content = "ABCDEF\nGHIJKL\n123456";

  logscenario("Reading from the beginning of a file.") {
    loggiven("a file that contains \"ABCDEF\\nGHIJKL\\n123456\"") {
      logassert((f=createtestfile(file_content)));
    }
    logwhen("the first 4 characters are read") 
      bufread(b,0,4,f);  
    logthen("the buffer should contain \"ABCD\\0\"") {
      logcheck(buf(b)[4] == '\0');
      logcheck(strncmp(buf(b),"ABCD",4) == 0);
    }
  }
  logscenario("Continuing reading from the file.") {
    logwhen("the rest of file is read and added at the end of the buffer")
      bufreadall(b,buflen(b),f);
    logthen("the full content should be in the buffer") {
      logassert(buf(b)[buflen(b)] == '\0');
      logexpect(buflen(b) == strlen(file_content),"buflen:%d instead of  %lu",buflen(b),strlen(file_content));
      logcheck(strcmp(buf(b),file_content) == 0);
    }
  }
  
  logscenario("Reading a file line by line.") {
    int l = 0;
    logexamples({char *ln;}, {{"ABCDEF\n"},{"GHIJKL\n"},{"123456\n"}}) {
      loggiven("a file that contains \"ABCDEF\\nGHIJKL\\n123456\"") {
        fseek(f,0,SEEK_SET);
      }
      logwhen("the line %d is read", ++l) {
        logcheck(bufreadln(b,0,f)); 
      }
      logthen("line %d should be \"%.6s\"",l,logexample.ln) {
        logcheck(logexample.ln);
        logcheck(strlen(buf(b))==7);
        logexpect(logexample.ln && strcmp(logexample.ln,buf(b))==0,"got \"%.6s\"",buf(b));
      }
    }
    logand("no other lines should be available for reading")
      logcheck(!bufreadln(b,0,f));
    logand("last line should be preserved if reading from an empty file")
      logcheck(strcmp(buf(b),"123456\n") == 0);
  }
  fclose(f);
  
  logscenario("Setting strings at different positions") {
    logexamples({int pos; char *str    ; char *exp    ;},
               {{    0  ,      "WXYZ"  ,       "WXYZ"  },
                {    2  ,      "CDE"   ,       "WXCDE" }}) {
      loggiven("an existing buffer")
        logcheck(b);
      logwhen("the string \"%s\" is set to position %d",logexample.str, logexample.pos)
        bufsets(b, logexample.pos, logexample.str);
      logthen("the buffer contains \"%s\"",logexample.exp) {
        logcheck(bufgetc(b,strlen(logexample.exp)) == '\0');
        logcheck(buflen(b) == strlen(logexample.exp));
        logcheck(strcmp(buf(b),logexample.exp)==0);
      }
    }
  }
  
  loggiven("the buffer contains \"WXCDE\"") ;
  logwhen("the character 'A' is inserted in position 2")
    bufinsc(b,2,'A');
  logthen("the buffer contains \"WXACDE\"") {
    logcheck(bufgetc(b,6) == '\0');
    logcheck(buflen(b) == 6);
    logexpect(strcmp(buf(b),"WXACDE")==0 , "   [%s]",buf(b));
  }
  
  loggiven("the buffer contains \"WXACDE\"");
  logwhen("the string \"]|[\" is inserted at position 3")
     bufinss(b,3,"]|[");
  logthen("the buffer contains \"WXA]|[CDE\"") {
    logcheck(buflen(b) == 9);
    logcheck(strcmp(buf(b),"WXA]|[CDE")==0);
  }

  loggiven("the buffer contains \"WXA]|[CDE\"");
  logwhen("the string \"]...\" is added at the end")
    bufsets(b,buflen(b),"]...");
  logthen("the buffer contaanins \"WXA]|[CDE]...\"") {
    logcheck(buflen(b) == 13);
    logcheck(strcmp(buf(b),"WXA]|[CDE]...")==0);
  }

  loggiven("the buffer contains \"WXA]|[CDE]...\"");
  logwhen("the string \"...[\" is added at the beginning")
    bufinss(b,0,"...[");
  logthen("the buffer contains \"...[WXA]|[CDE]...\"") {
    logcheck(buflen(b) == 17);
    logcheck(strcmp(buf(b),"...[WXA]|[CDE]...")==0);
  }

  loggiven("the buffer contains \"...[WXA]|[CDE]...\"");
  logwhen("the first three charactes (from 0 to 2) are deleted")
    bufdel(b,0,2);
  logthen("the buffer contains \"[WXA]|[CDE]...\"") {
    logcheck(buflen(b) == 14);
    logexpect((strcmp(buf(b),"[WXA]|[CDE]...")==0), "got: \"%s\"",buf(b));
  }
  
  loggiven("the buffer contains \"[WXA]|[CDE]...\"");
  logwhen("characters are deleted starting from position 11 up to the end")
    bufdel(b,11,buflen(b));
  logthen("the buffer contains \"[WXA]|[CDE]\"") {
    logcheck(buflen(b) == 11);
    logcheck(strcmp(buf(b),"[WXA]|[CDE]")==0);
  }
  
  bufdel(b,5,5);
  logcheck(buflen(b) == 10);
  logexpect(strcmp(buf(b),"[WXA][CDE]")==0,"   [%s]",buf(b));
  
  loggiven("a buffer exists")
    logcheck(b);
  logwhen("the buffer is freed") {
    b = buffree(b);
  }
  logthen("the pointer returned is NULL") {
    logcheck(b==NULL);
  }
  
  logscenario("Formatted print in a buffer.") {
      loggiven("a buffer")
        logcheck((b = bufnew()));
      logwhen("a formatted string is print in the buffer") 
        bufsetf(b,"%d",32);
      logthen("the right string is there") {
        logexpect(buflen(b) == 2,"len: %d",buflen(b));  
        logexpect(strcmp(buf(b),"32")==0,"   [%s]",buf(b));
      }
      bufaddf(b,"%02X",32);
      logexpect(strcmp(buf(b),"3220")==0,"   [%s]",buf(b));
  }
  
  logcheck(buffreeze("l_buf.frz",b));
  b = vecfree(b);
  logcheck(!b);
  
  b = bufunfreeze("l_buf.frz");
  logassert(b);
  
  logexpect(strcmp(buf(b),"3220")==0,"   [%s]",buf(b));
  
  b = buffree(b);
  logclose();
  exit(0);
}
