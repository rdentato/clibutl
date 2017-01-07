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
#define UTL_MEMCHECK
#include "utl.h"
#include <math.h>


#define OUTOFMEM   1
#define NOFILE     2
#define WRONGINPUT 3
#define PANIC      5

void myfunc(int ex)
{
  logprintf("Throwing %d",ex);
  throw(ex);
}

int main(int argc, char *argv[])
{
  
  int k = 0;
  
  logopen("l_try.log","w");
  
  try {
    logprintf("No exception");
    k=1;
  }
  catchall {
    logprintf("Catch all");
    k = 9;    
  }
  logcheck(k==1);
  
  try {
    logprintf("Exception NOFILE");
    throw(NOFILE);
  }
  catch(NOFILE) {
    k = 20;  logprintf("NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("OUT OF MEMORY");
  }  
  catchall {
    k = 30;  logprintf("DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==20);
 
  try {
    logprintf("Exception NOFILE");
    throw(OUTOFMEM);
  }
  catch(NOFILE) {
    k = 20;  logprintf("NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==10);
 
  try {
    logprintf("Exception unhandled");
    throw(PANIC);
  }
  catch(NOFILE) {
    k = 20;  logprintf("NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==30);
  
  k = 0;
  try {
    logprintf("Exception unhandled (no catch all)");
    k=7; // WARNING! Local vars will be reverted to the previous value
    throw(PANIC);
    k=9; // Won't be executed
  }
  catch(NOFILE) {
    k = 20;  logprintf("NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("OUTOFMEM");
  }  
  if (!logcheck(k==0)) logprintf("k: %d",k);
 

  k = 0;
  try {
    logprintf("throw from a function");
    myfunc(OUTOFMEM);
  }
  catch(NOFILE) {
    k = 20;  logprintf("NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==10);

  k = 0;
  try {
    logprintf("Nested try");
    try {
      throw(NOFILE);
    }
    catch(NOFILE) {
      k=2;
    }
  }
  catch(NOFILE) {
    k = 20;  logprintf("NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==2);


  logclose();
  exit(0);
}
