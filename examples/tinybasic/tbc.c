/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
** 
*/

#include "utl.h"
#include <getopt.h>

/*
   __ _ _ __ __ _ _ __ ___  _ __ ___   __ _ _ __ 
  / _` | '__/ _` | '_ ` _ \| '_ ` _ \ / _` | '__|
 | (_| | | | (_| | | | | | | | | | | | (_| | |   
  \__, |_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|   
   __/ |                                         
  |___/                                          
 
progam ::= line*
 
line ::= number statement

statement ::= "PRINT" pr-expr-list  /
              "INPUT" in-var-list   /
              "LET"? var "=" expr /
              "GOTO" expr  /
              "GOSUB" expr  /
              "FOR" var "=" expr "TO" expr ("STEP" expr)? /
              "NEXT" /
              "RETURN" /
              "IF" expr relop expr "THEN" statement /
              "END" /
              "REM" /
              "POKE" expr "," expr 

var ::= "A" | "B" | ... | "Z"
string ::= '"' char* '"'

pr-expr-list ::= (string|expression) ((','/';') (string|expression) )* ';'?
in-var-list ::= var (',' var)*

expr = (term ('+' / '-'))* term
term = (fact ('*' / '/'))* fact
fact = '-' fact / '(' expr ')' / num / var / fun

fun = 'RND' '(' expr ')' /
      'PEEK' '(' expr ')' 

relop ::= '<' ('>' | '=')? | '>' '='? | '='

number ::= digit digit*
   
digit ::= '0' | '1' | '2' | '3' | ... | '8' | '9'
 
*/

pegrule(tinyprg) {
  pegaction(start_pgm);
  pegstar { 
    pegstar { pegwspace; pegeol; }
    pegref (line);
  }
  pegaction(end_pgm);
}

pegrule (line) {
  pegwspace;
  pegopt {
    pegfailsetmessage("Expected line number");
    pegaction (line_start) {
      pegref(linenum);
    }
    pegwspace;
    pegfailsetmessage("Invalid statement");
    pegaction(line_end) {pegref(statement);}
  }
  pegwspace;
  pegeol;
}

pegrule (linenum) {
  static int lastln = 0;
  int lnnum;
  pegfailsetmessage("Invalid line number");
  pegpmx("<+d>");
  if (!pegfailed()) {
    lnnum = atoi(pmxstart(0));
    if (lnnum > lastln) {
      if ( 1 <= lnnum && lnnum <= 32767) lastln = lnnum;
      else pegerror;
    }   
    else pegerror;
  }
}

pegrule(statement) {
  pegchoice {
    pegeither {                       pegpmx("<I>PRINT");
                                      pegref(pr_expr_list); 
    }
    pegor {                           pegpmx("<I>INPUT");
                                      pegref(in_var_list);
    }
    pegor {                           pegpmx("<I>LET<*k>"); 
                pegaction (let_var) { pegref(var); }
                                      pegpmx("<*k>=<*k>");
                                      pegref(expr);
                pegaction(let_end);
    }
    pegor {                           pegpmx("<I>GOTO");    
                pegaction(goto_addr); pegref(expr); 
                pegaction(goto_end);
    }
    pegor {                           pegpmx("<I>GOSUB");    
                pegaction(goto_addr); pegref(expr); 
                pegaction(gosub_end);
    }
    pegor {                           pegpmx("<I>REM<*!N>");
    }
    pegor {     pegaction(gosub_ret) {pegpmx("<I>RETURN");}
    }
    pegor {     pegaction(halt)      {pegpmx("<I>END");}
    }
    pegor {                           pegpmx("<I>POKE<*k>");
                pegaction(poke_addr); pegref(expr); 
                                      pegpmx("<*k>,<*k>");
                pegaction(poke_val);  pegref(expr); 
                pegaction(poke_end);
    }
    pegor {     pegaction(if_start);  pegpmx("<I>IF");
                                      pegref(expr);
                                      pegref(relop);
                                      pegref(expr);
                pegaction(if_mid);    pegpmx("<I>THEN<*k>");
                                      pegref(statement);
                pegaction(if_end);
    }
  }
}

pegrule(var) {
  pegpmx("<=A-Za-z>");
}

pegrule(in_var_list) {
  pegfailsetmessage("Invalid argument list for INPUT");
  pegwspace;
  pegaction(in_var) { pegref(var); }
  pegstar {                      pegstr("<*k>,<*k>");
             pegaction(in_var) { pegref(var);}
  }
}

pegrule(string_or_expr) {
  pegfailsetmessage("Invalid argument list for PRINT");
  pegwspace;
  pegchoice {
    pegeither { pegaction(pr_string) { pegpmx("<Q\"\"\\>"); }}
    pegor     { pegaction(pr_expr_start); pegref(expr); pegaction(pr_expr_end); }
  }
  pegwspace;
}

pegrule(pr_expr_list) {
  pegref(string_or_expr);
  pegstar {  pegaction(pr_separator) {pegoneof(",;");}
             pegref(string_or_expr);
  }
  pegaction(pr_nl) { pegopt{ pegstr(";");  }}
}

pegrule(expr) {  // expr = (term ('+' / '-'))* term
  pegfailsetmessage("Invalid expression");
  pegwspace; 
  pegaction(expr_start);
  pegstar {                      pegref(term);
            pegaction(expr_any) {pegoneof("+-");}
  }
  pegref(term);
  pegaction(expr_end);
  pegwspace; 
}

pegrule(term) { // term = (fact ('*' / '/'))* fact
  pegwspace; 
  pegstar {               pegref(fact);
    pegaction (expr_any) {pegoneof("*/");}
  }
  pegref(fact);
  pegwspace; 
}

pegrule(fact) { // fact = '-' fact / '(' expr ')' / num / var / 'RND' '(' expr ')'  / 'PEEK' '(' expr ')' 
  pegwspace; 
  pegchoice {
    pegeither { pegaction(expr_any) {pegstr("-");}
                pegwspace;
                pegref(fact);
    }
    pegor {     pegaction(expr_any) {pegstr("(");}
                                     pegref(expr);
                pegaction(expr_any) {pegstr(")");}
    }
    pegor {     pegaction(expr_any) {pegpmx("<+d>");}
    }
    pegor {     pegaction(expr_fun) {pegpmx("<I>PEEK|RND"); }
                                     pegwspace;
                pegaction(expr_any) {pegstr("(");}
                                     pegref(expr);
                pegaction(expr_any) {pegstr(")");}
    }
    pegor {     pegaction(expr_var) {pegref(var); }
    }
  }
  pegwspace; 
}

pegrule(relop) { 
  pegchoice {
    pegeither { pegaction(expr_any) { pegpmx("%<<?==>|%><?==>"); }
    }
    pegor {     pegaction(expr_neq) { pegstr("<>"); }
    }
    pegor {     pegaction(expr_eq)  { pegstr("="); }
    }
  }  
}

/*
             _   _                 
            | | (_)                
   __ _  ___| |_ _  ___  _ __  ___ 
  / _` |/ __| __| |/ _ \| '_ \/ __|
 | (_| | (__| |_| | (_) | | | \__ \
  \__,_|\___|\__|_|\___/|_| |_|___/

*/

typedef struct {
  FILE *f;
  int   n;
} tbaux_t;

int pr_expr_start(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"printf(\"%%d\",");
  return 0;
}

int pr_expr_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,");\n");
  return 0;
}
int pr_separator(const char *from, const char *to, void *aux)
{
  const char *s = from;
  FILE *f = ((tbaux_t *)aux)->f;
  if (*s == ',') fprintf(f,"printf(\"\\t\");\n");
  return 0;
}

int pr_nl(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  if (from == to) fprintf(f,"printf(\"\\n\");\n");
  fprintf(f,"fflush(stdout);\n");
  return 0;
}

int pr_string(const char *from, const char *to, void *aux)
{
  const char *s = from;
  FILE *f = ((tbaux_t *)aux)->f;
  
  fprintf(f,"printf(\"%%s\",");
  while (*s && s < to) {
    if (*s == '\\') fputs("\\\\",f);
    else fputc(*s,f);
    s++;
  }
  fprintf(f,");\n");
  return 0;
}

int let_var(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"var('%c') = ",*from);
  return 0;
}

int in_var(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"scanf(\"%%d\",&var('%c'));\n",*from);
  return 0;
}

int let_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fputs(";\n",f);
  return 0;
}

int expr_start(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"(short)(");  
  return 0;
}

int expr_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,")");  
  return 0;
}

int expr_fun(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  const char *s = from;
  while (*s && s<to) fputc(tolower((int)(*s++)),f);
  return 0;
}

int expr_any(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"%.*s",(int)(to-from),from);  
  return 0;
}

int expr_neq(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"!=");
  return 0;
}

int expr_eq(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"==");
  return 0;
}

int expr_var(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"var('%c')",*from);
  return 0;
}

int poke_addr(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"mem[(");
  return 0;
}

int poke_val(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,") & 1023] = (unsigned char)((");
  return 0;
}

int poke_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,") & 0xFF);\n");
  return 0;
}

int goto_addr(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"nextline = ");
  return 0;
}

int goto_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,";\ngoto start;\n");
  return 0;
}

int gosub_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,";\n");
  fprintf(f,"if (ret_sptr == 32) {nextline=50000;}\n");
  fprintf(f,"else {ret_stack[ret_sptr++] = -linenum;}\n");
  fprintf(f,"goto start;\n");
  return 0;
}

int gosub_ret(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,";\n");
  fprintf(f,"if (ret_sptr == 0) {nextline=50001;}\n");
  fprintf(f,"else {nextline = ret_stack[--ret_sptr] ;}\n");
  fprintf(f,"goto start;\n");
  return 0;
}

int halt(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"goto halt;\n");
  return 0;
}

int line_start(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  ((tbaux_t *)aux)->n = atoi(from);
  fprintf(f,"case %.*s: linenum=%.*s;\n",(int)(to-from), from, (int)(to-from), from);  
  return(0);
}

int line_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  int ln =  ((tbaux_t *)aux)->n;
  fprintf(f,"case -%d: \n",ln);  
  return(0);
}

int if_start(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"if (");  
  return(0);
}

int if_mid(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,") {\n");  
  return(0);
}

int if_end(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fprintf(f,"}\n");  
  return(0);
}

int start_pgm(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fputs("#include <stdio.h>\n",f);
  fputs("#include <stdlib.h>\n",f);
  fputs("#include <ctype.h>\n",f);
  fputs("#include <time.h>\n",f);
  fputs("unsigned char mem[1024];\n",f);
  fputs("short vars[26];\n",f);
  fputs("int ret_stack[32];\n",f);
  fputs("short ret_sptr=0;\n",f);
  fputs("#define var(v) vars[toupper((int)v)-'A']\n",f);
  fputs("#define peek(x) mem[(x) & 1023]\n",f);
  fputs("short rnd(short x) { int r ;\n",f);
  fputs("if (x==0) {\n",f);
  fputs("srand(time(0));\n",f);
  fputs("x = 0x7FFF;\n",f);
  fputs("}\n",f);
  fputs("else if (x<0) {\n",f);
  fputs("srand(-x);\n",f);
  fputs("x = 0x7FFF;\n",f);
  fputs("}\n",f);
  fputs("r = rand() & 0x7FFF;\n",f);
  fputs("return ((int)x * r) / 0x8000;\n",f);
  fputs("}\n",f);
  fputs("int main (int argc, char *argv[]) {\n",f);
  fputs("int linenum=0;\n",f);
  fputs("int nextline=0;\n",f);
  fputs("start:\n",f);
  fputs("switch(nextline) {\n",f);
  fputs("case 0:\n",f);
  return 0;
}

int end_pgm(const char *from, const char *to, void *aux)
{
  FILE *f = ((tbaux_t *)aux)->f;
  fputs("goto halt;\n",f);
  fputs("case 50000:\n",f);
  fputs("fprintf(stderr,\"Runtime error %d,1: Return stack overflow\",linenum);\n",f);
  fputs("goto halt;\n",f);
  fputs("case 50001:\n",f);
  fputs("fprintf(stderr,\"Runtime error %d,1: Return stack empty\",linenum);\n",f);
  fputs("goto halt;\n",f);
  fputs("default:\n",f);
  fputs("fprintf(stderr,\"Runtime error %d,1: Invalid GOTO/GOSUB %d\",linenum,nextline);\n",f);
  fputs("goto halt;\n",f);
  fputs("}\n",f);
  fputs("halt: return(0);\n",f);
  fputs("}\n",f);
  return 0;
}
 
 
 
/*
                  _       
                 (_)      
  _ __ ___   __ _ _ _ __  
 | '_ ` _ \ / _` | | '_ \ 
 | | | | | | (_| | | | | |
 |_| |_| |_|\__,_|_|_| |_|
                          
*/

void usage(int n)
{
   fprintf(stderr,"Usage: tbc [-c | -o prg] src\n");
   exit(n);
}

int main (int argc, char *argv[]) 
{
  char *tmpfname = NULL;
  char *outfname = "t.out";
  FILE *outfile = NULL;
  char *source_fname = NULL;
  buf_t source_code = NULL;
  FILE *source_file = stdin;
  peg_t parser;
  int source_only = 0;
  char cmd[1024];
  tbaux_t tbaux;
  int op;

  while ((op = getopt(argc, argv, "o:hc")) != -1) {
    switch (op) {
      case 'o': outfname = optarg;  break;
      case 'c': source_only = 1;  break;
      case 'h': usage(0); break;
      default : usage(1); break;          
    }
  }
  
  if (optind < argc) {
    source_fname = argv[optind];
    source_file = fopen(source_fname,"rb");
    if (!source_file) {
      fprintf(stderr,"Cannot open '%s' for input", source_fname);
      exit(1);
    }
  }

  source_code = bufnew();
  if (!source_code) {
    fprintf(stderr,"Cannot allocate mamory for input");
    exit(1);
  }
  bufreadall(source_code,0,source_file);
  if (source_file != stdin) fclose(source_file);
  
  tmpfname = "tmp_tb.c";
  outfile = fopen(tmpfname,"wb");
  assert(outfile);
  parser = pegnew();
  tbaux.f = outfile;
  tbaux.n = 0;
  if (!pegparse(parser,tinyprg,buf(source_code),&tbaux) || (pegpos(parser)[0] != '\0' )) { 
    const char *t;
    t = pegfailline(parser);
    while (*t && *t != '\r' && *t != '\n') fputc(*t++,stderr);
    fputc('\n',stderr);
    fprintf(stderr,"%*s\n",pegfailcolumn(parser),"^");
    fprintf(stderr,"Syntax error %d,%d: %s\n", pegfaillinenum(parser), pegfailcolumn(parser),pegfailmessage(parser));
  }
  else {
    fclose(outfile); outfile=NULL;
    if (!source_only) sprintf(cmd,"cc -o %s %s ",outfname,tmpfname);
    if (source_only || system(cmd)) {
      outfile = fopen(tmpfname,"rb");
      if (outfile) {
        int c;
        while ((c=fgetc(outfile)) != EOF) {
          fputc(c,stdout);
        }
      }
    } 
  }
  source_code = buffree(source_code);
  parser = pegfree(parser);
  if (outfile) fclose(outfile);
  remove(tmpfname);
  exit(0);
}
