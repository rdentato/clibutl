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


/*
         0
        /|\
       1 2 3
      / / \ \
     4 5   6 7
          / \
         8   9

*/
FILE *outfile = NULL;

int prtnode(arb_t a)
{
    if (a) {
      loginfo("Node: %d (%p) %d", arbgetdata(a), (void*)a, a->cur);
    }
    return 0;
}

int prtopen(arb_t a)
{
    fprintf(outfile,"(%d",arbgetdata(a));
    return 0;
}

int prtopen20(arb_t a)
{
  int *s;
  s = arbaux(a);
  fprintf(outfile,"(");
  if (*s < -20) return 1;
  *s += arbgetdata(a);
  fprintf(outfile,"%d",arbgetdata(a));
  return 0;
}

int prtclose(arb_t a)
{
    fprintf(outfile,")");
    return 0;
}


int main(int argc, char *argv[])
{
  
  arb_t a;
  int i;
  arb_node_t n;
  utl_arb_node_t *p;
  
  logopen("l_arb.log","w");

  outfile = fopen("l_arb2.log","w");

  vec_t v = NULL;
  int k;
  v=vecnew(int);
  logassert(v);
  
  loginfo("\nQUE\n");
  
  logcheck(veccount(v) == 0);
  logcheck(vecisempty(v));
  k=vecfirst(int,v,-12);
  logcheck(k == -12);
  vecenq(int,v,100);
  logcheck(veccount(v) == 1);
  k=vecfirst(int,v,0);
  logcheck(k == 100);

  vecenq(int,v,200);
  logcheck(veccount(v) == 2);
  k=vecfirst(int,v,0);
  logcheck(k == 100);
  
  k=veclast(int,v,0);
  logcheck(k == 200);

  vecdeq(v);
  logcheck(veccount(v) == 1);
  k=vecfirst(int,v,0);
  logcheck(k == 200);
  
  vecdeq(v);
  logcheck(vecisempty(v));
  v=vecfree(v);
  
  loginfo("\nSTK\n");
  
  v=vecnew(int);
  logcheck(vecisempty(v));
  k = vectop(int,v,-1);
  logcheck(k == -1);  
  vecpush(int,v,1);
  logcheck(!vecisempty(v));
  logcheck(veccount(v) == 1);
  
  k = vectop(int,v,-1);
  logcheck(k == 1);  
  
  vecdrop(v);
  logcheck(vecisempty(v));

  vecpush(int,v,2);
  logcheck(!vecisempty(v));
  logcheck(veccount(v) == 1);
  
  k = vectop(int,v,-1);
  logcheck(k == 2);  

  vecpush(int,v,3);
  logcheck(!vecisempty(v));
  logcheck(veccount(v) == 2);
  
  k = vectop(int,v,-1);
  logcheck(k == 3);  
  
  v=vecfree(v);
  
  loginfo("\nARB\n");
  a = arbnew();
  logassert(a);
  logcheck(a->esz == sizeof(utl_arb_node_t));
  logcheck(arbcount(a) == 0);
  
  arbaddchild(a);
  logcheck(arbcount(a) == 1);
  logcheck(a->cur == 1);
  logcheck(a->fst == 1);
  p = ((utl_arb_node_t *)(a->vec))+(a->cur-1);
  
  arbsetdata(a,-1);
  i = arbgetdata(a);
  logcheck(i == -1);
  logcheck(p->upn == 0);
  logcheck(p->dat == -1);

  n = arbroot(a);
  logcheck(n == 1);
 
  arbaddchild(a);
  arbsetdata(a,-3);
  logcheck(a->cur == 2);
  logcheck(a->fst == 1);
  logcheck(((utl_arb_node_t *)(a->vec))[a->cur-1].upn == 1);
  logcheck(((utl_arb_node_t *)(a->vec))[a->cur-1].nxt == 0);
  logcheck(((utl_arb_node_t *)(a->vec))[a->fst-1].dwn == 2);
  
  arbcurrent(a,n);
  logcheck(a->cur == 1);
  
  arbaddchild(a);
  arbsetdata(a,-2);
  logcheck(a->cur == 3);
  logcheck(((utl_arb_node_t *)(a->vec))[a->cur-1].upn == 1);
  logcheck(((utl_arb_node_t *)(a->vec))[a->cur-1].nxt == 2);
  logcheck(((utl_arb_node_t *)(a->vec))[a->fst-1].dwn == 3);
  
  arbaddchild(a);
  arbsetdata(a,-20);
  arbaddsibling(a);
  arbsetdata(a,-21);
  
  arbroot(a);
  arbdfspre(a,prtnode);
  loginfo("      ***  POST");
  arbroot(a);
  arbdfspost(a,prtnode);
  
  loginfo("      ***  PRE");
  arbroot(a);
  arbdfspre(a,prtnode);

  arbroot(a);
  arbdfs(a,prtopen,prtclose);

  int sum;
  arbaux(a,&sum);
  logcheck(arbaux(a) == &sum);
  fprintf(outfile,"\n");
  sum=0;
  arbroot(a);
  arbdfs(a,prtopen20,prtclose);
  
  #if 1
  loginfo("      ***  BFS");
  arbroot(a);
  arbbfs(a,prtnode);
  #endif
  
  arbroot(a);
  arbfirstchild(a);
  
  i = arbprune(a);
  
  logcheck(i==0);
  logexpect(arbcount(a) == 2,"(Got %d)",arbcount(a));
  arbbfs(a,prtnode);
  
  arbroot(a);
  arbaddchild(a);
  arbsetdata(a,2);
  logexpect(arbcount(a) == 3,"(Got %d)",arbcount(a));
  
  arbroot(a);
  arbbfs(a,prtnode);
  
  a = arbfree(a);
  
  fclose(outfile);
  logclose();
    
  exit(0);
}
