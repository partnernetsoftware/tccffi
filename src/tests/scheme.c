#define SAO_CAT(a, ...) SAO_PRIMITIVE_CAT(a, __VA_ARGS__)
#define SAO_PRIMITIVE_CAT(a, ...) a ## __VA_ARGS__
#define SAO_IIF(c) SAO_PRIMITIVE_CAT(SAO_IIF_, c)
#define SAO_IIF_0(t, ...) __VA_ARGS__
#define SAO_IIF_1(t, ...) t
#define SAO_CHECK_N(x, n, ...) n
#define SAO_CHECK(...) SAO_CHECK_N(__VA_ARGS__, 0,)
#define SAO_PROBE(x) x, 1,
#define SAO_IS_PAREN(x) SAO_CHECK(SAO_IS_PAREN_PROBE x)
#define SAO_IS_PAREN_PROBE(...) SAO_PROBE(~)
#define SAO_NOT(x) SAO_CHECK(SAO_PRIMITIVE_CAT(SAO_NOT_, x))
#define SAO_NOT_0 SAO_PROBE(~)
#define SAO_COMPL(b) SAO_PRIMITIVE_CAT(SAO_COMPL_, b)
#define SAO_COMPL_0 1
#define SAO_COMPL_1 0
#define SAO_BOOL(x) SAO_COMPL(SAO_NOT(x))
#define SAO_IF(c) SAO_IIF(SAO_BOOL(c))
#define SAO_EAT(...)
#define SAO_EXPAND(...) __VA_ARGS__
#define SAO_WHEN(c) SAO_IF(c)(SAO_EXPAND, SAO_EAT)
#define SAO_EMPTY()
#define SAO_DEFER(id) id SAO_EMPTY()
#define SAO_OBSTRUCT(...) __VA_ARGS__ SAO_DEFER(SAO_EMPTY)()
#define SAO_EVAL(...)  SAO_EVAL1(SAO_EVAL1(SAO_EVAL1(__VA_ARGS__)))
#define SAO_EVAL1(...) SAO_EVAL2(SAO_EVAL2(SAO_EVAL2(__VA_ARGS__)))
#define SAO_EVAL2(...) SAO_EVAL3(SAO_EVAL3(SAO_EVAL3(__VA_ARGS__)))
#define SAO_EVAL3(...) SAO_EVAL4(SAO_EVAL4(SAO_EVAL4(__VA_ARGS__)))
#define SAO_EVAL4(...) SAO_EVAL5(SAO_EVAL5(SAO_EVAL5(__VA_ARGS__)))
#define SAO_EVAL5(...) __VA_ARGS__
#define SAO_WHILE(macro, value, ...) SAO_WHEN(SAO_NOT(SAO_IS_PAREN(value ())))\
	( SAO_OBSTRUCT(macro) (value) SAO_OBSTRUCT(SAO_WHILE_INDIRECT) () (macro, __VA_ARGS__) )
#define SAO_WHILE_INDIRECT() SAO_WHILE 
#define SAO_ITR(mmm,qqq,...) SAO_EVAL( SAO_WHILE( mmm,qqq,__VA_ARGS__ ) )
#define DEFINE_ENUM(n) libc_##n,
#define LIBC_FUNC_LIST fprintf,stderr,exit,malloc,memset,strdup,strcmp,printf,\
	stdin,putc,getc,ungetc,isalnum,strchr,isdigit,isalpha,fopen,fread,fclose,feof,\
	usleep,msleep,sleep,fputc,_setmode,_fileno,setmode,fileno,\
	gettimeofday,calloc,stdout,NULL
//TODO make ffi buffer then after the ffic()
enum {
	SAO_ITR(DEFINE_ENUM,SAO_EXPAND(LIBC_FUNC_LIST))
};
void* (*libc_a[libc_NULL])();//function buffer
#define libc(f) libcf(libc_##f,#f)
#include "../ffic.h"
typedef void*(*ffi_func)();
ffi_func libcf(int fi,const char* fn){
	return libc_a[fi]?libc_a[fi]:(libc_a[fi]=ffic("c",fn));
}
//TODO change to int=>char* map
#define NULL ((void*)0)
//////////////////////////////////////////////////////////////////////////////
#define is_null(x) ((x) == 0 || (x) == NIL)
#define is_EOL(x) (is_null((x)) || (x) == EMPTY_LIST)
#define error(x) do{libc(fprintf)(libc(stderr),"%s\n",x);libc(exit)(1);}while(0)
#define caar(x) (car(car((x))))
#define cdar(x) (cdr(car((x))))
#define cadr(x) (car(cdr((x))))
#define caddr(x) (car(cdr(cdr((x)))))
#define cadddr(x) (car(cdr(cdr(cdr((x))))))
#define cadar(x) (car(cdr(car((x)))))
#define cddr(x) (cdr(cdr((x))))
#define cdadr(x) (cdr(car(cdr((x)))))
#define atom(x) (!is_null(x) && (x)->type != type_list)
#define type_check(x, t) (sao_type_check(__func__, x, t))
char *types[6] = {"integer","symbol","string","list","primitive","vector"};
typedef enum {
	type_integer, type_symbol, type_string, type_list, type_primitive, type_vector
} type_t;
typedef struct object object;
typedef object *(*primitive_t)(object *);
struct object {
	char gc;
	type_t type;
	union {
		long integer;
		char *string;
		struct {
			object **vector;
			int vsize;
		};
		struct {
			object *car;
			object *cdr;
		};
		primitive_t primitive;
	};
} __attribute__((packed));
object *ENV;
object *NIL;
object *EMPTY_LIST;
object *TRUE;
object *FALSE;
object *QUOTE;
object *DEFINE;
object *SET;
object *LET;
object *IF;
object *LAMBDA;
object *BEGIN;
object *PROCEDURE;
int is_tagged(object *cell, object *tag);
object *cons(object *x, object *y);
object *load_file(object *args);
object *cdr(object *);
object *car(object *);
object *lookup_variable(object *var, object *env);
int sao_type_check(const char *func, object *obj, type_t type);
typedef struct _FileChar FileChar, *pFileChar;
struct _FileChar {
	int c;
	FileChar * prev;
	FileChar * next;
	//pFileChar next;
};
typedef struct {
	void* fp;
	FileChar * first;
	FileChar * last;
	FileChar * current;
	long count;
} FILEWrapper;
FILEWrapper * FileWrapper_new(void* fp);
long sao_is_digit(int c);
long sao_is_alpha(int c);
long sao_is_alphanumber(int c);
object *sao_eval(object *exp, object *env);
object *sao_load_expr(FILEWrapper * fw);
void sao_comment(FILEWrapper * fw);
object *sao_load_str(FILEWrapper * fw);
object *sao_read_list(FILEWrapper * fw);
int sao_read_int(FILEWrapper * fw, int start);
int sao_peek(FILEWrapper * fw);
void sao_ungetc(int c, FILEWrapper * fw);
object *sao_make_integer(int x);
object *sao_read_symbol(FILEWrapper * fw, char start);
void sao_out_expr(char *str, object *e);
inline long sao_is_digit(int c) { return (long) libc(isdigit)(c); }
inline long sao_is_alpha(int c) { return (long) libc(isalpha)(c); }
inline long sao_is_alphanumber(int c) { return (long) libc(isalnum)(c); }
////////////////////////////////////////////////////////////////////////
struct htable { object *key; };
static struct htable *HTABLE = 0;
static int HTABLE_SIZE = 0;
static long ht_hash(const char *s) {
	long h = 0;
	char *u = (char *) s;
	while (*u) { h = (h * 256 + (*u)) % HTABLE_SIZE; u++; }
	return h;
}
int ht_init(int size) {
	if (HTABLE || !(size % 2))
		error("Hash table already initialized or even # of entries");
	//printf("ht_init %d\n",size);
	HTABLE = libc(malloc)(sizeof(struct htable) * size);
	libc(memset)(HTABLE, 0, sizeof(struct htable) * size);
	HTABLE_SIZE = size;
	if(HTABLE_SIZE==0)
		error("HTABLE_SIZE=0???");
	return size;
}
void ht_insert(object *key) {
	long h = ht_hash(key->string);
	HTABLE[h].key = key;
}
object *ht_lookup(char *s) {
	long h = ht_hash(s);
	return HTABLE[h].key;
}
////////////////////////////////////////////////////////////////////////
object *alloc() {
	object *ret = libc(malloc)(sizeof(object)); //TODO gc()
	return ret;
}
int sao_type_check(const char *func, object *obj, type_t type)
{
	if (is_null(obj)) {
		libc(fprintf)(libc(stderr), "Invalid argument to function %s: NIL\n", func);
		libc(exit)(1);
	} else if (obj->type != type) {
		libc(fprintf)(libc(stderr), "ERR: function %s. expected %s got %s\n",
				func, types[type], types[obj->type]);
		libc(exit)(1);
	}
	return 1;
}
object *make_vector(int size) {
	object *ret = alloc();
	ret->type = type_vector;
	ret->vector = libc(malloc)(sizeof(object *) * size);
	ret->vsize = size;
	libc(memset)(ret->vector, 0, size);
	return ret;
}
object *sao_make_symbol(char *s) {
	object *ret = ht_lookup(s);
	if (is_null(ret)) {
		ret = alloc();
		ret->type = type_symbol;
		ret->string = libc(strdup)(s);
		ht_insert(ret);
	}
	return ret;
}
object *make_integer(int x) {
	object *ret = alloc();
	ret->type = type_integer;
	ret->integer = x;
	return ret;
}
object *make_primitive(primitive_t x) {
	object *ret = alloc();
	ret->type = type_primitive;
	ret->primitive = x;
	return ret;
}
object *make_lambda(object *params, object *body) {
	return cons(LAMBDA, cons(params, body));
}
object *make_procedure(object *params, object *body,
		object *env) {
	return cons(PROCEDURE, cons(params, cons(body, cons(env, EMPTY_LIST))));
}
inline object *cons(object *x, object *y) {
	object *ret = alloc();
	ret->type = type_list;
	ret->car = x;
	ret->cdr = y;
	return ret;
}
inline object *car(object *cell) {
	if (is_null(cell) || cell->type != type_list) return NIL;
	return cell->car;
}
inline object *cdr(object *cell) {
	if (is_null(cell) || cell->type != type_list) return NIL;
	return cell->cdr;
}
object *append(object *l1, object *l2) {
	if (is_null(l1)) return l2;
	return cons(car(l1), append(cdr(l1), l2));
}
object *reverse(object *list, object *first) {
	if (is_null(list)) return first;
	return reverse(cdr(list), cons(car(list), first));
}
int is_equal(object *x, object *y) {
	if (x == y)
		return 1;
	if (is_null(x) || is_null(y))
		return 0;
	if (x->type != y->type)
		return 0;
	switch (x->type) {
		case type_list:
			return 0;
		case type_integer:
			return x->integer == y->integer;
		case type_symbol:
		case type_string:
			return !libc(strcmp)(x->string, y->string);
		case type_primitive:
			return 0;
		case type_vector:
			return 0;
	}
	return 0;
}
int not_false(object *x) {
	if (is_null(x) || is_equal(x, FALSE)) return 0;
	if (x->type == type_integer && x->integer == 0) return 0;
	return 1;
}
int is_tagged(object *cell, object *tag) {
	if (is_null(cell) || cell->type != type_list)
		return 0;
	return is_equal(car(cell), tag);
}
int length(object *exp) {
	if (is_null(exp)) return 0;
	return 1 + length(cdr(exp));
}
object *prim_type(object *args) {
	return sao_make_symbol(types[car(args)->type]);
}
object *prim_get_env(object *args) {
	//libc(assert)(is_null(args));
	return ENV;
}
//object *prim_set_env(object *args) {
//	ENV = car(args);
//	return NIL;
//}
object *prim_list(object *args) {
	return (args);
}
object *prim_cons(object *args) {
	return cons(car(args), cadr(args));
}
object *prim_car(object *args) {
#ifdef STRICT
	type_check(car(args), type_list);
#endif
	return caar(args);
}
object *prim_cdr(object *args) {
#ifdef STRICT
	type_check(car(args), type_list);
#endif
	return cdar(args);
}
object *prim_setcar(object *args) {
	type_check(car(args), type_list);
	(args->car->car = (cadr(args)));
	return NIL;
}
object *prim_setcdr(object *args) {
	type_check(car(args), type_list);
	(args->car->cdr = (cadr(args)));
	return NIL;
}
object *prim_is_nullq(object *args) {
	return is_EOL(car(args)) ? TRUE : FALSE;
}
object *prim_pairq(object *args) {
	if (car(args)->type != type_list)
		return FALSE;
	return (atom(caar(args)) && atom(cdar(args))) ? TRUE : FALSE;
}
object *prim_listq(object *args) {
	object *list;
	if (car(args)->type != type_list)
		return FALSE;
	for (list = car(args); !is_null(list); list = list->cdr)
		if (!is_null(list->cdr) && (list->cdr->type != type_list))
			return FALSE;
	return (car(args)->type == type_list && prim_pairq(args) != TRUE) ? TRUE : FALSE;
}
object *prim_atomq(object *sexp) {
	return atom(car(sexp)) ? TRUE : FALSE;
}
/* = primitive, only valid for numbers */
object *prim_cmp(object *args) {
	if ((car(args)->type != type_integer) || (cadr(args)->type != type_integer))
		return FALSE;
	return (car(args)->integer == cadr(args)->integer) ? TRUE : FALSE;
}
/* eq? primitive, checks memory location, or if equal values for primitives */
object *prim_eq(object *args) {
	return is_equal(car(args), cadr(args)) ? TRUE : FALSE;
}
object *prim_equal(object *args) {
	if (is_equal(car(args), cadr(args)))
		return TRUE;
	if ((car(args)->type == type_list) && (cadr(args)->type == type_list)) {
		object *a, *b;
		a = car(args);
		b = cadr(args);
		while (!is_null(a) && !is_null(b)) {
			if (!is_equal(car(a), car(b)))
				return FALSE;
			a = cdr(a);
			b = cdr(b);
		}
		return TRUE;
	}
	if ((car(args)->type == type_vector) && (cadr(args)->type == type_vector)) {
		if (car(args)->vsize != cadr(args)->vsize) {
			return FALSE;
		}
		object **va = car(args)->vector;
		object **vb = cadr(args)->vector;
		int i = 0;
		for (i = 0; i < car(args)->vsize; i++) {
			if (!is_equal(*(va + i), *(vb + i))) {
				return FALSE;
			}
		}
		return TRUE;
	}
	return FALSE;
}
object *prim_add(object *list) {
	type_check(car(list), type_integer);
	long total = car(list)->integer;
	list = cdr(list);
	while (!is_EOL(car(list))) {
		type_check(car(list), type_integer);
		total += car(list)->integer;
		list = cdr(list);
	}
	return make_integer(total);
}
object *prim_sub(object *list) {
	type_check(car(list), type_integer);
	long total = car(list)->integer;
	list = cdr(list);
	while (!is_null(list)) {
		type_check(car(list), type_integer);
		total -= car(list)->integer;
		list = cdr(list);
	}
	return make_integer(total);
}
object *prim_div(object *list) {
	type_check(car(list), type_integer);
	long total = car(list)->integer;
	list = cdr(list);
	while (!is_null(list)) {
		type_check(car(list), type_integer);
		total /= car(list)->integer;
		list = cdr(list);
	}
	return make_integer(total);
}
object *prim_mul(object *list) {
	type_check(car(list), type_integer);
	long total = car(list)->integer;
	list = cdr(list);
	while (!is_null(list)) {
		type_check(car(list), type_integer);
		total *= car(list)->integer;
		list = cdr(list);
	}
	return make_integer(total);
}
object *prim_gt(object *sexp) {
	type_check(car(sexp), type_integer);
	type_check(cadr(sexp), type_integer);
	return (car(sexp)->integer > cadr(sexp)->integer) ? TRUE : NIL;
}
object *prim_lt(object *sexp) {
	type_check(car(sexp), type_integer);
	type_check(cadr(sexp), type_integer);
	return (car(sexp)->integer < cadr(sexp)->integer) ? TRUE : NIL;
}
object *prim_exit(object *args) {
	//libc(assert)(is_null(args));
	libc(exit)(0);
	return NIL;
}
object *prim_read(object *args) {
	//libc(assert)(is_null(args));
	FILEWrapper * fw = FileWrapper_new(libc(stdin));
	return sao_load_expr(fw);
}
object *prim_vget(object *args) {
	type_check(car(args), type_vector);
	type_check(cadr(args), type_integer);
	if (cadr(args)->integer >= car(args)->vsize)
		return NIL;
	return car(args)->vector[cadr(args)->integer];
}
object *prim_vset(object *args) {
	type_check(car(args), type_vector);
	type_check(cadr(args), type_integer);
	if (is_null(caddr(args)))
		return NIL;
	if (cadr(args)->integer >= car(args)->vsize)
		return NIL;
	car(args)->vector[cadr(args)->integer] = caddr(args);
	return sao_make_symbol("ok");
}
object *prim_vec(object *args) {
	type_check(car(args), type_integer);
	return make_vector(car(args)->integer);
}
object *extend_env(object *var, object *val, object *env) {
	return cons(cons(var, val), env);
}
object *lookup_variable(object *var, object *env) {
	while (!is_null(env)) {
		object *frame = car(env);
		object *vars = car(frame);
		object *vals = cdr(frame);
		while (!is_null(vars)) {
			if (is_equal(car(vars), var))
				return car(vals);
			vars = cdr(vars);
			vals = cdr(vals);
		}
		env = cdr(env);
	}
	return NIL;
}
void set_variable(object *var, object *val, object *env) {
	while (!is_null(env)) {
		object *frame = car(env);
		object *vars = car(frame);
		object *vals = cdr(frame);
		while (!is_null(vars)) {
			if (is_equal(car(vars), var)) {
				vals->car = val;
				return;
			}
			vars = cdr(vars);
			vals = cdr(vals);
		}
		env = cdr(env);
	}
}
object *define_variable(object *var, object *val,
		object *env) {
	object *frame = car(env);
	object *vars = car(frame);
	object *vals = cdr(frame);
	while (!is_null(vars)) {
		if (is_equal(var, car(vars))) {
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
char type_symbolS[] = "~!@#$%^&*_-+\\:,.<>|{}[]?=/";
int depth = 0;
object *eval_list(object *exp, object *env) {
	if (is_null(exp)) return NIL;
	return cons(sao_eval(car(exp), env), eval_list(cdr(exp), env));
}
object *eval_sequence(object *exps, object *env) {
	if (is_null(cdr(exps))) return sao_eval(car(exps), env);
	sao_eval(car(exps), env);
	return eval_sequence(cdr(exps), env);
}
object *load_file(object *args) {
	object *exp;
	object *ret = 0;
	char *filename = car(args)->string;
#if defined(DEBUG)
	libc(printf)("Evaluating file %s\n", filename);
#endif
	void*fp = libc(fopen)(filename, "r");
	if (fp == 0) {
		libc(printf)("Error opening file %s\n", filename);
		return NIL;
	}
	FILEWrapper * fw = FileWrapper_new(fp);
	for (;;) {
		exp = sao_load_expr(fw);
		if (is_null(exp))
			break;
		ret = sao_eval(exp, ENV);
	}
	libc(fclose)(fp);
	return ret;
}
#if defined(DEBUG)
#include "debug_scheme.c"
#endif
#define PROFILE
static long ffi_microtime(void)
{
#ifdef _WIN32
	return (long)(ffic("kernel32","GetTickCount")());
#else
	struct timeval {
		long tv_sec;
		long tv_usec;
	};
	struct timeval * tv = libc(calloc)(sizeof(struct timeval),sizeof(char));
	libc(gettimeofday)(tv, 0);
	return tv->tv_sec*1000 + (tv->tv_usec+500)/1000;
#endif
}
#define NEW_OBJECT(t,name) t*name=libc(calloc)(sizeof(t),sizeof(char));
FILEWrapper * FileWrapper_new(void* fp)
{
	//FILEWrapper * fw = libc(calloc)(sizeof(FILEWrapper),sizeof(char));
	NEW_OBJECT(FILEWrapper,fw);
	fw->fp = fp;
	fw->current = fw->last = fw->first = (void*)0;
	fw->count = 0;
	return fw;
}
void FileWrapper_feed(FILEWrapper* fw)
{
	ffi_func fread = libc(fread);
	int ok=0,ko=0;
	int k=0;
	int ct = 0;
	for(;;)
	{
		if(1==(long)fread(&k,sizeof(char),1,fw->fp))
		{
			NEW_OBJECT(FileChar,fc);
			fc->c = k; 
			fc->next = (void*) 0;
			fc->prev = (void*) 0;
			//printf("%d-%c ",k,k);
			if(0==fw->first){
				fw->first = fc;
				fw->current = fc;
			}
			if(0==fw->last){
				fw->last = fc;
			}else{
				fc->prev = fw->last;//
				fw->last->next = fc;
				fw->last = fc;
			}
			fw->count+=1;
			ok++;
		}else{ //printf("ct=%d,ok=%d,ko=%d,k=%d,EOF=%s\n",ct,ok,ko,k,feof(fw->fp)?"Y":"N");
			return;
		}
		ct++;
	}
}
int sao_getc(FILEWrapper *fw)
{
	int c;
	FileChar * current = fw->current;
	if(current!=0){
		c = current->c;
		fw->current=current->next;
	}else{
		c = -1;
	}
	return c;
}
object *sao_prim_print(object *args) {
	sao_out_expr(0, car(args));
	libc(printf)("\n");
	return NIL;
}
object *sao_read_symbol(FILEWrapper * fw, char start)
{
	char buf[128];
	buf[0] = start;
	int i = 1;
	while (sao_is_alphanumber(sao_peek(fw))
			|| libc(strchr)(type_symbolS, sao_peek(fw)))
	{
		if (i >= 128)
			error("Symbol name too long - maximum length 128 characters");
		buf[i++] = sao_getc(fw);
	}
	buf[i] = '\0';
	return sao_make_symbol(buf);
}
object *sao_make_integer(int x)
{
	object *ret = alloc();
	ret->type = type_integer;
	ret->integer = x;
	return ret;
}
void sao_ungetc(int c, FILEWrapper * fw)
{
	FileChar * current = fw->current;
	if(current!=0){
		c = current->c;
		fw->current=current->prev;
	}else{
		//printf("sao_ungetc() TODO\n");
	}
}
int sao_peek(FILEWrapper * fw)
{
	int c = sao_getc(fw);
	sao_ungetc(c, fw);
	return c;
}
int sao_read_int(FILEWrapper * fw, int start)
{
	while ( sao_is_digit(sao_peek(fw)) )
		start = start * 10 + (sao_getc(fw) - '0');
	return start;
}
object *sao_read_list(FILEWrapper * fw)
{
	object *obj;
	object *cell = EMPTY_LIST;
	for (;;) {
		obj = sao_load_expr(fw);
		if (obj == EMPTY_LIST)
			return reverse(cell, EMPTY_LIST);
		cell = cons(obj, cell);
	}
	return EMPTY_LIST;
}
inline object *sao_load_str(FILEWrapper * fw)
{
	char buf[256];
	int i = 0;
	int c;
	while ((c = sao_getc(fw)) != '\"') {
		if (c == (-1))
			return NIL;
		if (i >= 256) error("String too long - maximum length 256 characters");
		buf[i++] = (char) c;
	}
	buf[i] = '\0';
	object *s = sao_make_symbol(buf);
	s->type = type_string;
	return s;
}
void sao_comment(FILEWrapper * fw)
{
	int c;
	for (;;) {
		c = sao_getc(fw);
		if (c == '\n' || c == (-1)) return;
	}
}
object *sao_load_expr(FILEWrapper * fw)
{
	int c;
	for (;;) { //TODO switch(){}
		c = sao_getc(fw);
		if (c == (-1)) return 0;
		if (c == '\n' || c == '\r' || c == ' ' || c == '\t') {
			//if ((c == '\n' || c == '\r') && is_stdin) {
			//if "-i"
			//	for (int i = 0; i < depth; i++) libc(printf)("..");
			//}
			continue;
		}
		if (c == '\"') return sao_load_str(fw);
		if (c == ';') {
			sao_comment(fw);
			continue;
		}
		if (c == '\'') return cons(QUOTE, cons(sao_load_expr(fw), NIL));
		if (c == '(') {
			depth++;
			return sao_read_list(fw);
		}
		if (c == ')') {
			depth--;
			return EMPTY_LIST;
		}
		if (sao_is_digit(c)) return sao_make_integer(sao_read_int(fw, c - '0'));
		if (c == '-' && sao_is_digit(sao_peek(fw)))
			return sao_make_integer(-1 * sao_read_int(fw, sao_getc(fw) - '0'));
		if (libc(isalpha)(c) || libc(strchr)(type_symbolS, c))
			return sao_read_symbol(fw, c);
	}
	return NIL;
}
void sao_out_expr(char *str, object *e)
{
	if (str)
		libc(printf)("%s ", str);
	if (is_null(e)) {
		libc(printf)("'()");
		return;
	}
	switch (e->type) {
		case type_string:
			libc(printf)("\"%s\"", e->string);
			break;
		case type_symbol:
			libc(printf)("%s", e->string);
			break;
		case type_integer:
			libc(printf)("%ld", e->integer);
			break;
		case type_primitive:
			libc(printf)("<function>");
			break;
		case type_vector:
			libc(printf)("<vector %d>", e->vsize);
			break;
		case type_list:
			if (is_tagged(e, PROCEDURE)) {
				libc(printf)("<closure>");
				return;
			}
			libc(printf)("(");
			object **t = &e;
			while (!is_null(*t)) {
				sao_out_expr(0, (*t)->car);
				if (!is_null((*t)->cdr)) {
					libc(printf)(" ");
					if ((*t)->cdr->type == type_list) {
						t = &(*t)->cdr;
					} else {
						sao_out_expr(".", (*t)->cdr);
						break;
					}
				} else
					break;
			}
			libc(printf)(")");
	}
}
object *sao_eval(object *exp, object *env)
{
tail:
	if (is_null(exp) || exp == EMPTY_LIST) {
		return NIL;
	} else if (exp->type == type_integer || exp->type == type_string) {
		return exp;
	} else if (exp->type == type_symbol) {
		object *s = lookup_variable(exp, env);
#ifdef STRICT
		if (is_null(s)) {
			sao_out_expr("Unbound symbol:", exp);
			printf("\n");
		}
#endif
		return s;
	} else if (is_tagged(exp, QUOTE)) {
		return cadr(exp);
	} else if (is_tagged(exp, LAMBDA)) {
		return make_procedure(cadr(exp), cddr(exp), env);
	} else if (is_tagged(exp, DEFINE)) {
		if (atom(cadr(exp)))
			define_variable(cadr(exp), sao_eval(caddr(exp), env), env);
		else {
			object *closure =
				sao_eval(make_lambda(cdr(cadr(exp)), cddr(exp)), env);
			define_variable(car(cadr(exp)), closure, env);
		}
		return sao_make_symbol("ok");
	} else if (is_tagged(exp, BEGIN)) {
		object *args = cdr(exp);
		for (; !is_null(cdr(args)); args = cdr(args))
			sao_eval(car(args), env);
		exp = car(args);
		goto tail;
	} else if (is_tagged(exp, IF)) {
		object *predicate = sao_eval(cadr(exp), env);
		exp = (not_false(predicate)) ? caddr(exp) : cadddr(exp);
		goto tail;
	} else if (is_tagged(exp, sao_make_symbol("or"))) {
		object *predicate = sao_eval(cadr(exp), env);
		exp = (not_false(predicate)) ? caddr(exp) : cadddr(exp);
		goto tail;
	} else if (is_tagged(exp, sao_make_symbol("cond"))) {
		object *branch = cdr(exp);
		for (; !is_null(branch); branch = cdr(branch)) {
			if (is_tagged(car(branch), sao_make_symbol("else")) ||
					not_false(sao_eval(caar(branch), env))) {
				exp = cons(BEGIN, cdar(branch));
				goto tail;
			}
		}
		return NIL;
	} else if (is_tagged(exp, SET)) {
		if (atom(cadr(exp)))
			set_variable(cadr(exp), sao_eval(caddr(exp), env), env);
		else {
			object *closure =
				sao_eval(make_lambda(cdr(cadr(exp)), cddr(exp)), env);
			set_variable(car(cadr(exp)), closure, env);
		}
		return sao_make_symbol("ok");
	} else if (is_tagged(exp, LET)) {
		/* We go with the strategy of transforming let into a lambda function*/
		object **tmp;
		object *vars = NIL;
		object *vals = NIL;
		if (is_null(cadr(exp)))
			return NIL;
		/* NAMED LET */
		if (atom(cadr(exp))) {
			for (tmp = &exp->cdr->cdr->car; !is_null(*tmp); tmp = &(*tmp)->cdr) {
				vars = cons(caar(*tmp), vars);
				vals = cons(cadar(*tmp), vals);
			}
			/* Define the named let as a lambda function */
			define_variable(cadr(exp),
					sao_eval(make_lambda(vars, cdr(cddr(exp))),
						extend_env(vars, vals, env)),
					env);
			/* Then evaluate the lambda function with the starting values */
			exp = cons(cadr(exp), vals);
			goto tail;
		}
		for (tmp = &exp->cdr->car; !is_null(*tmp); tmp = &(*tmp)->cdr) {
			vars = cons(caar(*tmp), vars);
			vals = cons(cadar(*tmp), vals);
		}
		exp = cons(make_lambda(vars, cddr(exp)), vals);
		goto tail;
	} else {
		/* procedure structure is as follows:
			 ('procedure, (parameters), (body), (env)) */
		object *proc = sao_eval(car(exp), env);
		object *args = eval_list(cdr(exp), env);
		if (is_null(proc)) {
#ifdef STRICT
			sao_out_expr("Invalid arguments to sao_eval:", exp);
			printf("\n");
#endif
			return NIL;
		}
		if (proc->type == type_primitive)
			return proc->primitive(args);
		if (is_tagged(proc, PROCEDURE)) {
			env = extend_env(cadr(proc), args, cadddr(proc));
			exp = cons(BEGIN, caddr(proc)); /* procedure body */
			goto tail;
		}
	}
	sao_out_expr("Invalid arguments to sao_eval:", exp);
	libc(printf)("\n");
	return NIL;
}
void init_env() {
#define add_prim(s, c) define_variable(sao_make_symbol(s), make_primitive(c), ENV)
#define add_sym(s, c) do{c=sao_make_symbol(s);define_variable(c,c,ENV);}while(0);
	ENV = extend_env(NIL, NIL, NIL);
	add_sym("#t", TRUE);
	add_sym("#f", FALSE);
	add_sym("quote", QUOTE);
	add_sym("lambda", LAMBDA);
	add_sym("procedure", PROCEDURE);
	//TODO merge three:
	add_sym("define", DEFINE);
	add_sym("let", LET);
	add_sym("set!", SET);
	add_sym("begin", BEGIN);//TODO remove or add END
	add_sym("if", IF);
	define_variable(sao_make_symbol("true"), TRUE, ENV);
	define_variable(sao_make_symbol("false"), FALSE, ENV);
	add_prim("cons", prim_cons);
	add_prim("car", prim_car);
	add_prim("cdr", prim_cdr);
	add_prim("set-car!", prim_setcar);
	add_prim("set-cdr!", prim_setcdr);
	add_prim("list", prim_list);
	add_prim("list?", prim_listq);
	add_prim("null?", prim_is_nullq);
	add_prim("pair?", prim_pairq);
	add_prim("atom?", prim_atomq);
	add_prim("eq?", prim_eq);
	add_prim("equal?", prim_equal);
	add_prim("+", prim_add);
	add_prim("-", prim_sub);
	add_prim("*", prim_mul);
	add_prim("/", prim_div);
	add_prim("=", prim_cmp);
	add_prim("<", prim_lt);
	add_prim(">", prim_gt);
	add_prim("type", prim_type);
	add_prim("load", load_file);
	add_prim("print", sao_prim_print);
	//add_prim("ffi", prim_ffi);
	//add_prim("get-global-environment", prim_get_env);
	add_prim("env", prim_get_env);//remove this feature
	//	add_prim("set-global-environment", prim_set_env);
	add_prim("exit", prim_exit);//TODO change to ffi
	//add_prim("exec", prim_exec);//TODO change to ffi
	add_prim("read", prim_read);//read from stdin (like scan)
	add_prim("vector", prim_vec);
	add_prim("vector-get", prim_vget);
	add_prim("vector-set", prim_vset);

	add_prim("add", prim_add);
	add_prim("sub", prim_sub);
	add_prim("mul", prim_mul);
	add_prim("div", prim_div);
	add_prim("cmp", prim_cmp);
	add_prim("lt", prim_lt);
	add_prim("gt", prim_gt);
}
int main(int argc, char **argv)
{
	//TODO fprintf(stderr,)
	ffi_func printf = libc(printf);
	ht_init(8192-1);
	init_env();
	libc(setmode)(libc(fileno)(libc(stdin)),0x8000/*O_BINARY*/);
	FILEWrapper * fw = FileWrapper_new(libc(stdin));
	for(;;){
		//producer:
		FileWrapper_feed(fw);
		object *obj = sao_load_expr(fw);
		if (!is_null(obj)) {
#if defined(PROFILE)
			printf("%lu: ",ffi_microtime());
#endif
			sao_out_expr("<=", obj);
			printf("\n");
			object *exp = sao_eval(obj, ENV);
			if (!is_null(exp)) {
#if defined(PROFILE)
				printf("%lu: ",ffi_microtime());
#endif
				//TODO if "-i"
				sao_out_expr("=>", exp);
				printf("\n");
			}else{
				//sao_out_expr("null??",exp);
				//printf("\n");
			}
		}else{
			printf(" end ");
			break;
		}
	}
	return 0;
}
