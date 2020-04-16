define_map(ctype,  long,double,int,float,i64,u64,string,struct,pointer);//etc TODO
enum { type_ctype=1+type_string, type_native, type_vector, type_table,};

#define LIST_SAO_TAG true,false,set,let,var,if,lambda,begin,or,else,cond,error,procedure
SAO_ITR(define_sao_tag, SAO_EXPAND(LIST_SAO_TAG));

//#define sao_new_table(s) sao_new((sao_obj){._type=type_table, ._size=s})
//#define sao_new_table(s) sao_new((sao_obj){._type=type_table, ._size=s,._table=SAO_EXPAND(SAO_NEW_C(p_sao_obj,s))})
#define sao_new_table(s) sao_new((sao_obj){._type=type_table, ._size=s,._table=SAO_NEW_C(p_sao_obj,s)})
//#define sao_new_vector(l) sao_new((sao_obj){._type=type_vector, ._len=l})
//#define sao_new_vector(l) sao_new((sao_obj){._type=type_vector, ._len=l,._vector=SAO_EXPAND(SAO_NEW_C(p_sao_obj,l))})
#define sao_new_vector(l) sao_new((sao_obj){._type=type_vector, ._len=l,._vector=SAO_NEW_C(p_sao_obj,l)})
#define sao_new_native(x,n) sao_new((sao_obj){._type=type_native, ._native=x,._ffi=n})
#define sao_new_lambda(params,body) cons(SAO_TAG_lambda, cons(params,body))
#define sao_new_procedure(params,body,ctx) cons(SAO_TAG_procedure, cons(params, cons(body, cons(ctx, SAO_NULL))))
#define sao_add_sym_x(x) SAO_TAG_##x=sao_new_symbol(#x);sao_var(SAO_TAG_##x,SAO_TAG_##x,SAO_TAG_global);

//TODO has bug to fix, don't use seriously.
void _sao_print(ffic_string str, p_sao_obj el){
	if (str) sao_stdout("%s ", str);
	//if (!(el)) { sao_stdout("'()"); return; }//TODO
	if (!(el)) { return; }
	//if(el->_raw) sao_stdout(" {%s<%s>} ",el->_raw,type_names[el->_type]);
	switch (el->_type) {
		case type_ctype://TODO can it be same as symbol or ctype
			sao_stdout("<ctype>"); break;
		case type_string:
			sao_stdout("\"%s\"", el->_string); break;
		case type_symbol:
			sao_stdout("%s", el->_string); break;
		case type_long:
			sao_stdout("%ld", el->_long); break;
		case type_double:
			sao_stdout("%g", el->_double); break;
		case type_native:
			sao_stdout("<function>"); break;
		case type_vector:
			sao_stdout("<vector %d>", el->_len); break;
		case type_table:
			sao_stdout("<table %d>", el->_size); break;
		case type_list:
			if (sao_is_tagged(el, SAO_TAG_procedure)) {
				sao_stdout("<closure>");//TODO mereg with lambda?
				return;
			}
			int skip=0;
			p_sao_obj *t = &el;
			if(!SAO_ARGV(l)){
				//if(!caller_string){
				//	sao_stdout(" [%s] ",caller_string);
				//}
				//else
				if ((*t)) {
					if((*t)->car && type_symbol == (*t)->car->_type){
						_sao_print(0, (*t)->car);//
						skip=1;
					}
				}
			}
			sao_stdout("(");
			while ((*t)) {
				if(!SAO_ARGV(l)){
					if(skip==1){
						skip=0;
					}else{
						sao_stdout(" ");
						_sao_print(0, (*t)->car);
					}
				}else{
					sao_stdout(" ");
					_sao_print(0, (*t)->car);
				}
				if (((*t)->cdr)) {
					if ((*t)->cdr->_type == type_list) {
						t = &(*t)->cdr;
					} else {
						_sao_print(".", (*t)->cdr);
						break;
					}
				} else
					break;
			}
			sao_stdout(")");
	}
}
//TODO 
//#include "ffic.h"
//make libsaolang.(dylib|dll|so) for ffic loading
p_sao_obj sao_not_false(p_sao_obj x) {
	p_sao_obj rt = x;
	if (!x || sao_is_eq(x, SAO_TAG_false)) rt = SAO_NULL;
	else if (x->_type == type_long && x->_long == 0) rt = SAO_NULL;
	return rt;
}
p_sao_obj sao_eval(p_sao_obj exp, p_sao_obj ctx);
p_sao_obj sao_eval_list(p_sao_obj exp, p_sao_obj ctx) {
	if (!(exp)) return SAO_NULL;
	return cons(sao_eval(car(exp), ctx), sao_eval_list(cdr(exp), ctx));
}
long sao_table_hash(const ffic_string s, int ht_len) {
	long h = 0;
	ffic_string u = s;
	while (*u) { h = (h * 256 + (*u)) % ht_len; u++; }
	return h;
}
p_sao_obj sao_table_lookup(p_sao_obj holder,ffic_string s) {
	if(!holder) sao_error("sao_table_lookup(holder)\n");
	p_sao_obj* the_table = holder->_table;
	if(!the_table) sao_error("empty _table?");
	if(!holder->_size) sao_error("empty _table.size?");
	long h = sao_table_hash(s, holder->_size);
	return the_table[h];
}
p_sao_obj sao_table_insert(p_sao_obj holder,p_sao_obj key_obj){
	if(!holder) sao_error("sao_table_insert(holder)\n");
	if(!key_obj) sao_error("sao_table_insert(key_obj)\n");

	p_sao_obj* the_table = holder->_table;
	if(!the_table) sao_error("empty _table?");
	if(!holder->_size) sao_error("empty _table.size?");
	long h = sao_table_hash(key_obj->_string, holder->_size);
	if(the_table[h]){
		//sao_warn("TODO sao_table_insert table need to resize (%d,%s)?\n",h,key_obj->_string);
	}
	//if(!the_table || (SAO_NULL != the_table[h] && SAO_NULL!=the_table[h]->_string)){
	//	int size = 2*(holder->size+1)-1 ;
	//	sao_tbl_resize(holder, size);
	//	the_table = sao_tbl_resize(holder, key_obj);//again
	//}
	the_table[h]= key_obj;
	return holder;
}
p_sao_obj sao_tbl_resize(p_sao_obj holder,int size){
	//if(!holder)
	if(!holder)
		holder = sao_new_table(size);
	//TMP...(fake resize first) TODO to implement the real resize soon
	else{
		holder->_table = SAO_NEW_C(p_sao_obj,size);//TMP
		holder->_size = size;
	}
	return holder;
}
p_sao_obj sao_eval(p_sao_obj exp, p_sao_obj ctx)
{
tail:
	if (!(exp)) { return SAO_NULL; }
	else if (exp->_type == type_long || exp->_type == type_string) { return exp; }//TODO
	else if (exp->_type == type_symbol) {
		p_sao_obj sym = sao_get_var(exp, ctx);
		if (!sym) { if(SAO_ARGV(s)){ sao_error("ERROR: symbol(%s) not found.\n",exp->_string); } } return sym;
	}
	else if(sao_is_list(exp)){
		p_sao_obj _car = car(exp);
		p_sao_obj _cadr = cadr(exp);
		if (sao_is_eq(_car, SAO_TAG_quote)) { return _cadr; }
		else if (sao_is_eq(_car, SAO_TAG_lambda)) { return sao_new_procedure(_cadr, cddr(exp), ctx); }
		else if (sao_is_eq(_car, SAO_TAG_var)) {
			if (sao_is_atom(_cadr)) sao_var(_cadr, sao_eval(caddr(exp), ctx), ctx);
			else {
				p_sao_obj closure = sao_eval(sao_new_lambda(cdr(_cadr), cddr(exp)), ctx);
				sao_var(car(_cadr), closure, ctx);
			}
			return SAO_TAG_true;
		}
		else if (sao_is_eq(_car, SAO_TAG_begin)) {
			p_sao_obj args = cdr(exp);
			for (; (cdr(args)); args = cdr(args)) sao_eval(car(args), ctx);
			exp = car(args);
			goto tail;
		}
		else if (sao_is_eq(_car, SAO_TAG_if)) {
			p_sao_obj predicate = sao_eval(_cadr, ctx);
			exp = (sao_not_false(predicate)) ? caddr(exp) : cadddr(exp);
			goto tail;
		}
		else if (sao_is_eq(_car, SAO_TAG_or)) {
			p_sao_obj predicate = sao_eval(_cadr, ctx);
			exp = (sao_not_false(predicate)) ? caddr(exp) : cadddr(exp);
			goto tail;
		}
		else if (sao_is_eq(_car, SAO_TAG_cond)) {
			p_sao_obj branch = cdr(exp);
			for (; (branch); branch = cdr(branch)) {
				if (sao_is_tagged(car(branch), SAO_TAG_else) || sao_not_false(sao_eval(caar(branch), ctx))) {
					exp = cons(SAO_TAG_begin, cdar(branch));
					goto tail;
				}
			}
			return SAO_NULL;
			//return SAO_TAG_false;//should null??
		}
		else if (sao_is_eq(_car, SAO_TAG_set)) { //TODO works in current ctx
			if (sao_is_atom(_cadr)){
				sao_set(_cadr, sao_eval(caddr(exp), ctx), ctx);
			} else {
				p_sao_obj closure =
					sao_eval(sao_new_lambda(cdr(_cadr), cddr(exp)), ctx);
				sao_set(car(_cadr), closure, ctx);
			}
			return SAO_TAG_true;
		}
		else if (sao_is_eq(_car, SAO_TAG_let)) { /* convert to lambda, TODO merge with? */
			p_sao_obj idx;
			p_sao_obj vars = SAO_NULL, vals = SAO_NULL;
			if (!_cadr) return SAO_NULL;//
			if (sao_is_atom(_cadr)) {
				for (idx = caddr(exp); (idx); idx = cdr(idx)) { vars = cons(caar(idx), vars); vals = cons(cadar(idx), vals); }
				sao_var(_cadr, sao_eval(sao_new_lambda(vars, cdddr(exp)), sao_expand(vars, vals, ctx)), ctx);
				exp = cons(_cadr, vals);
				goto tail;
			}
			for (idx = _cadr; (idx); idx = cdr(idx)) { vars = cons(caar(idx), vars); vals = cons(cadar(idx), vals); }
			exp = cons(sao_new_lambda(vars, cddr(exp)), vals);
			goto tail;
		}
		{ /* procedure( parameters, body-expr, ctx) */
			p_sao_obj proc = sao_eval(_car, ctx);
			p_sao_obj args = sao_eval_list(cdr(exp), ctx);
			if (!proc) {
				if(SAO_ARGV(s)){
					sao_print("WARNING: Invalid arguments to sao_eval:", exp);
					sao_stdout("\n");
				}
				return SAO_NULL;//should null?
			}
			if (proc->_type == type_native){ return proc->_native(args); }//TODO if empty native but ffi, should auto load into _native 
			if (sao_is_tagged(proc, SAO_TAG_procedure))
			{
				ctx = sao_expand(cadr(proc), args, cadddr(proc));
				exp = cons(SAO_TAG_begin, caddr(proc)); /* body-expr */
				goto tail;
			}
			//sao_stdout("DEBUG 800 native[%d,%d]\n",proc->_type,proc->_native);
		}
	}
	else{
		sao_print("DEBUG: unhandle atom",exp);
	}
	sao_stdout("\n");
	//return SAO_TAG_false;
	return SAO_NULL;//must
}

p_sao_obj sao_type_assert(const ffic_string func, p_sao_obj obj, int type)
{
	if (!obj) {
		//sao_stderr("Invalid argument to function %s: SAO_NULL\n", func); libc(exit)(1);
		sao_error("Invalid argument to function %s: SAO_NULL\n", func);
	} else if (obj->_type != type) {
		//sao_stderr( "ERR: function %s. expected %s got %s\n", func, type_names[type], type_names[obj->_type]); libc(exit)(1);
		sao_error( "ERR: function %s. expected %s got %s\n", func, type_names[type], type_names[obj->_type]);
	}
	return obj;
}
#define SAO_ASSERT_TYPE(x, t) (sao_type_assert((ffic_string)__func__, x, t))

p_sao_obj native_type(p_sao_obj args) { return sao_new_symbol(type_names[car(args)->_type]); }
p_sao_obj native_global(p_sao_obj args) { return SAO_TAG_global; }
p_sao_obj native_list(p_sao_obj args) { return (args); }
p_sao_obj native_cons(p_sao_obj args) { return cons(car(args), cadr(args)); }
p_sao_obj native_car(p_sao_obj args) { if(SAO_ARGV(s)) SAO_ASSERT_TYPE(car(args), type_list); return caar(args); }
p_sao_obj native_cdr(p_sao_obj args) { if(SAO_ARGV(s)) SAO_ASSERT_TYPE(car(args), type_list); return cdar(args); }
p_sao_obj native_setcar(p_sao_obj args) { SAO_ASSERT_TYPE(car(args), type_list); (args->car->car = (cadr(args))); return SAO_NULL; }
p_sao_obj native_setcdr(p_sao_obj args) { SAO_ASSERT_TYPE(car(args), type_list); (args->car->cdr = (cadr(args))); return SAO_NULL; }
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
p_sao_obj native_atom(p_sao_obj expr) {
	//sao_stderr("debug native_atom\n",expr);
	return sao_is_atom(car(expr)) ? SAO_TAG_true : SAO_TAG_false;
}
p_sao_obj native_eq(p_sao_obj args) { return sao_is_eq(car(args), cadr(args)) ? SAO_TAG_true : SAO_TAG_false; }
//TODO to change name ( avoid eq)
p_sao_obj native_same(p_sao_obj args) {
	if (sao_is_eq(car(args), cadr(args))) return SAO_TAG_true;
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
//////////////////////////////////////////////////////////////////////////////
p_sao_obj native_cmp(p_sao_obj args) {
	//sao_stderr("debug native_cmp(%d)\n",args->_type);
//	return SAO_TAG_true;
	//TODO REDESIGN !!!
	if ((car(args)->_type != type_long) || (cadr(args)->_type != type_long)) return SAO_TAG_false;
	return (car(args)->_long == cadr(args)->_long) ? SAO_TAG_true : SAO_TAG_false;
}
//p_sao_obj native_not(p_sao_obj args) { return native_cmp(args); }
p_sao_obj native_gt(p_sao_obj sexp) {
	SAO_ASSERT_TYPE(car(sexp), type_long);
	SAO_ASSERT_TYPE(cadr(sexp), type_long);
	return (car(sexp)->_long > cadr(sexp)->_long) ? SAO_TAG_true : SAO_TAG_false;
}
p_sao_obj native_lt(p_sao_obj sexp) {
	SAO_ASSERT_TYPE(car(sexp), type_long);
	SAO_ASSERT_TYPE(cadr(sexp), type_long);
	return (car(sexp)->_long < cadr(sexp)->_long) ? SAO_TAG_true : SAO_TAG_false;
}
p_sao_obj native_add(p_sao_obj list) {
	SAO_ASSERT_TYPE(car(list), type_long);
	long total = car(list)->_long;
	list = cdr(list);
	while ((car(list)))
	{
		SAO_ASSERT_TYPE(car(list), type_long);
		total += car(list)->_long;
		list = cdr(list);
	}
	return sao_new_long(total);
}
p_sao_obj native_sub(p_sao_obj list) {
	SAO_ASSERT_TYPE(car(list), type_long);
	long total = car(list)->_long;
	list = cdr(list);
	while ((list)) {
		SAO_ASSERT_TYPE(car(list), type_long);
		total -= car(list)->_long;
		list = cdr(list);
	}
	return sao_new_long(total);
}
p_sao_obj native_div(p_sao_obj list) {
	SAO_ASSERT_TYPE(car(list), type_long);
	long total = car(list)->_long;
	list = cdr(list);
	while ((list)) {
		SAO_ASSERT_TYPE(car(list), type_long);
		total /= car(list)->_long;
		list = cdr(list);
	}
	return sao_new_long(total);
}
p_sao_obj native_mul(p_sao_obj list) {
	SAO_ASSERT_TYPE(car(list), type_long);
	long total = car(list)->_long;
	list = cdr(list);
	while ((list)) {
		SAO_ASSERT_TYPE(car(list), type_long);
		total *= car(list)->_long;
		list = cdr(list);
	}
	return sao_new_long(total);
}
//////////////////////////////////////////////////////////////////////////////
//TODO tmp cat...(has mem leak)
char* sao_strcat(char * dst, char * src){
	char *target = libc(malloc)((long)libc(strlen)(dst) + (long)libc(strlen)(src) + 1);
	libc(strcpy)(target, dst);
	libc(strcat)(target, src); 
	return target;
}
p_sao_obj native_shell(p_sao_obj args) {
	p_sao_obj _car;
	p_sao_obj _cdr = args;
	char * cmd=SAO_NULL;
	while( (_car = car(_cdr)) ) {
		if(!cmd){ cmd=_car->_string; }
		else cmd=sao_strcat(cmd,_car->_string);
		_cdr = cdr(_cdr);
	}
	//libc(printf)("\nnative_shell cmd=%s\n",cmd);
	if(cmd) libc(system)(cmd);//TODO return result as data.
	return SAO_TAG_true;
}
p_sao_obj native_ffi(p_sao_obj args) {
	sao_print("ffi todo",args);
	//libc(system)("ls");
	return SAO_TAG_true;
}
p_sao_obj native_exit(p_sao_obj args) { libc(exit)(0); return SAO_NULL; }
//TODO merge read/load
p_sao_obj native_read(p_sao_obj args) { return sao_load_expr(sao_stream_new(libc(stdin),stream_file)); }
p_sao_obj native_load(p_sao_obj args) { //TODO merge with native_read() 1!
	p_sao_obj exp;
	p_sao_obj ret = SAO_NULL;
	ffic_string filename = car(args)->_string;
	//TODO
	void*fp = libc(fopen)(filename, "r");
	if (fp == 0) {
		sao_stderr("Error opening file %s\n", filename);
		return SAO_NULL;
	}
	sao_stream * fw = sao_stream_new(fp,stream_file);
	for (;;) {
		exp = sao_load_expr(fw);
		if (!exp) break;
		ret = sao_eval(exp, SAO_TAG_global);//attach to global here. TODO attach the to the caller later (or the 2nd argv)
	}
	libc(fclose)(fp);
	return ret;
}
p_sao_obj native_vector(p_sao_obj args) {
	p_sao_obj sym = SAO_ASSERT_TYPE(car(args), type_long);
	//return sao_new((sao_obj){._type=type_vector,._len=sym->_long});
	return sao_new_vector(sym->_long);
}
p_sao_obj native_vget(p_sao_obj args) {
	p_sao_obj vct = SAO_ASSERT_TYPE(car(args), type_vector);
	p_sao_obj key = SAO_ASSERT_TYPE(cadr(args), type_long);
	if (key->_long >= vct->_len) return SAO_NULL;
	return vct->_vector[key->_long];
}
p_sao_obj native_vset(p_sao_obj args){
	p_sao_obj vct = SAO_ASSERT_TYPE(car(args), type_vector);
	p_sao_obj key = SAO_ASSERT_TYPE(cadr(args), type_long);
	if (!(caddr(args))) return SAO_NULL;
	if (key->_long >= vct->_len) return SAO_NULL;
	car(args)->_vector[key->_long] = caddr(args);
	return SAO_TAG_true;
}
p_sao_obj native_print(p_sao_obj args) {
	sao_print(0, car(args));
	sao_stdout("\n");
	return SAO_NULL;
}
p_sao_obj native_c_int(p_sao_obj args) {
	p_sao_obj _car;
	p_sao_obj _cdr = args;
	char * s=SAO_NULL;
	while( (_car = car(_cdr)) ) {
		if(!s){ s=_car->_string; }
		else s=sao_strcat(s,_car->_string);
		_cdr = cdr(_cdr);
	}
	libc(printf)("\n native_c_int=%s\n",s);
	return SAO_NULL;
}
#define add_sym_list(n) sao_var(sao_new_symbol(#n), sao_new_native(native_##n,#n), SAO_TAG_global);
p_sao_obj saolang_init()
{
	sao_print = _sao_print;
	
//	ffic_func printf = libc(printf);
//	printf("saolang_init() type_ctype=%d,type_table=%d\n",type_ctype,type_table);

	SAO_ITR(sao_add_sym_x, SAO_EXPAND(LIST_SAO_TAG));

	//p_sao_obj g_symbol_holder = SAO_NULL;
	//g_symbol_holder = sao_new_table(65536-1);//TODO auto expand for the tables
	SAO_ITR(add_sym_list, print,lt,add,sub,exit);//minimum for fib.sao
	//CommonLisp: format,defun
	//Clojure:defn
	SAO_ITR(add_sym_list, //quote,cond,var(i.e. define),
			atom,eq,car,cdr,cons,//function from LISP
			);
	SAO_ITR(add_sym_list,
			exit,shell,ffi,global,//sys
			type,cons,setcar,setcdr,//core
			list,vector,vget,vset,//data structure
			load,print,read,//io
			add,sub,mul,div,cmp,lt,gt,//logic,
			is_null,is_list,pairq,eq,same,//helpers
			);
	SAO_ITR(add_sym_list, c_int);
	return SAO_TAG_global;
}

//TODO jot the symbol depth
//	p_sao_obj ret = sao_table_lookup(g_symbol_holder,s);
//	if (!(ret)) {
//		ret = sao_alloc(type_symbol);
//		ret->_string = libc(strdup)(s);
//		//ht_insert(ret);
//		sao_table_insert(g_symbol_holder,ret);
//	}else{
//		//TODO need using depth also?
//		if(!libc(strcmp)(ret->_string,s)){
//			//sao_warn("sao_table_insert again same for (%s)?\n",s);
//			//sao_table_insert(g_symbol_holder,ret);
//		}else{
//			sao_error("g_symbol_holder full? (%s,%s)\n",ret->_string,s);
//		//	int newsize = 2*(gHTable_len+1)-1 ;
//		//	ht_resize( newsize );
//		//	return sao_new_symbol(s);
//		}
//	}


