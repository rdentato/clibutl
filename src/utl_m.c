#line 2 "src/utl_hdr.c"
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
*/
#line 93 "src/utl_hdr.c"
#define UTL_MAIN
#include "utl.h"

const char *utl_emptystring = "";

int   utl_ret(int x)      {return x;}
void *utl_retptr(void *x) {return x;}

#ifndef UTL_NOTRY
utl_jb_t *utl_jmp_list = NULL; // For try/catch
#endif

/* * Collection of hash functions * */

/* Bob Jenkins' "one_at_a_time" hash function
**  http://burtleburtle.net/bob/hash/doobs.html
*/
uint32_t utl_hash_string(void *key)
{
  uint32_t h = 0x071f9f8f;
  uint8_t *k = key;
  
  while (*k)  {
    h += *k++;
    h += (h << 10);
    h ^= (h >> 6);
  }
  h += (h << 3);
  h ^= (h >> 11);
  h += (h << 15);
  
  return h;
}

/* Bob Jenkins' integer hash function
**  http://burtleburtle.net/bob/hash/integer.html
*/

uint32_t utl_hash_int32(void *key)
{
  uint32_t h = *((uint32_t *)key);
  h = (h+0x7ed55d16) + (h<<12);
  h = (h^0xc761c23c) ^ (h>>19);
  h = (h+0x165667b1) + (h<<5);
  h = (h+0xd3a2646c) ^ (h<<9);
  h = (h+0xfd7046c5) + (h<<3);
  h = (h^0xb55a4f09) ^ (h>>16);
  return h;
}

/* Quick and dirty PRNG */
/*
uint32_t utl_rnd()
{ // xorshift
  static uint32_t rnd = 0;
  while (rnd == 0) rnd = (uint32_t)time(0);
	rnd ^= rnd << 13;
	rnd ^= rnd >> 17;
	rnd ^= rnd << 5;
	return rnd;
}
*/
/*
uint32_t utl_rnd()
{ // Linear Congruetial
  static uint32_t rnd = 0;
  if (rnd == 0) rnd = (uint32_t)time(0);
	rnd = 1664525 * rnd + 1013904223;
	return rnd;
}
*/
/* returns log2(n) assuming n is 2^m */
int utl_unpow2(int n)
{ int r;

  r  =  (n & 0xAAAA) != 0;
  r |= ((n & 0xCCCC) != 0) << 1;
  r |= ((n & 0xF0F0) != 0) << 2;
  r |= ((n & 0xFF00) != 0) << 3;
  return r;
}


#line 286 "src/utl_log.c"
#ifndef UTL_NOLOG
#ifdef UTL_MAIN

FILE *utl_log_file = NULL;
uint32_t utl_log_check_num   = 0;
uint32_t utl_log_check_fail  = 0;
int16_t utl_log_dbglvl = 0;
int16_t utl_log_prdlvl = 0;
const char *utl_log_w = "w";

log_watch_t *utl_log_watch = NULL;

int utl_log_close(const char *msg)
{
  int ret = 0;
  
  if (utl_log_check_num) {
    logprintf("CHK #KO: %d (of %d)",utl_log_check_fail,utl_log_check_num);
    utl_log_check_fail = 0;
    utl_log_check_num = 0;
  }
  if (msg) logprintf("%s",msg);
  if (utl_log_file && utl_log_file != stderr) ret = fclose(utl_log_file);
  utl_log_file = NULL;
  return ret;
}

FILE *utl_log_open(const char *fname, const char *mode)
{
  char md[4];
  md[0] = (mode && *mode == 'w')? 'w' : 'a';
  md[1] = '+';
  md[2] = '\0';
  utl_log_close(NULL);
  utl_log_file = fopen(fname,md);
  logprintf("LOG START");
  utl_log_check_num = 0;
  utl_log_check_fail = 0;
  return utl_log_file;
}

int utl_log_time(void)
{
  char       log_tstr[32];
  time_t     log_time;
  struct tm *log_time_tm;
  int        ret = 0;
  
  if (!utl_log_file) utl_log_file = stderr;
  if (time(&log_time) == ((time_t)-1)) ret = -1;
  if (ret >= 0 && !(log_time_tm = localtime(&log_time))) ret = -1;
  if (ret >= 0 && !strftime(log_tstr,32,"%Y-%m-%d %H:%M:%S",log_time_tm)) ret =-1;
  if (ret >= 0) ret = fprintf(utl_log_file,"%s ",log_tstr);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  
  return ret;
}

int utl_log_check(int res, const char *test, const char *file, int32_t line)
{
  int ret = 0;
  
  if (utl_log_dbglvl > UTL_LOG_D) return 1;
  
  ret = utl_log_time();
  
  if (ret >= 0) ret = fprintf(utl_log_file,"CHK %s (%s)?\x09:%s:%d\x09\n", (res?"PASS":"FAIL"), test, file, line);
  if (ret >= 0 && fflush(utl_log_file)) ret = -1;
  if (!res) utl_log_check_fail++;
  utl_log_check_num++;
  return res;
}

void utl_log_assert(int res, const char *test, const char *file, int32_t line)
{
  if (!utl_log_check(res,test,file,line)) {
    logprintf("CHK ASSERTION FAILED");
    logclose();
    abort();
  }
}

void utl_log_watch_check(char *buf, log_watch_t *lwatch, const char *file, int32_t line)
{ 
  int k=0;
  char *p;
  int expected = 1;
  int res = 1;
  pmx_t pmx_state;
  char **watch;
  
  _logprintf("XXX %s (%p)",buf,(void*)lwatch);
  if (!lwatch) return;
  watch = lwatch->watch;
  for (k=0; k<UTL_LOG_WATCH_SIZE; k++) {
    expected = 1;
    p = watch[k];
    _logprintf(">>> %s",p?p:"");
    if (p) {
      if (p[0] == '\1' && p[1] == '\0') break;
      if (p[0] == '!') {p++; expected = (p[0]=='!'); }
     
      _logprintf("?? err:%d exp:%d %s %s [%s]",utl_log_check_fail,expected,(char *)watch[k],p,buf);
      pmxclear(&pmx_state);
    	res = pmxsearch(p,buf) != NULL;
      pmxrestore(&pmx_state);
      if (res) {
        utl_log_check(expected,watch[k],file,line);
        if (expected) watch[k] = NULL;
      }
    }
  }
}

void utl_log_watch_last(log_watch_t *lwatch, const char *file, int32_t line)
{ 
  /* The  only tests in `watch[]` should be the ones with `!` at the beginning */
  int k;
  int expected = 0;
  char *p;
  char **watch;
  
  if (!lwatch) return;
  watch = lwatch->watch;
  for (k=0; k<UTL_LOG_WATCH_SIZE;k++) {
    p = watch[k];
    expected = 0;
    if (p) {
      if (p[0] == '\1' && p[1] == '\0') break;
      if (p[0] != '!') expected = 1;
      utl_log_check(!expected,(char *)(watch[k]),file,line);
    }
  }
}

void utl_log_setlevel(const char *lvl) {
  int l = 0;
  if (lvl) {
    if (*lvl == '*') lvl = "I,T";
    l = 2;
    switch (toupper(*lvl)) {
      case 'N' : l++; 
      case 'E' : l++; 
      case 'W' : l++; 
      case 'I' : utl_log_prdlvl = l; 
                 break;
    }

    while (*lvl && *lvl!=',') lvl++;
    if (*lvl) {
      lvl++;
      l = 0;
      switch (toupper(*lvl)) {
        case 'N' : l+=4; 
        case 'D' : l++; 
        case 'T' : utl_log_dbglvl = l; 
                   break;
      }
    }
  }
}

#endif
#endif
