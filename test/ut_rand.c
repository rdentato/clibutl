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


void simpletests(int size)
{
  int n = 0;
  int frq_1[2] = {0};
  int frq_2[4] = {0};

  double X1, X2;

  uint32_t t;

  logassert(sizeof(uint32_t)*CHAR_BIT == 32);

  for (int k=0; k<size; k++) {
    t = utlrand();
    for (int j=0; j<32; j++) {
      frq_1[t & 1]++;                 // Frequency test
      if (j<31) frq_2[t & 3]++;       // Serial test
      t >>= 1;
      n++;
    }
  }

  X1 = (double)(frq_1[0]-frq_1[1]);
  X1 = (X1 * X1) / n;

  X2 = (double)(frq_2[0] * frq_2[0]) + (double)(frq_2[1] * frq_2[1]) +
       (double)(frq_2[2] * frq_2[2]) + (double)(frq_2[3] * frq_2[3]);

  X2 = 4.0*X2/(n-1.0) ;
  X2 = X2 - (2.0/n) * (frq_1[0] * frq_1[0] + frq_1[1] * frq_1[1]) +1;


  loginfo("Statistical tests for %d bits",n);
  loginfo("n0:%d n1:%d",frq_1[0], frq_1[1]);
  loginfo("n00:%d n01:%d n10:%d n11:%d",frq_2[0], frq_2[1], frq_2[2], frq_2[3]);
  loginfo("X1 %f",X1);
  loginfo("X2 %f",X2);
  logexpect(X1<12,"X1: %f",X1);
  logexpect(X2<12,"X2: %f",X2);
}

void newfile(char *fname, int size)
{
   uint32_t t;
   FILE *fp;

   fp = fopen(fname,"wb");

   logassert(fp);

   loginfo("About to write: %d x %lu bytes (%lu) on '%s'",size,sizeof(uint32_t), size *sizeof(t),fname);
   for (int k=0; k<size;k++) {
     t = utlrand();
     fwrite(&t,sizeof(t),1,fp);
   }

   fclose(fp);
   logclose();

   logopen("l_rnd.log","a");

}

int main(int argc, char *argv[])
{
  
  logopen("l_rnd.log","w");
  loglevel("I,T");
  
  uint64_t s1,s2;

  s1 = 12;
  s2 = 545;

  utlsrand(s1,s2);

  utlrandstate(&s1,&s2);
  logcheck(s1 == 12 && s2 == 545);

  utlsrand(0);
  utlrandstate(&s1,&s2);
  logcheck(s1 != 12 && s2 != 545);

  #define MAX_NUM 6

  uint32_t n[MAX_NUM];
  uint32_t t;

  for (int i=0; i<MAX_NUM; i++) {
    t = utlrand();
    n[i] = t;
    loginfo("%u %u",i,t);
  }

  int k = 0;
  for (int i=0; i<MAX_NUM; i++) {
    t = utlrand();
    if (n[i] == t) k++;
    loginfo("%u %u",i,t);
  }
  logcheck(k < MAX_NUM-1);

  utlsrand(s1,s2);
  k=0;
  for (int i=0; i<MAX_NUM; i++) {
    t = utlrand();
    if (n[i] == t) k++;
    loginfo("%u %u",i,t);
  }
  logexpect(k == MAX_NUM,"k: %d",k);

  newfile("l_rnd.dat",50000); 
  //simpletests(10000);
  logclose();
  exit(0);
}
