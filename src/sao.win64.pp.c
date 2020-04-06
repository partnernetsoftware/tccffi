enum { libc_fprintf, libc_malloc, libc_memset, libc_strdup, libc_strcmp, libc_printf, libc_putc, libc_getc, libc_isalnum, libc_strchr, libc_isdigit, libc_isalpha, libc_fopen, libc_fread, libc_fgets, libc_fclose, libc_feof, libc_usleep, libc_msleep, libc_sleep, libc_fputc, libc_strlen, libc_fflush, libc_free, libc_system, libc_setmode, libc_fileno, libc_stdin, libc_stdout, libc_stderr, libc_microtime, libc_exit, };
typedef signed char sao_i8;
typedef unsigned char sao_u8;
typedef signed short int sao_i16;
typedef unsigned short int sao_u16;
typedef signed int sao_i32;
typedef unsigned int sao_u32;
typedef signed long int sao_i64;
typedef unsigned long int sao_u64;
typedef struct _iobuf {
 char *_ptr;
 int _cnt;
 char *_base;
 int _flag;
 int _file;
 int _charbuf;
 int _bufsiz;
 char *_tmpfname;
} FILE;
__attribute__((dllimport)) FILE * __iob_func(void);
typedef void* ffic_ptr;
typedef ffic_ptr(*ffic_func)();
extern int fprintf(FILE *stream, const char *format, ...);
extern int fflush(FILE *stream);
extern int strcmp(const char*,const char*);
extern ffic_ptr LoadLibraryA(const char*);
extern ffic_ptr GetProcAddress(ffic_ptr,const char*);
void ffic_strcat(char *buffer, const char *source, const char* append) {
 while (*source) *(buffer++) = *(source++);
 while (*append) *(buffer++) = *(append++);
 *buffer = '\0';
}
ffic_ptr ffic_void(){return 0;};
ffic_ptr(*ffic_raw(const char* part1, const char* funcname, const char* part2))()
{
 char libfilename[512] = {0};
 ffic_strcat(libfilename,part1,
   (part2==0)?
   ".dylib"
   :part2
   );
 return GetProcAddress(LoadLibraryA(libfilename), funcname);
}
ffic_ptr ffic_usleep(int nano_seconds)
{
 ffic_raw("kernel32","Sleep",0)(nano_seconds/1000);
 return 0;
};
ffic_ptr ffic_msleep(int microseconds)
{
 ffic_raw("kernel32","Sleep",0)(microseconds);
 return 0;
};
ffic_ptr ffic_sleep(int seconds)
{
 ffic_raw("kernel32","Sleep",0)(seconds*1000);
 return 0;
}
sao_u64 ffic_microtime(void);
ffic_ptr(*ffic(const char* libname, const char* funcname, ...))()
{
 ffic_ptr addr = 0;
 if(!strcmp("c",libname)){
  if(!strcmp("stderr",funcname)){ addr = (&__iob_func()[2]); }
  else if(!strcmp("stdout",funcname)){ addr = (&__iob_func()[1]); }
  else if(!strcmp("stdin",funcname)){ addr = (&__iob_func()[0]); }
  else{
   libname =
    "libc"
    ;
   if(!strcmp("microtime",funcname)){ return (ffic_ptr) ffic_microtime; }
   else if(!strcmp("usleep",funcname)){ return ffic_usleep; }
   else if(!strcmp("sleep",funcname)){ return ffic_sleep; }
   else if(!strcmp("msleep",funcname)){ return ffic_msleep; }
   else if(!strcmp("fileno",funcname)){ funcname = "_fileno"; }
   else if(!strcmp("setmode",funcname)){
    funcname = "_setmode";
   }
   else if(!strcmp("strdup",funcname)){ funcname = "_strdup"; }
  }
 }
 if(addr==0) addr = ffic_raw(libname,funcname,0);
 if(0==addr){
  fprintf((&__iob_func()[2]),"ERR: Not found %s.%s\n", libname, funcname);fflush((&__iob_func()[2]));
  return ffic_void;
 }
 return addr;
}
struct timeval {
 long tv_sec;
 long tv_usec;
};
sao_u64 ffic_microtime(void)
{
 struct timeval tv;
 static ffic_func gettimeofday;
 gettimeofday = ffic_raw("kernel32","GetSystemTimePreciseAsFileTime",0);
 if (!gettimeofday) gettimeofday = ffic_raw("kernel32","GetSystemTimeAsFileTime",0);
 static const sao_u64 epoch = 116444736000000000;
 struct _FILETIME {
  unsigned long dwLowDateTime;
  unsigned long dwHighDateTime;
 } file_time;
 gettimeofday(&file_time);
 sao_u64 since_1601 = ( (sao_u64) file_time.dwHighDateTime << 32) | (sao_u64) file_time.dwLowDateTime;
 sao_u64 since_1970 = ((sao_u64) since_1601 - epoch);
 sao_u64 microseconds_since_1970 = since_1970 / 10;
 tv.tv_sec = (microseconds_since_1970 / (sao_u64) 1000000);
 tv.tv_usec = microseconds_since_1970 % (sao_u64) 1000000;
 return ((sao_u64)tv.tv_sec*(sao_u64)1000 + (((sao_u64)tv.tv_usec)/(sao_u64)1000)%(sao_u64)1000);
}
ffic_func libc_a[libc_exit+1];
ffic_func libc_(int fi,const char* fn){ return libc_a[fi]?libc_a[fi]:(libc_a[fi]=ffic("c",fn)); }
void* sao_calloc(long _sizeof){return libc_(libc_memset,"memset")(libc_(libc_malloc,"malloc")(_sizeof),0,_sizeof);}
typedef enum { stream_file, stream_char, } stream_t; char* stream_names[] = { "file", "char", };;
typedef enum { type_list, type_integer, type_double, type_symbol, type_string, type_native, type_vector, type_table, } type_t; char* type_names[] = { "list", "integer", "double", "symbol", "string", "native", "vector", "table", };;
typedef enum { ctype_long, ctype_double, ctype_any, } ctype_t; char* ctype_names[] = { "long", "double", "any", };;
typedef enum { argt_i, argt_p, argt_d, argt_v, argt_e, argt_s, argt_l, argt_h, } argt_t; char* argt_names[] = { "i", "p", "d", "v", "e", "s", "l", "h", };;
int argta[argt_h+1];
typedef struct _sao_obj sao_obj,*p_sao_obj;
typedef p_sao_obj (*native_t)(p_sao_obj );
struct _sao_obj {
 union {
  struct {
   union {
    struct {
     p_sao_obj car;
     p_sao_obj cdr;
     p_sao_obj upr;
    };
    struct {
      p_sao_obj* _vector;
     long _len;
    };
    struct {
      p_sao_obj* _table;
     long _size;
    };
    char *_string;
    long _integer;
    double _double;
    native_t _native;
   };
   type_t _type;
  };
 };
};
p_sao_obj SAO_TAG_nil=((void*)0); p_sao_obj SAO_TAG_argv=((void*)0); p_sao_obj SAO_TAG_global=((void*)0);;
p_sao_obj SAO_TAG_true=((void*)0); p_sao_obj SAO_TAG_false=((void*)0); p_sao_obj SAO_TAG_quote=((void*)0); p_sao_obj SAO_TAG_set=((void*)0); p_sao_obj SAO_TAG_let=((void*)0); p_sao_obj SAO_TAG_var=((void*)0); p_sao_obj SAO_TAG_procedure=((void*)0); p_sao_obj SAO_TAG_if=((void*)0); p_sao_obj SAO_TAG_lambda=((void*)0); p_sao_obj SAO_TAG_begin=((void*)0); p_sao_obj SAO_TAG_or=((void*)0); p_sao_obj SAO_TAG_ok=((void*)0); p_sao_obj SAO_TAG_else=((void*)0); p_sao_obj SAO_TAG_cond=((void*)0); p_sao_obj SAO_TAG_error=((void*)0);;
typedef struct _FileChar {
 int c;
 struct _FileChar * ptr_prev;
 struct _FileChar * ptr_next;
} FileChar;
typedef struct {
 stream_t _type;
 void* fp;
 char* pos;
 FileChar * ptr_start;
 FileChar * ptr_head;
 FileChar * ptr_last;
} sao_stream;
p_sao_obj sao_eval(p_sao_obj exp, p_sao_obj ctx);
p_sao_obj sao_load_expr(sao_stream * fw);
p_sao_obj sao_is_atom(p_sao_obj x){ return (x&&x->_type)?x:SAO_TAG_nil; }
long sao_is_digit(int c) { return (long) libc_(libc_isdigit,"isdigit")(c); }
long sao_is_alpha(int c) { return (long) libc_(libc_isalpha,"isalpha")(c); }
long sao_is_alphanumber(int c) { return (long) libc_(libc_isalnum,"isalnum")(c); }
struct htable { p_sao_obj key; };
static struct htable *gHTable = 0;
static int gHTable_len = 0;
static long ht_hash(const char *s, int ht_len) {
 long h = 0;
 char *u = (char *) s;
 while (*u) { h = (h * 256 + (*u)) % ht_len; u++; }
 return h;
}
int ht_resize(int newsize){
 struct htable * newTable = sao_calloc( sizeof(struct htable) *(newsize) );
 for(int i=0;i<gHTable_len;i++){
  if (((void*)0)!=gHTable[i].key) {
   int h = ht_hash(gHTable[i].key->_string, newsize);
   if(((void*)0) != newTable[h].key){
    libc_(libc_printf,"printf")("DEBUG: newTable(%d) still full ??\n", newsize);
   }
   newTable[h].key = gHTable[i].key;
  }
 }
 gHTable = newTable;
 gHTable_len = newsize;
 return newsize;
}
void ht_insert(p_sao_obj key_obj)
{
 long h = ht_hash(key_obj->_string, gHTable_len);
 if(((void*)0) != gHTable[h].key && ((void*)0)!=gHTable[h].key->_string){
  int newsize = 2*(gHTable_len+1)-1 ;
  ht_resize( newsize );
  ht_insert( key_obj );
  return;
 }
 gHTable[h].key = key_obj;
}
p_sao_obj ht_lookup(char *s) {
 long h = ht_hash(s, gHTable_len);
 return gHTable[h].key;
}
p_sao_obj sao_alloc(type_t type) {
 sao_obj*ret=sao_calloc( sizeof(sao_obj) );;
 if(ret<0) do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"ASSERT: mem full when sao_alloc(%d)",ret);libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
 ret->_type = type;
 return ret;
}
p_sao_obj cons(p_sao_obj car, p_sao_obj cdr) { p_sao_obj ret = sao_alloc(type_list); ret->car = car; ret->cdr = cdr; return ret; }
p_sao_obj car(p_sao_obj x) { return (!x || x->_type)? SAO_TAG_nil: x->car; }
p_sao_obj cdr(p_sao_obj x) { return (!x || x->_type)? SAO_TAG_nil: x->cdr; }
p_sao_obj caar(p_sao_obj x) { return (!x || x->_type || !x->car || x->car->_type)? SAO_TAG_nil: x->car->car; }
p_sao_obj cdar(p_sao_obj x) { return (!x || x->_type || !x->car || x->car->_type)? SAO_TAG_nil: x->car->cdr; }
p_sao_obj cadr(p_sao_obj x) { return (!x || x->_type || !x->cdr || x->cdr->_type)? SAO_TAG_nil: x->cdr->car; }
p_sao_obj cddr(p_sao_obj x) { return (!x || x->_type || !x->cdr || x->cdr->_type)? SAO_TAG_nil: x->cdr->cdr; }
p_sao_obj cadar(p_sao_obj x) {
 if(!x) return SAO_TAG_nil;
 if(x->_type) return SAO_TAG_nil;
 if(!x->car)return SAO_TAG_nil;
 if(x->car->_type)return SAO_TAG_nil;
 if(!x->car->cdr)return SAO_TAG_nil;
 if(x->car->cdr->_type)return SAO_TAG_nil;
 return x->car->cdr->car;
}
p_sao_obj caddr(p_sao_obj x) {
 if(!x)return SAO_TAG_nil;
 if(x->_type) return SAO_TAG_nil;
 if(!x->cdr)return SAO_TAG_nil;
 if(x->cdr->_type)return SAO_TAG_nil;
 if(!x->cdr->cdr)return SAO_TAG_nil;
 if(x->cdr->cdr->_type)return SAO_TAG_nil;
 return x->cdr->cdr->car;
}
p_sao_obj cdddr(p_sao_obj x) {
 if(!x)return SAO_TAG_nil;
 if(x->_type) return SAO_TAG_nil;
 if(!x->cdr)return SAO_TAG_nil;
 if(x->cdr->_type)return SAO_TAG_nil;
 if(!x->cdr->cdr)return SAO_TAG_nil;
 if(x->cdr->cdr->_type)return SAO_TAG_nil;
 return x->cdr->cdr->cdr;
}
p_sao_obj cdadr(p_sao_obj x) {
 if(!x)return SAO_TAG_nil;
 if(x->_type) return SAO_TAG_nil;
 if(!x->cdr)return SAO_TAG_nil;
 if(x->cdr->_type)return SAO_TAG_nil;
 if(!x->cdr->car)return SAO_TAG_nil;
 if(x->cdr->car->_type)return SAO_TAG_nil;
 return x->cdr->car->cdr;
}
p_sao_obj cadddr(p_sao_obj x) {
 if(!x)return SAO_TAG_nil;
 if(x->_type) return SAO_TAG_nil;
 if(!x->cdr)return SAO_TAG_nil;
 if(x->cdr->_type)return SAO_TAG_nil;
 if(!x->cdr->cdr)return SAO_TAG_nil;
 if(x->cdr->cdr->_type)return SAO_TAG_nil;
 if(!x->cdr->cdr->cdr)return SAO_TAG_nil;
 if(x->cdr->cdr->cdr->_type)return SAO_TAG_nil;
 return x->cdr->cdr->cdr->car;
}
p_sao_obj sao_new_vector(int size) {
 p_sao_obj ret = sao_alloc(type_vector);
 ret->_vector = sao_calloc( sizeof(sao_obj) *(size) );
 ret->_len = size;
 return ret;
}
p_sao_obj sao_new_symbol(char *s) {
 p_sao_obj ret = ht_lookup(s);
 if (!(ret)) {
  ret = sao_alloc(type_symbol);
  ret->_string = libc_(libc_strdup,"strdup")(s);
  ht_insert(ret);
 }else{
  if(!libc_(libc_strcmp,"strcmp")(ret->_string,s)){
  }else{
   int newsize = 2*(gHTable_len+1)-1 ;
   ht_resize( newsize );
   return sao_new_symbol(s);
  }
 }
 return ret;
}
p_sao_obj sao_new_native(native_t x) {
 p_sao_obj ret = sao_alloc(type_native);
 ret->_native = x;
 return ret;
}
p_sao_obj sao_new_lambda(p_sao_obj params, p_sao_obj body) {
 return cons(SAO_TAG_lambda, cons(params, body));
}
p_sao_obj sao_new_procedure(p_sao_obj params, p_sao_obj body,
  p_sao_obj ctx) {
 return cons(SAO_TAG_procedure, cons(params, cons(body, cons(ctx, SAO_TAG_nil))));
}
p_sao_obj sao_reverse(p_sao_obj list, p_sao_obj first) {
 p_sao_obj rt = (!(list)) ? first :
  sao_reverse(cdr(list), cons(car(list), first));
 return rt;
}
p_sao_obj sao_is_eq(p_sao_obj x, p_sao_obj y) {
 do{
  if (x == y) return x;
  if (!x || !y) break;
  if (x->_type != y->_type) break;
  switch (x->_type) {
   case type_integer: if(x->_integer == y->_integer) return x;
   case type_symbol:
   case type_string: if(!libc_(libc_strcmp,"strcmp")(x->_string, y->_string)) return x;
   default: break;
  }
 }while(0);
 return SAO_TAG_nil;
}
p_sao_obj sao_not_false(p_sao_obj x) {
 if (!(x) || sao_is_eq(x, SAO_TAG_false)) return SAO_TAG_nil;
 if (x->_type == type_integer && x->_integer == 0) return SAO_TAG_nil;
 return x;
}
p_sao_obj sao_is_tagged(p_sao_obj cell, p_sao_obj tag) { return (cell&&!cell->_type) ? sao_is_eq(car(cell),tag) : SAO_TAG_nil; }
int sao_list_len(p_sao_obj expr) { return (expr) ? (1+sao_list_len(cdr(expr))):0; }
int sao_deq_c(sao_stream *fw)
{
 int c = -2;
 FileChar * ptr_head = fw->ptr_head;
 if(ptr_head!=((void*)0)){
  c = ptr_head->c;
  fw->ptr_head=ptr_head->ptr_next;
 }
 return c;
}
int sao_enq_c(sao_stream* fw,int k){
 FileChar*fc=sao_calloc( sizeof(FileChar) );;
 fc->c = k;
 fc->ptr_prev= fw->ptr_last;
 if(((void*)0)==fw->ptr_start){
  fw->ptr_start = fc;
 }
 if(((void*)0)==fw->ptr_head){
  fw->ptr_head = fc;
 }
 if(((void*)0)!=fw->ptr_last){
  fw->ptr_last->ptr_next = fc;
 }
 fw->ptr_last = fc;
 return k;
}
int sao_read_line(sao_stream* fw)
{
 int line_num = 0;
 ffic_func feof = libc_(libc_feof,"feof");
 do{
  if(fw->_type==stream_file){
   if(!fw->fp) do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"FILE NOT FOUND?");libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
   if(feof(fw->fp)){ break; }
  }else{
   if (fw->pos==0) libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"DEBUG no pos?");
   if (*(fw->pos)==0){
    libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"DEBUG end?");
    break;
   }
  }
  ffic_func fgets = libc_(libc_fgets,"fgets");
  ffic_func strlen = libc_(libc_strlen,"strlen");
  int LINE_LEN = 1024;
  char*line=sao_calloc( sizeof(char) *(LINE_LEN) );;
  if(fw->_type==stream_file){
   if(argta[argt_i]){
    libc_(libc_printf,"printf")("> ");
   }
   fgets(line,LINE_LEN,fw->fp);
   long strlen_line = (long) strlen(line);
   if(strlen_line>0){
    for(int i=0;i<strlen_line;i++)
    {
     if('\n'==sao_enq_c(fw,line[i])){ line_num++; }
    }
   }else{
    sao_enq_c(fw,(-1));
   }
  }else{
   while( *(fw->pos)!=0 ){
    if('\n'==sao_enq_c(fw,(*(fw->pos)))){ line_num++; }
    fw->pos++;
   }
   sao_enq_c(fw,(-1));
  }
  libc_(libc_free,"free")(line);
 }while(0);
 return line_num;
}
p_sao_obj sao_expand(p_sao_obj var, p_sao_obj val, p_sao_obj ctx) {
 return cons(cons(var, val), ctx);
}
p_sao_obj sao_get_var(p_sao_obj var, p_sao_obj ctx) {
 while ((ctx)) {
  p_sao_obj frame = car(ctx);
  p_sao_obj vars = car(frame);
  p_sao_obj vals = cdr(frame);
  while ((vars)) {
   if (sao_is_eq(car(vars), var))
    return car(vals);
   vars = cdr(vars);
   vals = cdr(vals);
  }
  ctx = cdr(ctx);
 }
 return SAO_TAG_nil;
}
p_sao_obj sao_set_var(p_sao_obj var, p_sao_obj val, p_sao_obj ctx) {
 while ((ctx)) {
  p_sao_obj frame = car(ctx);
  p_sao_obj vars = car(frame);
  p_sao_obj vals = cdr(frame);
  while ((vars)) {
   if (sao_is_eq(car(vars), var)) {
    vals->car = val;
    return car(vals);
   }
   vars = cdr(vars);
   vals = cdr(vals);
  }
  ctx = cdr(ctx);
 }
 return val;
}
p_sao_obj sao_def_var(p_sao_obj var, p_sao_obj val, p_sao_obj ctx)
{
 if(!ctx) do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"ASSERT: sao_def_var need ctx");libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
 p_sao_obj frame = car(ctx);
 if(!frame) do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"ASSERT: sao_def_var(): found no car in ctx");libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
 p_sao_obj vars = car(frame);
 p_sao_obj vals = cdr(frame);
 while ((vars)) {
  if (sao_is_eq(var, car(vars))) {
   vals->car = val;
   return val;
  }
  vars = cdr(vars);
  vals = cdr(vals);
 }
 frame->car = cons(var, car(frame));
 frame->cdr = cons(val, cdr(frame));
 return val;
}
char type_symbolS[] = "~!@#$%^&*_-+\\:.<>|{}[]?=/";
p_sao_obj sao_eval_list(p_sao_obj exp, p_sao_obj ctx) {
 if (!(exp)) return SAO_TAG_nil;
 return cons(sao_eval(car(exp), ctx), sao_eval_list(cdr(exp), ctx));
}
sao_stream * sao_stream_new(void* fp,stream_t type)
{
 sao_stream*fw=sao_calloc( sizeof(sao_stream) );;
 fw->fp = fp;
 if(type==stream_char) fw->pos = fp;
 fw->_type = type;
 return fw;
}
int sao_peek(sao_stream * fw)
{
 int c = 0;
 FileChar * ptr_head = fw->ptr_head;
 if(ptr_head!=0){
  c = ptr_head->c;
 }
 return c;
}
p_sao_obj sao_read_symbol(sao_stream * fw, char start)
{
 char buf[128];
 buf[0] = start;
 int i = 1;
 while (sao_is_alphanumber(sao_peek(fw))
   || libc_(libc_strchr,"strchr")(type_symbolS, sao_peek(fw)))
 {
  if (i >= 128)
   do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"Symbol name too long - maximum length 128 characters");libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
  buf[i++] = sao_deq_c(fw);
 }
 buf[i] = '\0';
 return sao_new_symbol(buf);
}
p_sao_obj sao_new_integer(int x)
{
 p_sao_obj ret = sao_alloc(type_integer);
 ret->_integer = x;
 return ret;
}
int sao_read_int(sao_stream * fw, int start)
{
 while ( sao_is_digit(sao_peek(fw)) )
  start = start * 10 + (sao_deq_c(fw) - '0');
 return start;
}
p_sao_obj sao_read_list(sao_stream * fw)
{
 p_sao_obj obj;
 p_sao_obj cell = SAO_TAG_nil;
 for (;;) {
  obj = sao_load_expr(fw);
  if (!(obj))
   return sao_reverse(cell, SAO_TAG_nil);
  cell = cons(obj, cell);
 }
 return SAO_TAG_nil;
}
p_sao_obj sao_load_str(sao_stream * fw)
{
 char buf[256];
 int i = 0;
 int c;
 while ((c = sao_deq_c(fw)) != '\"') {
  if (c == (-1)) return SAO_TAG_nil;
  if (i >= 256) do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"String too long - maximum length 256 characters");libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
  buf[i++] = (char) c;
 }
 buf[i] = '\0';
 p_sao_obj s = sao_new_symbol(buf);
 s->_type = type_string;
 return s;
}
void sao_comment(sao_stream * fw) { int c; for (;;) { c = sao_deq_c(fw); if (c == '\n' || c == (-1)) return; } }
p_sao_obj sao_load_expr(sao_stream * fw)
{
 int c;
 for (;;) {
  p_sao_obj theSymbol = SAO_TAG_nil;
  c = sao_deq_c(fw);
  switch(c){
   case (-1): return ((void*)0);
   case -2: sao_read_line(fw);continue;
   case '\n':
   case '\r':
   case ' ':
   case '\t':
   case 0:
   case ',': continue;
   case '\"': return sao_load_str(fw);
  }
  if (c == ';' || c=='#' || (c=='/'&&'/'==sao_peek(fw))){ sao_comment(fw); continue; }
  if (c == '\''){
   p_sao_obj child = sao_load_expr(fw);
   return cons(SAO_TAG_quote, cons(child, SAO_TAG_nil));
  }
  if (libc_(libc_isalpha,"isalpha")(c) || libc_(libc_strchr,"strchr")(type_symbolS, c)){
   theSymbol = sao_read_symbol(fw,c);
   if(argta[argt_l]){ return theSymbol; }
   else{
    while(' '==sao_peek(fw)) c = sao_deq_c(fw);
    if('('==sao_peek(fw)){
     c = sao_deq_c(fw);
    }else{
     return theSymbol;
    }
   }
  }
  if (c == '(') {
   p_sao_obj list = sao_read_list(fw);
   if(argta[argt_l]){ return list; }
   list = cons(theSymbol,list);
   return list;
  }
  if (c == ')') { return SAO_TAG_nil; }
  if (sao_is_digit(c)) return sao_new_integer(sao_read_int(fw, c - '0'));
  if (c == '-' && sao_is_digit(sao_peek(fw)))
   return sao_new_integer(-1 * sao_read_int(fw, sao_deq_c(fw) - '0'));
 }
 return SAO_TAG_nil;
}
void sao_out_expr(char *str, p_sao_obj el){
 if (str) libc_(libc_printf,"printf")("%s ", str);
 if (!(el)) { libc_(libc_printf,"printf")("'()"); return; }
 if (!(el)) { return; }
 switch (el->_type) {
  case type_string:
   libc_(libc_printf,"printf")("\"%s\"", el->_string); break;
  case type_symbol:
   libc_(libc_printf,"printf")("%s", el->_string); break;
  case type_integer:
   libc_(libc_printf,"printf")("%ld", el->_integer); break;
  case type_double:
   libc_(libc_printf,"printf")("%f", el->_double); break;
  case type_native:
   libc_(libc_printf,"printf")("<function>"); break;
  case type_vector:
   libc_(libc_printf,"printf")("<vector %d>", el->_len); break;
  case type_table:
   libc_(libc_printf,"printf")("<table %d>", el->_size); break;
  case type_list:
   if (sao_is_tagged(el, SAO_TAG_procedure)) {
    libc_(libc_printf,"printf")("<closure>");
    return;
   }
   int skip=0;
   p_sao_obj *t = &el;
   if(!argta[argt_l]){
    if ((*t)) {
     if((*t)->car && type_symbol == (*t)->car->_type){
      sao_out_expr(0, (*t)->car);
      skip=1;
     }
    }
   }
   libc_(libc_printf,"printf")("(");
   while ((*t)) {
    if(!argta[argt_l]){
     if(skip==1){
      skip=0;
     }else{
      libc_(libc_printf,"printf")(" ");
      sao_out_expr(0, (*t)->car);
     }
    }else{
     libc_(libc_printf,"printf")(" ");
     sao_out_expr(0, (*t)->car);
    }
    if (((*t)->cdr)) {
     if ((*t)->cdr->_type == type_list) {
      t = &(*t)->cdr;
     } else {
      sao_out_expr(".", (*t)->cdr);
      break;
     }
    } else
     break;
   }
   libc_(libc_printf,"printf")(")");
 }
}
p_sao_obj sao_eval(p_sao_obj exp, p_sao_obj ctx)
{
tail:
 if (!(exp))
 {
  return SAO_TAG_nil;
 } else if (exp->_type == type_integer || exp->_type == type_string) {
  return exp;
 } else if (exp->_type == type_symbol) {
  p_sao_obj s = sao_get_var(exp, ctx);
  if (!s) {
   do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"ERROR: symbol(%s) not found.\n",exp->_string);libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
  }
  return s;
 } else if (sao_is_tagged(exp, SAO_TAG_quote)) {
  return cadr(exp);
 } else if (sao_is_tagged(exp, SAO_TAG_lambda)) {
  return sao_new_procedure(cadr(exp), cddr(exp), ctx);
 } else if (sao_is_tagged(exp, SAO_TAG_var)) {
  if (sao_is_atom(cadr(exp)))
   sao_def_var(cadr(exp), sao_eval(caddr(exp), ctx), ctx);
  else {
   p_sao_obj closure =
    sao_eval(sao_new_lambda(cdr(cadr(exp)), cddr(exp)), ctx);
   sao_def_var(car(cadr(exp)), closure, ctx);
  }
  return SAO_TAG_ok;
 } else if (sao_is_tagged(exp, SAO_TAG_begin)) {
  p_sao_obj args = cdr(exp);
  for (; (cdr(args)); args = cdr(args))
   sao_eval(car(args), ctx);
  exp = car(args);
  goto tail;
 } else if (sao_is_tagged(exp, SAO_TAG_if)) {
  p_sao_obj predicate = sao_eval(cadr(exp), ctx);
  exp = (sao_not_false(predicate)) ? caddr(exp) : cadddr(exp);
  goto tail;
 } else if (sao_is_tagged(exp, SAO_TAG_or)) {
  p_sao_obj predicate = sao_eval(cadr(exp), ctx);
  exp = (sao_not_false(predicate)) ? caddr(exp) : cadddr(exp);
  goto tail;
 } else if (sao_is_tagged(exp, SAO_TAG_cond)) {
  p_sao_obj branch = cdr(exp);
  for (; (branch); branch = cdr(branch)) {
   if (sao_is_tagged(car(branch), SAO_TAG_else) ||
     sao_not_false(sao_eval(caar(branch), ctx))) {
    exp = cons(SAO_TAG_begin, cdar(branch));
    goto tail;
   }
  }
  return SAO_TAG_nil;
 } else if (sao_is_tagged(exp, SAO_TAG_set)) {
  if (sao_is_atom(cadr(exp))){
   sao_set_var(cadr(exp), sao_eval(caddr(exp), ctx), ctx);
  } else {
   p_sao_obj closure =
    sao_eval(sao_new_lambda(cdr(cadr(exp)), cddr(exp)), ctx);
   sao_set_var(car(cadr(exp)), closure, ctx);
  }
  return SAO_TAG_ok;
 } else if (sao_is_tagged(exp, SAO_TAG_let)) {
  p_sao_obj pointer;
  p_sao_obj vars = SAO_TAG_nil;
  p_sao_obj vals = SAO_TAG_nil;
  if (!(cadr(exp))) return SAO_TAG_nil;
  if (sao_is_atom(cadr(exp))) {
   for (pointer = caddr(exp); (pointer); pointer = cdr(pointer))
   {
    vars = cons(caar(pointer), vars);
    vals = cons(cadar(pointer), vals);
   }
   sao_def_var(cadr(exp),
     sao_eval(sao_new_lambda(vars, cdr(cddr(exp))),
      sao_expand(vars, vals, ctx)),
     ctx);
   exp = cons(cadr(exp), vals);
   goto tail;
  }
  for (pointer = cadr(exp); (pointer); pointer = cdr(pointer)) {
   vars = cons(caar(pointer), vars);
   vals = cons(cadar(pointer), vals);
  }
  exp = cons(sao_new_lambda(vars, cddr(exp)), vals);
  goto tail;
 } else {
  p_sao_obj proc = sao_eval(car(exp), ctx);
  p_sao_obj args = sao_eval_list(cdr(exp), ctx);
  if (!(proc)) {
   if(argta[argt_s]){
    sao_out_expr("WARNING: Invalid arguments to sao_eval:", exp);
    libc_(libc_printf,"printf")("\n");
   }
   return SAO_TAG_nil;
  }
  if (proc->_type == type_native){
   return proc->_native(args);
  }
  if (sao_is_tagged(proc, SAO_TAG_procedure)) {
   ctx = sao_expand(cadr(proc), args, cadddr(proc));
   exp = cons(SAO_TAG_begin, caddr(proc));
   goto tail;
  }
  libc_(libc_printf,"printf")("DEBUG 800 native[%d,%d]\n",proc->_type,proc->_native);
 }
 sao_out_expr("Invalid arguments to sao_eval:", exp);
 libc_(libc_printf,"printf")("\n");
 return SAO_TAG_nil;
}
p_sao_obj sao_parse( sao_stream * fw, int do_eval ) {
 sao_read_line(fw);
 sao_u64 (*microtime)() = ( sao_u64(*)() ) libc_(libc_microtime,"microtime");
 p_sao_obj rt = SAO_TAG_nil;
 for(;;){
  p_sao_obj exp = sao_load_expr(fw);
  if(exp==((void*)0)){
   break;
  }
  if ((exp)) {
   if(argta[argt_d]) libc_(libc_printf,"printf")("%llu: ",microtime());
   if(argta[argt_i]||argta[argt_d]){
    sao_out_expr("<=", exp);
    libc_(libc_printf,"printf")("\n");
   }
   if (do_eval){
    rt = sao_eval(exp, SAO_TAG_global);
    if(argta[argt_d]) libc_(libc_printf,"printf")("%llu: ",microtime());
    if(argta[argt_i]||argta[argt_d]){
     if ( (rt)) {
      sao_out_expr("=>", rt);
      libc_(libc_printf,"printf")("\n");
     }
    }
   }else{
    rt = exp;
   }
  }
 }
 return rt;
}
p_sao_obj sao_type_check(const char *func, p_sao_obj obj, type_t type)
{
 if (!(obj)) {
  libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"Invalid argument to function %s: SAO_TAG_nil\n", func);
  libc_(libc_exit,"exit")(1);
 } else if (obj->_type != type) {
  libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"ERR: function %s. expected %s got %s\n", func, type_names[type], type_names[obj->_type]);
  libc_(libc_exit,"exit")(1);
 }
 return obj;
}
p_sao_obj native_type(p_sao_obj args) { return sao_new_symbol(type_names[car(args)->_type]); }
p_sao_obj native_global(p_sao_obj args) { return SAO_TAG_global; }
p_sao_obj native_list(p_sao_obj args) { return (args); }
p_sao_obj native_cons(p_sao_obj args) { return cons(car(args), cadr(args)); }
p_sao_obj native_car(p_sao_obj args) { if(argta[argt_s]) (sao_type_check(__func__, car(args), type_list)); return caar(args); }
p_sao_obj native_cdr(p_sao_obj args) { if(argta[argt_s]) (sao_type_check(__func__, car(args), type_list)); return cdar(args); }
p_sao_obj native_setcar(p_sao_obj args) { (sao_type_check(__func__, car(args), type_list)); (args->car->car = (cadr(args))); return SAO_TAG_nil; }
p_sao_obj native_setcdr(p_sao_obj args) { (sao_type_check(__func__, car(args), type_list)); (args->car->cdr = (cadr(args))); return SAO_TAG_nil; }
p_sao_obj native_is_null(p_sao_obj args) { return !(car(args)) ? SAO_TAG_true : SAO_TAG_false; }
p_sao_obj native_pairq(p_sao_obj args) {
 if (car(args)->_type != type_list) return SAO_TAG_false;
 return (sao_is_atom(caar(args)) && sao_is_atom(cdar(args))) ? SAO_TAG_true : SAO_TAG_false;
}
p_sao_obj native_is_list(p_sao_obj args) {
 p_sao_obj list;
 if (car(args)->_type != type_list)
  return SAO_TAG_false;
 for (list = car(args); (list); list = list->cdr)
  if ((list->cdr) && (list->cdr->_type != type_list))
   return SAO_TAG_false;
 return (car(args)->_type == type_list && native_pairq(args) != SAO_TAG_true) ? SAO_TAG_true : SAO_TAG_false;
}
p_sao_obj native_atomq(p_sao_obj sexp) { return sao_is_atom(car(sexp)) ? SAO_TAG_true : SAO_TAG_false; }
p_sao_obj native_cmp(p_sao_obj args) {
 if ((car(args)->_type != type_integer) || (cadr(args)->_type != type_integer))
  return SAO_TAG_false;
 return (car(args)->_integer == cadr(args)->_integer) ? SAO_TAG_true : SAO_TAG_false;
}
p_sao_obj native_eqq(p_sao_obj args) { return sao_is_eq(car(args), cadr(args)) ? SAO_TAG_true : SAO_TAG_false; }
p_sao_obj native_equalq(p_sao_obj args) {
 if (sao_is_eq(car(args), cadr(args)))
  return SAO_TAG_true;
 if ((car(args)->_type == type_list) && (cadr(args)->_type == type_list)) {
  p_sao_obj a, b;
  a = car(args);
  b = cadr(args);
  while ((a) && (b)) {
   if (!sao_is_eq(car(a), car(b)))
    return SAO_TAG_false;
   a = cdr(a);
   b = cdr(b);
  }
  return SAO_TAG_true;
 }
 if ((car(args)->_type == type_vector) && (cadr(args)->_type == type_vector)) {
  if (car(args)->_len != cadr(args)->_len) {
   return SAO_TAG_false;
  }
  p_sao_obj *va = car(args)->_vector;
  p_sao_obj *vb = cadr(args)->_vector;
  int i = 0;
  for (i = 0; i < car(args)->_len; i++) {
   if (!sao_is_eq(*(va + i), *(vb + i))) {
    return SAO_TAG_false;
   }
  }
  return SAO_TAG_true;
 }
 return SAO_TAG_false;
}
p_sao_obj native_add(p_sao_obj list) {
 (sao_type_check(__func__, car(list), type_integer));
 long total = car(list)->_integer;
 list = cdr(list);
 while ((car(list)))
 {
  (sao_type_check(__func__, car(list), type_integer));
  total += car(list)->_integer;
  list = cdr(list);
 }
 return sao_new_integer(total);
}
p_sao_obj native_sub(p_sao_obj list) {
 (sao_type_check(__func__, car(list), type_integer));
 long total = car(list)->_integer;
 list = cdr(list);
 while ((list)) {
  (sao_type_check(__func__, car(list), type_integer));
  total -= car(list)->_integer;
  list = cdr(list);
 }
 return sao_new_integer(total);
}
p_sao_obj native_div(p_sao_obj list) {
 (sao_type_check(__func__, car(list), type_integer));
 long total = car(list)->_integer;
 list = cdr(list);
 while ((list)) {
  (sao_type_check(__func__, car(list), type_integer));
  total /= car(list)->_integer;
  list = cdr(list);
 }
 return sao_new_integer(total);
}
p_sao_obj native_mul(p_sao_obj list) {
 (sao_type_check(__func__, car(list), type_integer));
 long total = car(list)->_integer;
 list = cdr(list);
 while ((list)) {
  (sao_type_check(__func__, car(list), type_integer));
  total *= car(list)->_integer;
  list = cdr(list);
 }
 return sao_new_integer(total);
}
p_sao_obj native_gt(p_sao_obj sexp) {
 (sao_type_check(__func__, car(sexp), type_integer));
 (sao_type_check(__func__, cadr(sexp), type_integer));
 return (car(sexp)->_integer > cadr(sexp)->_integer) ? SAO_TAG_true : SAO_TAG_nil;
}
p_sao_obj native_lt(p_sao_obj sexp) {
 (sao_type_check(__func__, car(sexp), type_integer));
 (sao_type_check(__func__, cadr(sexp), type_integer));
 return (car(sexp)->_integer < cadr(sexp)->_integer) ? SAO_TAG_true : SAO_TAG_nil;
}
p_sao_obj native_shell(p_sao_obj args) {
 sao_out_expr("native_shell todo",car(args));
 libc_(libc_printf,"printf")(",len=%d\n",sao_list_len(args));
 p_sao_obj _car;
 p_sao_obj _cdr = args;
 while( (_car = car(_cdr)) )
 {
  sao_out_expr("\nTMP _car",_car);
  _cdr = cdr(_cdr);
 }
 return SAO_TAG_nil;
}
p_sao_obj native_ffi(p_sao_obj args) {
 sao_out_expr("ffi todo",args);
 return SAO_TAG_nil;
}
p_sao_obj native_exit(p_sao_obj args) { libc_(libc_exit,"exit")(0); return SAO_TAG_nil; }
p_sao_obj native_read(p_sao_obj args) { return sao_load_expr(sao_stream_new(libc_(libc_stdin,"stdin"),stream_file)); }
p_sao_obj native_load(p_sao_obj args) {
 p_sao_obj exp;
 p_sao_obj ret = SAO_TAG_nil;
 char *filename = car(args)->_string;
 void*fp = libc_(libc_fopen,"fopen")(filename, "r");
 if (fp == 0) {
  libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"Error opening file %s\n", filename);
  return SAO_TAG_nil;
 }
 sao_stream * fw = sao_stream_new(fp,stream_file);
 for (;;) {
  exp = sao_load_expr(fw);
  if (!(exp))
   break;
  ret = sao_eval(exp, SAO_TAG_global);
 }
 libc_(libc_fclose,"fclose")(fp);
 return ret;
}
p_sao_obj native_vector(p_sao_obj args) {
 p_sao_obj sym = (sao_type_check(__func__, car(args), type_integer));
 return sao_new_vector(sym->_integer);
}
p_sao_obj native_vget(p_sao_obj args) {
 p_sao_obj vct = (sao_type_check(__func__, car(args), type_vector));
 p_sao_obj key = (sao_type_check(__func__, cadr(args), type_integer));
 if (key->_integer >= vct->_len) return SAO_TAG_nil;
 return vct->_vector[key->_integer];
}
p_sao_obj native_vset(p_sao_obj args){
 p_sao_obj vct = (sao_type_check(__func__, car(args), type_vector));
 p_sao_obj key = (sao_type_check(__func__, cadr(args), type_integer));
 if (!(caddr(args))) return SAO_TAG_nil;
 if (key->_integer >= vct->_len) return SAO_TAG_nil;
 car(args)->_vector[key->_integer] = caddr(args);
 return SAO_TAG_ok;
}
p_sao_obj native_print(p_sao_obj args) {
 sao_out_expr(0, car(args));
 libc_(libc_printf,"printf")("\n");
 return SAO_TAG_nil;
}
p_sao_obj saolang_init()
{
 sao_def_var(sao_new_symbol("print"), sao_new_native(native_print), SAO_TAG_global); sao_def_var(sao_new_symbol("lt"), sao_new_native(native_lt), SAO_TAG_global); sao_def_var(sao_new_symbol("add"), sao_new_native(native_add), SAO_TAG_global); sao_def_var(sao_new_symbol("sub"), sao_new_native(native_sub), SAO_TAG_global); sao_def_var(sao_new_symbol("exit"), sao_new_native(native_exit), SAO_TAG_global);;
 sao_def_var(sao_new_symbol("exit"), sao_new_native(native_exit), SAO_TAG_global); sao_def_var(sao_new_symbol("global"), sao_new_native(native_global), SAO_TAG_global); sao_def_var(sao_new_symbol("print"), sao_new_native(native_print), SAO_TAG_global); sao_def_var(sao_new_symbol("lt"), sao_new_native(native_lt), SAO_TAG_global); sao_def_var(sao_new_symbol("add"), sao_new_native(native_add), SAO_TAG_global); sao_def_var(sao_new_symbol("sub"), sao_new_native(native_sub), SAO_TAG_global); sao_def_var(sao_new_symbol("cmp"), sao_new_native(native_cmp), SAO_TAG_global); sao_def_var(sao_new_symbol("cons"), sao_new_native(native_cons), SAO_TAG_global); sao_def_var(sao_new_symbol("setcar"), sao_new_native(native_setcar), SAO_TAG_global); sao_def_var(sao_new_symbol("cdr"), sao_new_native(native_cdr), SAO_TAG_global); sao_def_var(sao_new_symbol("list"), sao_new_native(native_list), SAO_TAG_global);;
 return SAO_TAG_global;
}
void print_version(){ libc_(libc_printf,"printf")(" SaoLang (R) v0.0.5 - Wanjo Chan (c) 2020\n"); }
void print_help(){ libc_(libc_printf,"printf")("Usage	 : sao [options] [script.sao | -]]\nOptions	 :\n	h:	Help\n	v:	Version\n	i:	Interactive\n	p:	Print final result\n	d:	Dev only\n	e:	Eval\n	s:	Strict mode\n	l:	Lisp syntax\n"); }
int main(int argc, char **argv) {
 ffic_func strcmp = libc_(libc_strcmp,"strcmp");
 libc_(libc_setmode,"setmode")(libc_(libc_fileno,"fileno")(libc_(libc_stdin,"stdin")),0x8000 );
 ht_resize(16384-1);
 SAO_TAG_global = sao_expand(SAO_TAG_nil, SAO_TAG_nil, SAO_TAG_nil);
 SAO_TAG_argv = sao_expand(SAO_TAG_nil, SAO_TAG_nil, SAO_TAG_nil);
 do{SAO_TAG_true=sao_new_symbol("true");sao_def_var(SAO_TAG_true,SAO_TAG_true,SAO_TAG_global);}while(0); do{SAO_TAG_false=sao_new_symbol("false");sao_def_var(SAO_TAG_false,SAO_TAG_false,SAO_TAG_global);}while(0); do{SAO_TAG_quote=sao_new_symbol("quote");sao_def_var(SAO_TAG_quote,SAO_TAG_quote,SAO_TAG_global);}while(0); do{SAO_TAG_set=sao_new_symbol("set");sao_def_var(SAO_TAG_set,SAO_TAG_set,SAO_TAG_global);}while(0); do{SAO_TAG_let=sao_new_symbol("let");sao_def_var(SAO_TAG_let,SAO_TAG_let,SAO_TAG_global);}while(0); do{SAO_TAG_var=sao_new_symbol("var");sao_def_var(SAO_TAG_var,SAO_TAG_var,SAO_TAG_global);}while(0); do{SAO_TAG_procedure=sao_new_symbol("procedure");sao_def_var(SAO_TAG_procedure,SAO_TAG_procedure,SAO_TAG_global);}while(0); do{SAO_TAG_if=sao_new_symbol("if");sao_def_var(SAO_TAG_if,SAO_TAG_if,SAO_TAG_global);}while(0); do{SAO_TAG_lambda=sao_new_symbol("lambda");sao_def_var(SAO_TAG_lambda,SAO_TAG_lambda,SAO_TAG_global);}while(0); do{SAO_TAG_begin=sao_new_symbol("begin");sao_def_var(SAO_TAG_begin,SAO_TAG_begin,SAO_TAG_global);}while(0); do{SAO_TAG_or=sao_new_symbol("or");sao_def_var(SAO_TAG_or,SAO_TAG_or,SAO_TAG_global);}while(0); do{SAO_TAG_ok=sao_new_symbol("ok");sao_def_var(SAO_TAG_ok,SAO_TAG_ok,SAO_TAG_global);}while(0); do{SAO_TAG_else=sao_new_symbol("else");sao_def_var(SAO_TAG_else,SAO_TAG_else,SAO_TAG_global);}while(0); do{SAO_TAG_cond=sao_new_symbol("cond");sao_def_var(SAO_TAG_cond,SAO_TAG_cond,SAO_TAG_global);}while(0); do{SAO_TAG_error=sao_new_symbol("error");sao_def_var(SAO_TAG_error,SAO_TAG_error,SAO_TAG_global);}while(0);;
 saolang_init();
 char * script_file = "-";
 int found_any = 0;
 if(argc>1){
  char argv_line[512] = {'_','(',0};
  char * argv_ptr = &argv_line[2];
  for(int i=1;i<argc;i++){*argv_ptr++=' ';char*wk=argv[i];while(*wk)*argv_ptr++=*wk++;}
  *argv_ptr++ = ')';
  sao_stream * fw = sao_stream_new(argv_line,stream_char);
  p_sao_obj arg_expr = sao_load_expr( fw );
  p_sao_obj pos = cdr(arg_expr);
  while(pos){
   p_sao_obj _car = car(pos);
   char * string_or_name;
   int i_val = 0;
   if( (_car&&!_car->_type) ){
    p_sao_obj _caar = car(_car);
    string_or_name = _caar->_string;
    p_sao_obj _cadar = car(cdr(_car));
    i_val = (_cadar && _cadar->_type==type_integer) ? _cadar->_integer : 0;
   }else{
    string_or_name = _car->_string;
    i_val = 1;
   }
   sao_def_var(sao_new_symbol(string_or_name), sao_new_integer(i_val), SAO_TAG_argv);
   int found = 0;
   for(int i=0;i<=argt_h;i++){
    if(string_or_name[0]==argt_names[i][0]){
     argta[i]+=i_val;
     found=1;break;
    }
   }
   if(!found) script_file = string_or_name;
   else found_any++;
   pos = cdr(pos);
  }
  libc_(libc_free,"free")(fw);
  sao_def_var(SAO_TAG_argv,SAO_TAG_argv,SAO_TAG_global);
 }
 if(!found_any){ print_help();argta[argt_i]++; argta[argt_v]++; }
 else { if(argta[argt_i]){argta[argt_v]++; found_any++;} }
 if(argta[argt_v]){ print_version();if(found_any==1)libc_(libc_exit,"exit")(0); }
 if(argta[argt_h]){ print_help();libc_(libc_exit,"exit")(0);}
 void* fp = ((!strcmp("-",script_file)) ? (void*)libc_(libc_stdin,"stdin") : (void*)libc_(libc_fopen,"fopen")(script_file, "r"));
 if(!fp) do{libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"FILE NOT FOUND: %s",script_file);libc_(libc_fprintf,"fprintf")(libc_(libc_stderr,"stderr"),"\n");libc_(libc_exit,"exit")(1);}while(0);
 sao_stream * fw = sao_stream_new(fp,stream_file);
 p_sao_obj result = sao_parse( fw, 1 );
 if(argta[argt_p]){ sao_out_expr(0,result);libc_(libc_printf,"printf")("\n"); }
 libc_(libc_fclose,"fclose")(fp);
 libc_(libc_free,"free")(fw);
 return 0;
}