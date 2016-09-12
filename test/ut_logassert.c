#include "utl.h"

int main(int argc, char *argv[])
{
  logopen("t_logassert.log","w");
  
  logassert(5>10);
  logassert(utl_ret(0));
  logcheck(3/utl_ret(0) == 0);
  logclose();
}
