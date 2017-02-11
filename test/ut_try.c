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


#define OUTOFMEM   0x0
#define NOFILE     0x1
#define WRONGINPUT 0x2
#define PANIC      0xF

void myfunc(int ex)
{
  logprintf("    Throwing %d",ex);
  throw(ex);
}

int main(int argc, char *argv[])
{
  
  int k = 0;
  
  logopen("l_try.log","w");
#if 1  
  try {
    logprintf("    No exception");
    k=1;
  }
  catchall {
    logprintf("   Catch all (caught: %d)",thrown());
    k = 9;    
  }
  logcheck(k==1);
  
  try {
    logprintf("     Exception NOFILE");
    throw(NOFILE,1);
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
    logexpect(thrown() == NOFILE,"exc: %d",thrown());
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("    OUT OF MEMORY");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==20);
 
  try {
    logprintf("     Exception NOFILE (single catch)");
    throw(NOFILE,1);
  }
  catch(NOFILE,OUTOFMEM) {
    logexpect(thrown() == NOFILE,"exc: %d",thrown());
    k = 20;  logprintf("    NO FILE FOUND OR OUT OF MEMORY");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==20);

  try {
    logprintf("     Exception OUTOFMEM (single catch)");
    throw(OUTOFMEM,1);
  }
  catch(NOFILE,OUTOFMEM) {
    logexpect(thrown() == OUTOFMEM,"exc: %d",thrown());
   k = 20;  logprintf("    NO FILE FOUND  OR OUT OF MEMORY");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==20);

  try {
    logprintf("    Exception NOFILE");
    throw(OUTOFMEM,1);
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("    OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==10);
 
  try {
    logprintf("    Exception unhandled");
    throw(PANIC,1);
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("    OUTOFMEM");
  }  
  catchall {
    logexpect(thrown() == PANIC,"exc: %d",thrown());
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==30);
  
  k = 0;
  try {
    logprintf("    Exception unhandled (no catch all)");
    k=7; // WARNING! Local vars might be reverted to the previous value
    throw(PANIC,0);
    k=9; // Won't be executed
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("    OUTOFMEM");
  }  
  // At this point k is undefined!! 
  //(with GCC would be 0, with MSVC would be 7)
  logprintf("    undefined k: %d",k);
  
#endif
 
  k = 652;
  try {
    logprintf("    throw from a function");
    logtrace("jb+: %p",(void *)&utl_jb);
    myfunc(OUTOFMEM);
    logtrace("     SHOULDN'T BE HERE");
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("    OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  if (!logcheck(k==10)) logprintf("k: %d",k);

  k = 0;
  try {
    logprintf("    Nested try");
    try {
      myfunc(NOFILE);
    }
    catch(NOFILE) {
      k=2;
    }
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10;  logprintf("    OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==2);

  k = 0;
  try {
    logprintf("    Nested try");
    try {
      throw(OUTOFMEM,100);
    }
    catch(NOFILE) {
      k=2;
    }
    catchall { k=3;rethrow();}
  }
  catch(NOFILE) {
    k = 20;  logprintf("    NO FILE FOUND");
  }  
  catch(OUTOFMEM) {
    k = 10 + thrownid();  logprintf("    OUTOFMEM");
  }  
  catchall {
    k = 30;  logprintf("    DON'T KNOW WHAT'S WRONG!");
  }
  logcheck(k==110);
  

  logclose();
  exit(0);
}
