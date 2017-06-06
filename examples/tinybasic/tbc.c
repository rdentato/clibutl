/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
** 
*/

#include "utl.h"

/*
   __ _ _ __ __ _ _ __ ___  _ __ ___   __ _ _ __ 
  / _` | '__/ _` | '_ ` _ \| '_ ` _ \ / _` | '__|
 | (_| | | | (_| | | | | | | | | | | | (_| | |   
  \__, |_|  \__,_|_| |_| |_|_| |_| |_|\__,_|_|   
   __/ |                                         
  |___/                                          
 
progam ::= line*
 
line ::= number statement

statement ::= "PRINT" expr-list  /
              "INPUT" var-list   /
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

expr-list ::= (string|expression) ((','/';') (string|expression) )* ';'?
var-list ::= var (',' var)*

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
  pegstar { 
    pegstar { pegwspace; pegeol; }
    pegref (line);
  }
}

pegrule (linenum) {
  pegpmx("<+d>");   
}

pegrule (line) {
  pegwspace;
  pegopt {
    pegref(linenum);
    pegwspace;
    pegref(statement);
  }
  pegeol;
}

pegrule(statement) {
  pegchoice {
    pegeither { pegpmx("<I>PRINT"); 
                pegref(expr_list);
    }
    pegor {     pegpmx("<I>INPUT");
                pegref(var_list);
    }
  }
}

pegrule(var) {
  pegpmx("<=A-Za-z>");
}

pegrule(var_list) {
  pegwspace;
  pegref(var);
  pegstar {
    pegpmx("<*k>,<*k>");
    pegref(var);
  }
}

pegrule(string_or_expr) {
  pegwspace;
  pegchoice {
    pegeither { pegpmx("<Q\"\"\\>"); }
    pegor     { pegref(expr); }
  }
  pegwspace;
}

pegrule(expr_list) {
  pegref(string_or_expr);
  pegstar {  pegoneof(",;");
             pegref(string_or_expr);
  }
  pegopt {  pegstr(";");  }
}

pegrule(expr) {  // expr = (term ('+' / '-'))* term
  pegwspace; 
  pegstar { pegref(term);
            pegoneof("+-");
  }
  pegref(term);
  pegwspace; 
}

pegrule(term) { // term = (fact ('*' / '/'))* fact
  pegwspace; 
  pegstar {
    pegref(fact);
    pegoneof("*/");
  }
  pegref(fact);
  pegwspace; 
}

pegrule(fact) { // fact = '-' fact / '(' expr ')' / num / var
  pegwspace; 
  pegchoice {
    pegeither { pegstr("-");
                pegwspace;
                pegref(fact);
    }
    pegor {     pegstr("(");
                pegref(expr);
                pegstr(")");
    }
    pegor {     pegpmx("<+d>");
    }
    pegor {     pegref(var); 
    }
  }
  pegwspace; 
}



/*
                  _       
                 (_)      
  _ __ ___   __ _ _ _ __  
 | '_ ` _ \ / _` | | '_ \ 
 | | | | | | (_| | | | | |
 |_| |_| |_|\__,_|_|_| |_|
                          
*/

int main (int argc, char *argv[]) 
{
  char *outfname = "";
  char *source_fname = NULL;
  buf_t source_code = NULL;
  FILE *source_file = stdin;
  peg_t parser;

  
  if (argc > 1 && argv[1][0] != '-') {
    source_fname = argv[1];
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
    
  parser = pegnew();
  if (pegparse(parser,tinyprg,buf(source_code))) { 
    if (pegfailpos(parser)[0] != '\0' ) {
      fprintf(stderr,"error %d,%d: %s\n", pegerrline(parser), pegerrcolumn(parser),pegfailpos(parser));    
    }
    else 
      fprintf(stdout,"compiled\n");
  }
  else {
    fprintf(stderr,"error\n");    
  }
  source_code = buffree(source_code);
  parser = pegfree(parser);
  exit(0);
}
