#ifndef UTL_NOADT
/** VECTORS **/

typedef struct vec_s {
  uint32_t  max;
  uint32_t  cnt;
  uint32_t  esz;
  uint32_t  first;
  uint32_t  last; 
  void     *vec;
} *vec_t;

vec_t utl_vecNew(uint32_t esz);
#define vecNew(ty) utl_vecNew(sizeof(ty))

vec_t utl_vecFree(vec_t v);
#define vecFree utl_vecFree

int utl_vecSet(vec_t v, uint32_t i, void *e);
#define vecSetRaw utl_vecSet
#define vecSet(ty,v,i,e) do { ty x = e; utl_vecSet(v,i,&x);} while(utlZero)

int utl_vecFill(vec_t v, uint32_t j, uint32_t i, void *e);
#define vecFillRaw utl_vecFill
#define vecFill(ty,v,j,i,e) do { ty x = e; utl_vecFill(v,j,i,&x);} while(utlZero)


int utl_vecAdd(vec_t v, void *e);
#define vecAddRaw  utl_vecAdd
#define vecAdd(ty,v,e) do { ty x = e; utl_vecAdd(v,&x);} while(utlZero)

int utl_vec_ins(vec_t v, uint32_t n, uint32_t l,void *e);
#define vecInsRaw(v,i,e) utl_vec_ins(v,i,1,e);
#define vecIns(ty,v,i,e) do { ty x = e; utl_vec_ins(v,i,1,&x);} while(utlZero)
#define vecInsGap(v,i,n) utl_vec_ins(v,i,n,NULL)

int utl_vec_valid_ndx(vec_t v, uint32_t i);

#define utl_vec_getptr(v,i)  (((char *)((v)->vec)) + ((i)*v->esz))

void *utl_vec_get(vec_t v, uint32_t  i);
#define vecGetRaw utl_vec_get
#define vecGet(ty,v,i,d) (utl_vec_valid_ndx(v,i) ? *((ty *)(utl_vec_getptr(v,i))) : d)

int utl_vecResize(vec_t v, uint32_t n);
#define vecResize utl_vecResize

uint32_t utl_vecCount(vec_t v);
#define vecCount     utl_vecCount

uint32_t utl_vecMax(vec_t v);
#define vecMax     utl_vecMax

uint32_t utl_vecEsz(vec_t v);
#define vecEsz     utl_vecEsz

void  *utl_vecVec(vec_t v);
#define vec(ty, v) ((ty *)utl_vecVec(v))

#ifdef UTL_LIB

vec_t utl_vecNew(uint32_t esz)
{
  vec_t v;
  v = malloc(sizeof(struct vec_s));
  if (v) {
    v->max = 0;    v->cnt = 0;
    v->first = 0;  v->last = 0;
    v->esz = esz;  v->vec = NULL;
  }
  return v;
}

vec_t utl_vecFree(vec_t v)
{
  if (v) {
    if (v->vec) free(v->vec);
    v->max = 0;    v->cnt = 0;
    v->first = 0;  v->last = 0;
    v->esz = 0;    v->vec = NULL;
    free(v);
  }
  return NULL;
}

uint32_t utl_vecCount(vec_t v) { return v? v->cnt : 0; }
uint32_t utl_vecMax(vec_t v)   { return v? v->max : 0; }
uint32_t utl_vecEsz(vec_t v)   { return v? v->esz : 0; }
void  *utl_vecVec(vec_t v)   { return v? v->vec : NULL; } 

int utl_vec_valid_ndx(vec_t v, uint32_t i) { return (v && v->vec && i < v->cnt); }

static int utl_vec_expand(vec_t v, uint32_t i)
{
  if (utl_vec_valid_ndx(v,i)) return 1;
  return utl_vecResize(v,i);
}

int utl_vecResize(vec_t v, uint32_t n)
{
  uint32_t new_max = 1;
  char *new_vec = NULL;
  if (!v) return 0;

  while (new_max <= n) new_max *= 2;
  
  if (new_max != v->max) {
    new_vec = realloc(v->vec, new_max * v->esz);
    if (!new_vec) return 0;
    v->vec = new_vec;
    v->max = new_max;
    if (v->cnt > v->max) v->cnt = v->max; /* in case of shrinking */
  }
  return 1;
}

#define utl_vec_cpy(v,i,e) memcpy(((char *)(v->vec)) + (i * v->esz),e,v->esz)

int utl_vecSet(vec_t v, uint32_t  i, void *e)
{
  if (!utl_vec_expand(v,i)) return 0;
  if (e) utl_vec_cpy(v,i,e);
  if (i >= v->cnt) v->cnt = i+1;
  return 1;
}

int utl_vecFill(vec_t v, uint32_t j, uint32_t i, void *e)
{
  char *p;
  uint32_t sz;
  
  if ((j > i) || !utl_vec_expand(v,i)) return 0;
  sz = v->esz;
  p = ((char *)(v->vec)) + (j * sz);  
  while ( j<=i) {
    memcpy(p,e, sz);
    j++, p += sz;
  }
  if (i >= v->cnt) v->cnt = i+1;
  return 1;
}

void *utl_vec_get(vec_t v, uint32_t i)
{
  if (!utl_vec_valid_ndx(v,i)) return NULL;
  return utl_vec_getptr(v,i);
}

void *utl_vecTop(vec_t v)
{
  if (!v || !v->cnt) return NULL;
  return ((char *)(v->vec)) + ((v->cnt-1)*v->esz);
}

void utl_vecPop(vec_t v) { if (v && v->cnt) v->cnt -= 1; }

int utl_vecAdd(vec_t v, void *e) { return utl_vecSet(v,v->cnt,e); }

int utl_vec_ins(vec_t v, uint32_t i, uint32_t l,void *e)
{
  char *b;
  uint32_t sz;
  
  if (!v) return 0;
  if (l == 0) return 1;
  if (i > v->cnt) i = v->cnt;

  if (!utl_vec_expand(v,v->cnt+l+1)) return 0;
  sz = v->esz;
  b = v->vec+i*sz;
  l = l*sz;
  memmove(b+l, b, (v->cnt-i)*sz);
  if (e)  memcpy(b,e,l);
  else memset(b,0x00,l);
  v->cnt += l;
  return 1;
}

#endif  /* UTL_LIB */

/** STACKS **/
 
#define stk_t             vec_t
#define stkNew(ty)        vecNew(ty)
#define stkFree           vecFree
#define stkCount(s)       vecCount(s)
#define stkEmpty(s)     (!vecCount(s))
#define stkPushRaw(s,e)   vecAddRaw(s,e)
#define stkPush(ty,s,e)   vecAdd(ty,s,e)
#define stkTopRaw(s)      utl_vecTop(s)
#define stkTop(ty,s,d)   (vecCount(s)? *((ty *)utl_vecTop(s)) : d)
#define stkPop(s)         utl_vecPop(s)

/**  BUFFERS **/
#define buf_t vec_t
int utl_bufSet(buf_t bf, uint32_t i, char c);

#define bufNew() utl_vecNew(1)
#define bufFree  utl_vecFree

char utl_bufGet(buf_t bf, uint32_t i);
#define bufGet   utl_bufGet

int utl_bufSet(buf_t bf, uint32_t i, char c);
#define bufSet   utl_bufSet

int utl_bufAdd(buf_t bf, char c);
#define bufAdd   utl_bufAdd

int utl_bufAddStr(buf_t bf, char *s);
#define bufAddStr  utl_bufAddStr

int utl_bufIns(buf_t bf, uint32_t i, char c);
#define bufIns(b,i,c) utl_bufIns(b,i,c)

int utl_bufInsStr(buf_t bf, uint32_t i, char *s);
#define bufInsStr(b,i,s) utl_bufInsStr(b,i,s)

#define bufResize utl_vecResize

#define bufClr(bf) utl_bufSet(bf,0,'\0');

int utl_bufFormat(buf_t bf, char *format, ...);
#define bufFormat utl_bufFormat

#define bufLen vecCount
#define bufMax vecMax
#define bufStr(b) vec(char,b)

int utl_bufAddLine(buf_t bf, FILE *f);
#define bufAddLine   utl_bufAddLine
#define bufReadLine  utl_bufAddLine

int utl_bufAddFile(buf_t bf, FILE *f);
#define bufAddFile   utl_bufAddFile
#define bufReadFile  utl_bufAddFile

#if !defined(UTL_HAS_SNPRINTF) && defined(_MSC_VER) && (_MSC_VER < 1800)
#define UTL_ADD_SNPRINTF
#define snprintf  c99_snprintf
#define vsnprintf c99_vsnprintf
#endif

#ifdef UTL_LIB
int utl_bufSet(buf_t bf, uint32_t i, char c)
{
  char *s;

  if (!utl_vec_expand(bf,i+1)) return 0;
  s = bf->vec;
  s[i] = c;
  if (c == '\0') {
    bf->cnt = i;
  }
  else if (i >= bf->cnt) {
    s[i+1] = '\0';
    bf->cnt = i+1;
  }
  return 1;
}

char utl_bufGet(buf_t bf, uint32_t i)
{
  if (!bf) return '\0';
  if (i >= bf->cnt) return '\0';
  return ((char*)(bf->vec))[i];
}

int utl_bufAdd(buf_t bf, char c)
{  return utl_bufSet(bf,bf->cnt,c); }

int utl_bufAddStr(buf_t bf, char *s)
{
  if (!bf) return 0;
  if (!s || !*s) return 1;
  
  while (*s) if (!utl_bufSet(bf,bf->cnt,*s++)) return 0;
  
  return utl_bufAdd(bf,'\0');
}

/* A line in the file can be ended by '\r\n', '\n' or '\r'.
** The NEWLINE characters are discarded.
** The string in the buffer is terminated with '\n\0'. 
*/
int utl_bufAddLine(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case '\r' : if ((c = fgetc(f)) != '\n') ungetc(c,f);
      case '\n' :
      case EOF  : c = EOF; utl_bufAdd(bf,'\n'); break;
      default   : utl_bufAdd(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

int utl_bufAddFile(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case EOF  : c = EOF; break;
      default   : utl_bufAdd(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

static int utl_buf_insert(buf_t bf, uint32_t i, uint32_t l, char *s)
{
  int n = 1;
  int k;
  char *b;
  
  if (!bf) return 0;
  if (!s)  return 1;
  if (l == 0) l = strlen(s);
  if (l == 0) return 1;
  if (i > bf->cnt) i = bf->cnt;

  if (!utl_vec_expand(bf,bf->cnt+l+1)) return 0;
  
  b = bf->vec+i;
  k = bf->cnt;
  memmove(b+l, b, k-i);
  memcpy(b,s,l);
  utl_bufSet(bf, k+l, '\0');
  return n;
}

int utl_bufInsStr(buf_t bf, uint32_t i, char *s)
{ return utl_buf_insert(bf,i,0,s); }

int utl_bufIns(buf_t bf, uint32_t i, char c)
{ return utl_buf_insert(bf,i,1,&c); }


/* {{ code from http://stackoverflow.com/questions/2915672 */
#ifdef UTL_ADD_SNPRINTF

inline int c99_snprintf(char* str, uint32_t size, const char* format, ...)
{
  int count;
  va_list ap;
  va_start(ap, format);
  count = c99_vsnprintf(str, size, format, ap);
  va_end(ap);
  return count;
}

inline int c99_vsnprintf(char* str, uint32_t size, const char* format, va_list ap)
{
  int count = -1;
  if (size != 0)   count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
  if (count == -1) count = _vscprintf(format, ap);
  return count;
}
#endif /* UTL_ADD_SNPRINTF */
/* }} */

int utl_bufFormat(buf_t bf, char *format, ...)
{
  int count;
  int count2;
  va_list ap;

  if (!bf) return -1;
  
  va_start(ap, format);
  count = vsnprintf(NULL,0,format, ap);
  va_end(ap);
  utl_bufSet(bf,count,'\0'); /* ensure there's enough room */
  va_start(ap, format);
  count2 = vsnprintf(bufStr(bf),count+1,format, ap);
  va_end(ap);
  
  utlAssert(count == count2);
  
  return count2;
}
#endif /* UTL_LIB */


/**  QUEUE **/
#define que_t vec_t

#define queDelFirst(q)  utl_que_del(q, 'F')
#define queDelLast(q)   utl_que_del(q, 'B')
#define queDel          queDelFirst
int utl_que_del(que_t qu, char where);

#define queFirstRaw(q)    utl_que_get(q,'F')
#define queLastRaw(q)     utl_que_get(q,'B')
void *utl_que_get(que_t qu, char where);

#define queFirst(ty,q,d) (!(q) || queCount(q) == 0 ? d : *((ty *)queFirstRaw(q)))
#define queLast(ty,q,d)  (!(q) || queCount(q) == 0 ? d : *((ty *)queLastRaw(q)))

#define queAddFirstRaw(q,e)  utl_que_add(q,e,'F')
#define queAddLastRaw(q,e)   utl_que_add(q,e,'B')
#define queAddRaw queAddLastRaw
int utl_que_add(que_t qu, void *e,char where);

#define queAddFirst(ty,q,e)  do { ty x = e; utl_que_add(q,&x,'F'); } while (utlZero)
#define queAddLast(ty,q,e)   do { ty x = e; utl_que_add(q,&x,'B'); } while (utlZero)
#define queAdd queAddLast

#define queNew(ty) utl_queNew(sizeof(ty))
que_t utl_queNew(uint32_t esz);

#define queEmpty utl_queEmpty
int utl_queEmpty(que_t qu); 

#define queMax   vecMax
#define queCount vecCount
#define queEsz   vecEsz
#define queFree  vecFree

#define que_first(q) ((q)->first)
#define que_last(q)  ((q)->last)

#ifdef UTL_LIB

que_t utl_queNew(uint32_t esz)
{
  que_t qu = vecNew(esz);
  if (qu) {
    utl_vec_expand(qu,7);
  }
  return qu;
}

int utl_queEmpty(que_t qu) { return (qu == NULL || queCount(qu) == 0); }

int utl_que_expand(que_t qu)
{
  uint32_t new_max = 8;
  uint32_t old_max = 0;
  uint32_t n = 0;
  uint32_t sz;

  old_max = queMax(qu);
  if (queCount(qu) < old_max) return 1; /* there's room enough */
  
  new_max = old_max * 2;

  if (!utl_vec_expand(qu,new_max-1)) return 0; /* couldn't expand!! */
  
  /* Rearrange existing elements */
  n = qu->last;
  sz = queEsz(qu);
  if (n < old_max / 2) { /* move elements 0 .. n-1 to the new space */
     if (n > 0) memcpy(utl_vec_getptr(qu, old_max),  utl_vec_getptr(qu, 0),  n * sz);
     qu->last = old_max+n;
  } else { /* move elements n .. old_max-1 to the new space */
     n = old_max-n; /* Elements to move */
     memcpy(utl_vec_getptr(qu, new_max-n),  utl_vec_getptr(qu, qu->first),  n * sz);
     qu->first = new_max-n;
  }
  
  return 1;
}

int utl_que_add(que_t qu, void *e,char where)
{
  if (!utl_que_expand(qu)) return 0;
  if (where == 'B') {
    utl_vec_cpy(qu,qu->last,e);
    qu->last = (qu->last +1) % queMax(qu);
  }
  else {
    qu->first = (qu->first + vecMax(qu) - 1) % queMax(qu);
    utl_vec_cpy(qu,qu->first,e);
  }
  qu->cnt++;
  return 1;
}

int utl_que_del(que_t qu, char where)
{
  if (!qu) return 0;
  if (queCount(qu) == 0) return 1;
  if (where == 'B') 
    qu->last = (qu->last + queMax(qu) - 1) % queMax(qu);
  else 
    qu->first = (qu->first +1) % queMax(qu);
  qu->cnt--;
  return 1;
}

void *utl_que_get(que_t qu, char where)
{
  uint32_t n;
  if (queCount(qu) == 0) return NULL;
  if (where == 'B') 
    n = (qu->last + queMax(qu) - 1) % queMax(qu);
  else 
    n = qu->first;
    
  return utl_vec_getptr(qu, n);
}

#endif /* UTL_LIB */ 


/*
  SKIP LISTS

A skip list is stored within an array (which is a vec_t). 

.v
    3 - --------------------------> +
    2 - -----------> E -----------> +
    1 - ------> C -> E ------> K -> +
    0 - -> A -> C -> E -> G -> K -> + 
    
                                    1
      0  1  2  3  4  5  6  7  8  9  0  1  2  3  4  5  6  7  8  9
lst   +  -  6  8 11  0  A  8  C 11 11  E 15 17  0  G 17  K  0  0
      
      0  1  2  3 
path  
..

Each node is a portion of the array:

  [e 1 2 3 4 ...]
  
  e is the index (in the vector dat) of the
  element stored in the list.
 
*/


typedef struct lst_s {
  utl_cmp_t cmp;
  vec_t     lst; 
  vec_t     dat;  
  uint32_t  path[32];
  uint32_t  flst[32]; 
} *lst_t;

lst_t utl_lstNew(uint32_t esz, utl_cmp_t cmp);
lst_t utl_lstFree(lst_t ll);

void    *utl_lstFirst(lst_t ll);
void    *utl_lstNext(lst_t ll);
void    *utl_lstGet(lst_t ll,uint32_t i);
int      utl_lstAdd(lst_t ll,void *e);
int      utl_lstDel(lst_t ll,void *e);
uint32_t utl_lstSearch(lst_t ll,void *e);

#ifdef UTL_LIB

static const uint32_t utl_lst_NULL     = 0xFFFFFFF0;
static const uint32_t utl_lst_PLUSINF  = 0xFFFFFFF1;
static const uint32_t utl_lst_MINUSINF = 0xFFFFFFF2;

lst_t utl_lstNew(uint32_t esz, utl_cmp_t cmp)
{
  lst_t ll;
  
  ll = malloc(sizeof(struct lst_s));
  if (ll) {
     ll->cmp = cmp;
     if ( (ll->lst = vecNew(uint32_t)) ) {
       if ( (ll->dat = utl_vecNew(esz)) ) {
         vecFill(uint32_t, ll->lst, 0,64, 0);
         vecSet(uint32_t, ll->lst, 0, utl_lst_PLUSINF);
         vecSet(uint32_t, ll->lst, 1, utl_lst_MINUSINF);
       }
       else { vecFree(ll->lst); free(ll); ll = NULL; }
     }
     else { free(ll); ll = NULL; }
  }
  return ll;
}

lst_t utl_lstFree(lst_t ll)
{
  if (ll) {
    ll->dat = vecFree(ll->dat);
    ll->lst = vecFree(ll->lst);
    free(ll);
  }
  return NULL;
}

#define lst_DATA(i)    ((i) & 0x07FFFFFF)
#define lst_HEIGHT(i)  (((i) >> 27)+1)

static int utl_lst_cmp(lst_t ll, uint32_t i, void *e)
{
  void *p;
  uint32_t j;
  
  if (i == 0) return  1;
  if (i == 1) return -1;

  j = vecGet(uint32_t, ll->lst, i,0);
  p = vecGetRaw(ll->dat,lst_DATA(j));
  return ll->cmp(p,e);
}

#define lst_maxheight(l) ((l)->lst->first)
uint32_t lstSearch(lst_t ll, void *e)
{
  uint32_t ret = 0;
  int lvl;
  uint32_t *lst;
  int res;
  
  if (ll) {
    lvl = lst_maxheight(ll);
    lst = vec(uint32_t,ll->lst);
    while(lvl-- > 0) {
      ll->path[lvl] = 1;
      while ( (res = utl_lst_cmp(ll,ll->path[lvl],e)) < 0) {
        ll->path[lvl] =  vecGet(uint32_t, ll->lst, ll->path[lvl],0);
      }
      if (res == 0) { ret = ll->path[lvl]; break; }
    }
  }
  return ret;
}

int utl_lstnewnode(lst_t ll, int lvl)
{
  int ret = 0;
  
  /* Is there any node of level lvl in the free list? */
  if (0) {
  }
  else {
  /* Get a new node at the end */
  
    ret = vecCount(ll->lst);
    vecAdd(int, ll->lst, ((lvl-1) << 27) | vecCount(ll->dat) );
    vecAddRaw(ll->dat,NULL);
    
    
    ll->lst->vec[ret+1] = lvl;
    ll->lst->cnt += lvl+2;
  }
  
  return ret;
}

static int utl_lstrand()
{
  static int seed = -1;
  
  if (seed == -1) srand(time(0));
  return rand();
}

int utl_lstAdd(lst_t ll, void *e) 
{
  uint32_t n;
  int lvl = 0;
    
  n = lstSearch(ll,e);
  if (n != utl_lst_NULL) {
    lvl = utl_lstrand();
  }
  return 1;
}

#endif /* UTL_LIB */ 

#endif /* UTL_NOADT */



#ifndef UTL_NOADT

/** VECTORS **/

typedef struct vec_s {
  size_t  max;
  size_t  cnt;
  size_t  esz;
  void   *vec;
} *vec_t;

vec_t utl_vecNew(size_t esz);
#define vecNew(ty) utl_vecNew(sizeof(ty))

vec_t utl_vecFree(vec_t v);
#define vecFree utl_vecFree

int utl_vecSet(vec_t v, size_t i, void *e);
#define vecSet utl_vecSet
#define vecSet(v,i,ty,e) do { \
} while(utlZero)

int utl_vecAdd(vec_t v, void *e);
#define vecAdd  utl_vecAdd

void *utl_vecGet(vec_t v, size_t  i);
#define vecGet utl_vecGet

int utl_vecResize(vec_t v, size_t n);
#define vecResize utl_vecResize

size_t utl_vecCount(vec_t v);
#define vecCount     utl_vecCount

size_t utl_vecMax(vec_t v);
#define vecMax     utl_vecMax

void  *utl_vecVec(vec_t v);
#define vec(v,ty)   ((ty *)utl_vecVec(v))

#ifdef UTL_LIB

vec_t utl_vecNew(size_t esz)
{
  vec_t v;
  v = malloc(sizeof(struct vec_s));
  if (v) {
    v->max = 0;    v->cnt = 0;
    v->esz = esz;  v->vec = NULL;
  }
  return v;
}

vec_t utl_vecFree(vec_t v)
{
  if (v) {
    if (v->vec) free(v->vec);
    v->max = 0;  v->cnt = 0;
    v->esz = 0;  v->vec = NULL;
    free(v);
  }
  return NULL;
}

size_t utl_vecCount(vec_t v) { return v? v->cnt : 0; }
size_t utl_vecMax(vec_t v)   { return v? v->max : 0; }
void  *utl_vecVec(vec_t v)   { return v? v->vec : NULL; } 

static int utl_vec_expand(vec_t v, size_t i)
{
  unsigned long new_max;
  char *new_vec = NULL;
   
  if (!v) return 0;
   
  new_max = v->max;
  
  /* The minimum block should be able to contain a void * */
  if (new_max < sizeof(void *)) new_max = sizeof(void *);

  while (new_max <= i) new_max *= 2; /* double */
   
  if (new_max > v->max) {
    new_vec = realloc(v->vec,new_max * v->esz);
    if (!new_vec) return 0;
    v->vec = new_vec;
    v->max = new_max;
  }
  return 1;
}

int utl_vecSet(vec_t v, size_t  i, void *e)
{
  if (!utl_vec_expand(v,i)) return 0;
  
  memcpy(((char *)(v->vec)) + (i * v->esz),e,v->esz);
  if (i >= v->cnt) v->cnt = i+1;
  return 1;
}

void *utl_vecGet(vec_t v, size_t i)
{
  if (!v) return NULL;
  if (i >= v->cnt) return NULL;
  return ((char *)(v->vec)) + (i*v->esz);
}

void *utl_vecTop(vec_t v)
{
  void *ret = NULL;
  if (v && v->cnt) {
    ret =  ((char *)(v->vec)) + (v->cnt*v->esz);
  }
  return ret;
}

void *utl_vecPop(vec_t v)
{
  void *ret;
  if ((ret = utl_vecTop(v))) v->cnt -= 1;
  return ret;
}

int utl_vecAdd(vec_t v, void *e)
{
  if (!v) return 0;
  return utl_vecSet(v,v->cnt,e);
}

int utl_vecResize(vec_t v, size_t n)
{
  size_t new_max = 1;
  char *new_vec = NULL;
  if (!v) return 0;

  while (new_max <= n) new_max *= 2;
  
  if (new_max != v->max) {
    new_vec = realloc(v->vec,new_max * v->esz);
    if (!new_vec) return 0;
    v->vec = new_vec;
    v->max = new_max;
    if (v->cnt > v->max) v->cnt = v->max;
  }
  return 1;
}

#endif  /* UTL_LIB */


/** STACKS **/

#define stk_t          vec_t
#define stkNew(ty)     vecNew(ty)
#define stkCount(s)    vecCount(s)
#define stkEmpty(s)  (!vecCount(s))
#define stkPush(s,e)   vecAdd(s,e)
#define stkTop(s)      utl_vecTop(s)
#define stkPop(s)      utl_vecPop(s)

/**  BUFFERS **/
#define buf_t vec_t
int utl_bufSet(buf_t bf, size_t i, char c);

#define bufNew() utl_vecNew(1)
#define bufFree  utl_vecFree

char utl_bufGet(buf_t bf, size_t i);
#define bufGet   utl_bufGet

int utl_bufSet(buf_t bf, size_t i, char c);
#define bufSet   utl_bufSet

int utl_bufAdd(buf_t bf, char c);
#define bufAdd   utl_bufAdd

int utl_bufAddStr(buf_t bf, char *s);
#define bufAddStr  utl_bufAddStr

int utl_bufIns(buf_t bf, size_t i, char c);
#define bufIns(b,i,c) utl_bufIns(b,i,c)

int utl_bufInsStr(buf_t bf, size_t i, char *s);
#define bufInsStr(b,i,s) utl_bufInsStr(b,i,s)


#define bufResize utl_vecResize

#define bufClr(bf) utl_bufSet(bf,0,'\0');

int utl_bufFormat(buf_t bf, char *format, ...);
#define bufFormat utl_bufFormat

#define bufLen vecCount
#define bufMax vecMax
#define bufStr(b) vec(b,char)

int utl_bufAddLine(buf_t bf, FILE *f);
#define bufAddLine   utl_bufAddLine
#define bufReadLine  utl_bufAddLine

int utl_bufAddFile(buf_t bf, FILE *f);
#define bufAddFile   utl_bufAddFile
#define bufReadFile  utl_bufAddFile

#if !defined(UTL_HAS_SNPRINTF) && defined(_MSC_VER) && (_MSC_VER < 1800)
#define UTL_ADD_SNPRINTF
#define snprintf  c99_snprintf
#define vsnprintf c99_vsnprintf
#endif

#ifdef UTL_LIB
int utl_bufSet(buf_t bf, size_t i, char c)
{
  char *s;

  if (!utl_vec_expand(bf,i+1)) return 0;
  s = bf->vec;
  s[i] = c;
  if (c == '\0') {
    bf->cnt = i;
  }
  else if (i >= bf->cnt) {
    s[i+1] = '\0';
    bf->cnt = i+1;
  }
  return 1;
}

char utl_bufGet(buf_t bf, size_t i)
{
  if (!bf) return '\0';
  if (i >= bf->cnt) return '\0';
  return ((char*)(bf->vec))[i];
}

int utl_bufAdd(buf_t bf, char c)
{  return utl_bufSet(bf,bf->cnt,c); }

int utl_bufAddStr(buf_t bf, char *s)
{
  if (!bf) return 0;
  if (!s || !*s) return 1;
  
  while (*s) if (!utl_bufSet(bf,bf->cnt,*s++)) return 0;
  
  return utl_bufAdd(bf,'\0');
}

/* A line in the file can be ended by '\r\n', '\n' or '\r'.
** The NEWLINE characters are discarded.
** The string in the buffer is terminated with '\n\0'. 
*/
int utl_bufAddLine(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case '\r' : if ((c = fgetc(f)) != '\n') ungetc(c,f);
      case '\n' :
      case EOF  : c = EOF; utl_bufAdd(bf,'\n'); break;
      default   : utl_bufAdd(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

int utl_bufAddFile(buf_t bf, FILE *f)
{
  int c = 0;
  int n = 0;
  do {
    switch ((c = fgetc(f))) {
      case EOF  : c = EOF; break;
      default   : utl_bufAdd(bf,(char)c); n++; break;
    }
  } while (c != EOF);
  return n;
}

static int utl_buf_insert(buf_t bf, size_t i, size_t l, char *s)
{
  int n = 1;
  int k;
  char *b;
  
  if (!bf) return 0;
  if (!s)  return 1;
  if (l == 0) l = strlen(s);
  if (l == 0) return 1;
  if (i > bf->cnt) i = bf->cnt;

  if (!utl_vec_expand(bf,bf->cnt+l+1)) return 0;
  
  b = bf->vec+i;
  k = bf->cnt;
  memmove(b+l, b, k-i);
  memcpy(b,s,l);
  utl_bufSet(bf, k+l, '\0');
  return n;
}

int utl_bufInsStr(buf_t bf, size_t i, char *s)
{ return utl_buf_insert(bf,i,0,s); }

int utl_bufIns(buf_t bf, size_t i, char c)
{ return utl_buf_insert(bf,i,1,&c); }


/* {{ code from http://stackoverflow.com/questions/2915672 */
#ifdef UTL_ADD_SNPRINTF

inline int c99_snprintf(char* str, size_t size, const char* format, ...)
{
  int count;
  va_list ap;
  va_start(ap, format);
  count = c99_vsnprintf(str, size, format, ap);
  va_end(ap);
  return count;
}

inline int c99_vsnprintf(char* str, size_t size, const char* format, va_list ap)
{
  int count = -1;
  if (size != 0)   count = _vsnprintf_s(str, size, _TRUNCATE, format, ap);
  if (count == -1) count = _vscprintf(format, ap);
  return count;
}
#endif /* UTL_ADD_SNPRINTF */
/* }} */

int utl_bufFormat(buf_t bf, char *format, ...)
{
  int count;
  int count2;
  va_list ap;

  if (!bf) return -1;
  
  va_start(ap, format);
  count = vsnprintf(NULL,0,format, ap);
  va_end(ap);
  utl_bufSet(bf,count,'\0'); /* ensure there's enough room */
  va_start(ap, format);
  count2 = vsnprintf(bufStr(bf),count+1,format, ap);
  va_end(ap);
  
  utlAssert(count == count2);
  
  return count2;
}

#endif /* UTL_LIB */

typedef struct que_s {
  uint32_t front;
  vec_t vec;
} *que_t;

#define queAddFirst   utl_queAdd
#define queAddLast    utl_queAdd
#define queFirst      utl_queFirst
#define queLast       utl_queLast
#define queDropFirst  utl_queDropFirst
#define queDropLast   utl_queDropFirst

#define queNew utl_queNew
que_t utl_queNew(size_t esz);

#define queFree utl_queFree
que_t utl_queFree(que_t qu);

#define queEmpty utl_queEmpty
int utl_queEmpty(que_t qu); 

#ifdef UTL_LIB

que_t utl_queNew(size_t esz)
{
  que_t qu = NULL;
  qu = malloc(sizeof(struct que_s));
  if (qu) {
    qu->front = qu->back = 0;
    qu->vec = vecNew(esz);
    if (qu->vec == NULL) { free(qu); qu = NULL; }
  }
  return qu;
}

que_t utl_queFree(que_t qu)
{
  if (qu) {
    qu->vec = vecFree(qu->vec);
    free(qu);
  }
  return NULL;
}

int utl_queEmpty(que_t qu)
{
  return (qu == NULL || vecCount(qu->vec) == 0);
}

int utl_Add(que_t qu, void *e)
{
   int ret = 0;
   if (qu) {
     if (vecCount(qu->vec) == vecMax(qu->vec)) qu = utl_queexapand(qu);
     vecSet(qu->vec,qu->
   }
   return ret;
}

#endif /* UTL_LIB */

#endif /* UTL_NOADT */
/*** BAG ***/

#define bag_t vec_t

typedef struct bag_node_s { size_t left; size_t right; } bag_node_t;

#define bag_NULL  ((size_t)-1)

#define bagCount  vecCount
#define bagMax    vecMax
#define bagEsz(s) (vecEsz(s)-sizeof(set_node_t))
#ifdef UTL_LIB

bag_t utl_bagNew(size_t esz, utl_cmp_t cmp)
{
  bag_t bg;
  
  bg = vecNew(sizeof(bag_node_t)+esz);
  if (bg) {
    bg->first = bg->last = bag_NULL;
    bg->cmp = cmp;
    utl_vec_expand(bg,7);
  }
  return bg;
}

int utl_bag_expand(bag_t bg)
{
  size_t max = 0;

  max = bagMax(bg);
  if (bagCount(bg) < max) return 1; /* there's room enough */
  if (!utl_vec_expand(bg,(max * 2)-1)) return 0; /* couldn't expand!! */
  return 1;  
}

#define utl_bag_eptr(b,i) (utl_vec_getptr(v,i)+sizeof(bag_node_t))

int utl_bag_add(bag_t bg, void *e)
{
  size_t nd=0;
  bag_node_t *p = NULL;
  
  if (!utl_bag_expand(bg)) return 0;

  if (bagCount(bg) == 0) { bg->last = bag_NULL; }
  
  if (bg->last != bag_NULL) {
    nd = bg->last;
    bg->last = utl_vec_getptr(bg,nd)->left;
  }
  else {
    nd = bagCount(bg);
  }
  memcpy(utl_bag_eptr(bg,nd),e,bagEsz(bg));
  
  if (bagCount(bg) == 0) {
    bg->first = 0;
    bg->cnt++;
  }
  else utl_bag_insert(bg, nd)
    
  return 1;   
}

#endif /* UTL_LIB */ 