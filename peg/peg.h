
typedef struct { 
  peg__id id;
  char *start;
  char *cur;
  char *save;
} peg_s, *peg;


int peg__check(peg p, peg__id i)
{
  p->id = i;
  return 0;
}


