
#include "utl.h"

/*
 expr  ::= term (op term)*
   term  ::= num
   op    ::= '+' | '-'
   num   ::= digit+
   digit ::= '0' | '1' | ... | '9' </pre>
*/

pegrule(expr) {
  pegref(term);
  pegstar {
    pegref(op);
    pegref(term);
  }
}

pegrule(term) {
  pegwspace;
  pegref(num);
  pegwspace;
}

pegrule(op) {
  pegwspace;
  pegchoice {
    pegeither { pegstr("+"); }
    pegor     { pegstr("-"); }
  }
}

pegrule(num) {
  pegplus {
    pegref(digit);
  }
}

pegrule(digit) {
  pegoneof("0123456789");
}

#define MAX_EXPR_LEN 256
char expression[MAX_EXPR_LEN];

int main(int argc, char *argv [])
{
  peg_t parser;
  
  parser = pegnew();

  while (fgets(expression,MAX_EXPR_LEN,stdin)) {
    if (pegparse(parser,expr,expression))
      printf("(%.*s) is a valid expression\n",pegpos(parser)-expression,expression);
    else
      printf("(%s) is not a valid expression\n",expression);
    fflush(stdout);
  }  
  
  parser = pegfree(parser);
  exit(0);
}
