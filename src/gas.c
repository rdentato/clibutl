
#include <stdio.h>
#include <math.h>
#include "wish.h"

/* PROBLEM: Determnine if a gas is "perfect" using the equation pV=RT */

const double R = 8.314472;

/* PURPOUSE: return 1 if it's a perfect gas.
             return 0 if it's not
*/
int isperfect(double P, double V, double T)
{
  int ret = 0;
  // I WISH!!!!!
  return ret;
}


int main(int argc, char *argv[])
{
  int ret=0;
  
  
  
  wishEQdbl(0,isperfect(1.,1.,1.));
  wishEQdbl(1,isperfect(1.,R,1.));
  
  return ret;
}

