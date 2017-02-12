/* 
**  (C) by Remo Dentato (rdentato@gmail.com)
** 
** This software is distributed under the terms of the MIT license:
**  https://opensource.org/licenses/MIT
** 
*/

#include "utl.h"

/*   ___                               
    / __|_ _ __ _ _ __  _ __  __ _ _ _ 
   | (_ | '_/ _` | '  \| '  \/ _` | '_|
    \___|_| \__,_|_|_|_|_|_|_\__,_|_|  

 A simple calculator for arithmetic expressions on integers
 
 We'll use the following (not left-recursive) peg:
 
   expr = (term ('+' / '-'))* term
   term = (fact ('*' / '/'))* fact
   fact = '(' expr ')' / num

 to better handle left associativity of '-' and '/' operators.
 
*/
  
pegrule(expr) {  // expr = (term ('+' / '-'))* term
  pegref(SPC); 
  pegstar {
    pegref(term);
    pegaction(op) {
      pegoneof("+-");
    }
  }
  pegref(term);
  pegref(SPC); 
}

pegrule(term) { // term = (fact ('*' / '/'))* fact
  pegaction(open);
  pegref(SPC); 
  pegstar {
    pegref(fact);
    pegaction(op) {
      pegoneof("*/");
    }
  }
  pegref(fact);
  pegaction(close);
  pegref(SPC); 
}

pegrule(fact) { // fact = '(' expr ')' / num
  pegref(SPC); 
  pegswitch {
    pegcase {
      pegstr("(");
      pegaction(open);
      pegref(expr);
      pegaction(close);
      pegstr(")");
    }
    pegcase {
      pegaction(arg) {
        pegmore{ pegdigit; } 
      }
    }
  }
  pegref(SPC); 
}

pegrule(SPC) {
  pegstar{ pegoneof(" \t\r\n");}
}

/*  ___ _           _   
   / __| |_ __ _ __| |__
   \__ \  _/ _` / _| / /
   |___/\__\__,_\__|_\_\
*/

#define MAX_STACK 32
typedef struct  {
  int cnt;
  struct {
    int val; char op;
  } stk[MAX_STACK];
} stack_t;

stack_t stack;

void stack_clean(stack_t *stk)
{
  stk->cnt =0;
}

void stack_push(stack_t *stk, int val, char op)
{
  if (stk->cnt < MAX_STACK) {
    stk->stk[stk->cnt].val = val;
    stk->stk[stk->cnt].op = op;
    stk->cnt++;
  }
}

void stack_drop(stack_t *stk)
{
  if (stk->cnt > 0) stk->cnt--;
}

#define stack_top(s_)     (s_)->stk[(s_)->cnt-1]
#define stack_isempty(s_) ((s_)->cnt == 0)

/*  ___                     _   _        _      _   _             
   / __| ___ _ __  __ _ _ _| |_(_)__    /_\  __| |_(_)___ _ _  ___
   \__ \/ -_) '  \/ _` | ' \  _| / _|  / _ \/ _|  _| / _ \ ' \(_-<
   |___/\___|_|_|_\__,_|_||_\__|_\__| /_/ \_\__|\__|_\___/_||_/__/
*/

void calc_op(stack_t *stk, int val)
{
  if (!stack_isempty(stk) > 0) {
    switch(stack_top(stk).op) {
      case '+' : stack_top(stk).val += val; break;
      case '-' : stack_top(stk).val -= val; break;
      case '*' : stack_top(stk).val *= val; break;
      case '/' : stack_top(stk).val /= val; break;
      default  : stack_top(stk).val  = val; break;
    }
    stack_top(stk).op = '=';
  }
}

void op(const char *from, const char *to, void *aux)
{
  stack_t *stk = aux;
  
  if (!stack_isempty(stk)) {
    stack_top(stk).op = *from;
  }
  printf("%c",*from);
}

void arg(const char *from, const char *to, void *aux)
{
  stack_t *stk = aux;
  int val;
  val = atoi(from);
  calc_op(stk,val);
  printf("%d",val);
}

void open(const char *from, const char *to, void *aux)
{
  stack_t *stk = aux;
  stack_push(&stack,0,'=');
  printf("(");
}

void close(const char *from, const char *to, void *aux)
{
  stack_t *stk = aux;
  int val;
  val = stack_top(stk).val;
  stack_drop(stk);
  calc_op(stk,val);
  printf(")");
}

/*             _     
    _ __  __ _(_)_ _ 
   | '  \/ _` | | ' \
   |_|_|_\__,_|_|_||_|
                      
*/

#define MAX_EXPR_LEN 256
char expression[MAX_EXPR_LEN];

int main(int argc, char *argv[])
{
   peg_t parser;
   parser = pegnew();
   while (fgets(expression,MAX_EXPR_LEN,stdin)) {
     stack_clean(&stack);
     printf("\n\n%s=",expression);
     stack_push(&stack,0,'=');
     if (pegparse(parser,expr,expression,&stack)) {
       printf("\n=%d",stack_top(&stack).val);
     }
     else {
       printf("error\n");
     }
   }
   parser = pegfree(parser);
   exit(0);
}