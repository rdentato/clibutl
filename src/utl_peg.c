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

//<<<//
#ifndef UTL_NOPEG

int utl_peg_str(const char *pat, const char *str)
{ int k = 0;
  if (!(str && *str)) return -1;
  while (*str && *pat && (*str == *pat)) {str++; pat++;k++;}
  return (*pat == '\0'? k: -1);
}

int utl_peg_oneof(const char *pat, const char *str)
{
  if (!(str && *str)) return -1;
  return (strchr(pat,*str)?1:-1);
}

int utl_peg_eol(const char *str)
{
  int ret;
  switch (*str) {
    case '\0' : ret = 0; break;
    case '\n' : ret = 1; break;
    case '\r' : ret = (str[1] == '\n') ? 2 : 1;    
    default   : ret = -1;
  }
  return ret;
}

int utl_peg_wspace(const char *str)
{
  int ret = 0;
  while (1) {
    switch (*str) {
      case '\b' : case '\a':
      case ' ' : case '\t': case '\xA0' : ret++; str++; break;
      default: return ret;
    }
  }
  return ret;
}

int utl_peg_vspace(const char *str)
{
  int ret = 0;
  while (1) {
    switch (*str) {
      case '\r' : case '\n': case '\v' : case '\f': case '\b' : case '\a':
      case ' ' : case '\t': case '\xA0' : ret++; str++; break;
      default: return ret;
    }
  }
  return ret;
}


static peg_t utl_peg_init(peg_t p, const char *s)
{
  if (p) {
    p->start    = s;
    p->pos      = s;
    p->aux      = NULL;
    p->auxi     = 0;
    p->fail     = 0;
    p->errpos   = s;
    p->errrule  = NULL;
    p->errln    = 1;
    p->errcn    = 1;
    p->errmsg   = NULL;
    if (s == utl_emptystring) {
      p->defer = NULL;
      p->mmz   = NULL;
    }
    if (p->defer)  vecclear(p->defer);
             else  p->defer = vecnew(pegdefer_t);
             
    if (p->mmz)  vecclear(p->mmz);
           else  p->mmz = vecnew(utl_peg_mmz_t *);
  }
  return p;
}

peg_t utl_peg_new()
{
  peg_t p;
  p = malloc(sizeof(struct peg_s));
  if (p) {
    p->defer = NULL;
    utl_peg_init(p,utl_emptystring);
  }
  return p; 
}

peg_t utl_peg_free(peg_t parser)
{
  if (parser) {
    if (parser->defer) parser->defer=vecfree(parser->defer);
    free(parser);
  }
  return NULL;
}

const char *utl_peg_defer(peg_t parser, pegaction_t func,const char *from, const char *to)
{
  pegdefer_t defer;
  
  if (!parser->fail) {
    defer.func = func;
    defer.from = from;
    defer.to   = to;
    vecpush(pegdefer_t, parser->defer, defer);
    //logdebug("Deferd: %p %p %p",(void *)func,(void *)from, (void *)to);
  }
  return NULL;
}

const char *utl_peg_back(peg_t parser,const char *rule_name, const char *pos,int32_t dcnt)
{
  int32_t cnt;
  if (parser->errpos <= pos) {
     parser->errpos  = pos;
     parser->errrule = rule_name;
     parser->errmsg = parser->errmsgtmp;
  }
  parser->pos = pos;
  cnt = veccount(parser->defer);
  if (dcnt < cnt) {
    cnt -= dcnt;
    vecdrop(parser->defer,cnt);
  }
  return NULL;  
}

void utl_peg_repeat(peg_t peg_, const char *pegr_, pegsave_t *peg_save)
{
  peg_save->rpt++;
  if (!PEG_FAIL) {
    peg_save->rlen  = PEG_POS - peg_save->rpos;
    peg_save->rpos  = PEG_POS;
    peg_save->rdcnt = PEG_DCNT;
  }
  if (PEG_FAIL || peg_save->rpt >= peg_save->max || peg_save->rlen == 0) {
      if (PEG_FAIL < 0 || (PEG_FAIL = (peg_save->rpt <= peg_save->min))) {
        PEG_BACK(peg_save->pos,peg_save->dcnt);
      }
      else {
        PEG_DCNT = peg_save->rdcnt;
        PEG_POS = peg_save->rpos;
      }
      peg_save->max = 0;
  }
}

static void utl_peg_memoize(peg_t parser, const char *startpos, int32_t startcnt, 
                                                 const char *rule_name, utl_peg_mmz_t *mmz)
{
  int i;
  pegdefer_t *defer;
  
  if (mmz) {
    if (mmz[0].startpos == NULL && mmz[0].startpos == mmz[1].startpos) {
      // First time
      vecpush(utl_peg_mmz_t *, parser->mmz, mmz); // to be cleaned at the end of parsing
      mmz[0].defer = vecnew(pegdefer_t);
      mmz[1].defer = vecnew(pegdefer_t);
      // logdebug("Added: %p",(void *)mmz);
    }
    
    i = (mmz[0].startpos < mmz[1].startpos)? 0:1;
    mmz[i].fail = parser->fail;
    mmz[i].startpos = startpos;
    mmz[i].endpos = parser->pos;
    vecclear(mmz[i].defer);
    while (startcnt < veccount(parser->defer)) {
      defer = vecgetptr(parser->defer,startcnt);
      vecaddptr(mmz[i].defer, defer);
      startcnt++;
    }
    // logdebug("Stored: %s@%p %d %p %d %d",rule_name,mmz[i].startpos,mmz[i].fail,mmz[i].endpos,0,veccount(mmz[i].defer));
  }
}

static void utl_peg_cleanmmz(peg_t parser)
{
  utl_peg_mmz_t *mmz;
  
  while (veccount(parser->mmz) > 0) {
    mmz = vectop(utl_peg_mmz_t *,parser->mmz,NULL);
    mmz[0].startpos = NULL;
    mmz[1].startpos = NULL;
    mmz[0].defer = vecfree(mmz[0].defer);
    mmz[1].defer = vecfree(mmz[1].defer);
    // logdebug("Cleared: %p",(void *)mmz);
    vecdrop(parser->mmz);
  }
}

static int utl_peg_gotmmz(peg_t parser, const char *startpos, int32_t startcnt, const char *rule_name, utl_peg_mmz_t *mmz)
{
  pegdefer_t *defer;
  
  if (mmz) {
    for (int i=0; i<2; i++) {
      if (mmz[i].startpos == parser->pos) {
        parser->fail = mmz[i].fail;
        parser->pos  = mmz[i].endpos;
        for (int k = 0; k < veccount(mmz[i].defer); k++) {
          defer = vecgetptr(mmz[i].defer,k);
          vecaddptr(parser->defer,defer);
        }
        // logdebug("Restor: %s@%p %d %p %d %d",rule_name,startpos,parser->fail,parser->pos,startcnt,veccount(parser->defer));
        return 1;
      }
    }
  }
  return 0;
}

void utl_peg_ref(peg_t parser, const char *rule_name, pegrule_t rule, utl_peg_mmz_t *mmz)
{
  const char *tmp = parser->pos;
  int32_t cnt = veccount(parser->defer);
  // int i = 0;
  // pegdefer_t *defer;
  
  if (!parser->fail) {
    //logdebug("Start:  %s@%p",rule_name,parser->pos);
    /* check result */
    if (!utl_peg_gotmmz(parser, tmp, cnt, rule_name, mmz)) {
      rule(parser,rule_name);
      if (parser->fail) {
        (void)utl_peg_back(parser,rule_name,tmp,cnt);
      }
      //logdebug("Result: %s@%p %d %p %d %d",rule_name,tmp,parser->fail,parser->pos,cnt,veccount(parser->defer));
      /* store result */
      utl_peg_memoize(parser,tmp,cnt,rule_name,mmz);
    }
  }
}

static int peg_defer_func_NULL(const char *from, const char *to, void *aux)
{ return 1; }

static void utl_peg_seterrln(peg_t parser)
{
  const char *s;
  
  if (*parser->errpos) {
    s = parser->start;
    parser->errln = 1;
    parser->errcn = 1;
    parser->errptr = s;
    while (*s && s < parser->errpos) {
      if (s[0] == '\r' && s[1] == '\n') s++;
      if (*s == '\r' || *s == '\n') {
        parser->errptr=s+1;
        parser->errln++;
        parser->errcn=0;
      }
      s++;
      parser->errcn++;
    }  
  }
}

static void utl_peg_execdeferred(peg_t parser)
{
  pegdefer_t defer;
  pegdefer_t defer_NULL = {peg_defer_func_NULL, NULL, NULL};
  
  //logdebug("Deferred actions: %d",veccount(parser->defer));
  defer = vecfirst(pegdefer_t, parser->defer, defer_NULL);
  while (defer.func != peg_defer_func_NULL) {
    //logdebug("Execd: %p %p %p",(void *)defer.func,(void *)defer.from, (void *)defer.to);
    if (defer.func(defer.from, defer.to, parser->aux))
      break;
    defer = vecnext(pegdefer_t, parser->defer, defer_NULL);
  }
}

int utl_peg_parse(peg_t parser, pegrule_t start_rule, utl_peg_mmz_t *mmzptr, 
                const char *txt,const char *rule_name, void *aux)
{
  if (parser && start_rule && txt) {
    utl_peg_init(parser,txt);
    parser->aux = aux;
    utl_peg_ref(parser, rule_name, start_rule, NULL);

    if (!parser->fail) {
      if (parser->errpos <= parser->pos) {
        parser->errpos  = parser->pos;
        parser->errrule = rule_name;
        parser->errmsg = parser->errmsgtmp;
      }
      utl_peg_execdeferred(parser);
    }
    utl_peg_seterrln(parser);
    utl_peg_cleanmmz(parser);
    return !parser->fail;
  }
  return 0;
}

#endif
//>>>//
