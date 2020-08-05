#ifndef FFIC_H
#define FFIC_H
typedef void* ffic_ptr;
typedef ffic_ptr(*ffic_func)();
typedef double (*ffic_func_d)();
typedef long (*ffic_func_l)();
typedef int (*ffic_func_i)();
typedef float (*ffic_func_f)();
typedef char* ffic_string;
typedef int* ffic_wstring;
//#define ffic_tmp_string(n) ((char[n]){0})
#define ffic_tmp_string(n) (char[n]){0}
typedef enum { ffic_os_unknown, ffic_os_win, ffic_os_osx, ffic_os_unx, } ffic_os_t;
#ifdef _WIN32
ffic_os_t ffic_os = ffic_os_win;
ffic_string ffic_libcname = "msvcrt";
ffic_string ffic_sosuffix = ".dll";
#elif defined(__APPLE__)
ffic_os_t ffic_os = ffic_os_osx;
ffic_string ffic_libcname = "libc";
ffic_string ffic_sosuffix = ".dylib";
#else
ffic_os_t ffic_os = ffic_os_unx;
ffic_string ffic_libcname = "libc";
ffic_string ffic_sosuffix = ".so";
#endif
static ffic_func ffic_gettimeofday=(ffic_ptr)0;
#ifndef SIZEOF_POINTER
# if defined(_WIN64)
# define SIZEOF_POINTER 8 //WIN 64
# elif defined(_WIN32)
# define SIZEOF_POINTER 4 //32
//# elif //TODO 32 linux..
# else
# define SIZEOF_POINTER 8 //64 as default...
# endif
#endif
typedef struct __FILE FILE;
typedef signed char ffic_i8;
typedef unsigned char ffic_u8;
typedef signed short int ffic_i16;
typedef unsigned short int ffic_u16;
typedef signed int ffic_i32;
typedef unsigned int ffic_u32;
typedef signed long ffic_ipt;
typedef unsigned long ffic_upt;
#if SIZEOF_POINTER==8
typedef signed long int ffic_i64;
typedef unsigned long int ffic_u64;
#elif SIZEOF_POINTER==4
typedef signed long long int ffic_i64;
typedef unsigned long long int ffic_u64;
#else
#error Unknown SIZEOF_POINTER ?
#endif
///////////////////////////////////////////////
#ifndef FFIC
#define FFIC 1
#endif
#ifdef FFIC
# if FFIC==2 //{
extern void*(*ffic(const char*, const char*, ...))();
#  ifndef libc
#  define libc(f) ffic("c",#f)
#  endif
# elif FFIC==1 //}{
#  if defined(_WIN32) || defined(_WIN64) //{
#ifdef UNICODE
extern ffic_ptr LoadLibraryW(const char*);
#define dlopen LoadLibraryW
#else
extern ffic_ptr LoadLibraryA(const char*);
#define dlopen(l,c) LoadLibraryA(l)
#endif
extern ffic_ptr GetProcAddress(ffic_ptr,const char*);
#define ffic_dlsym GetProcAddress
#   else //}{
extern ffic_ptr dlopen(const char *,int);
extern ffic_ptr dlsym(ffic_ptr, const char *);
#define ffic_dlsym dlsym
#   endif //}
#define ffic_dlopen dlopen 
extern int printf(const char*,...);
extern int strcmp(const char*,const char*);//TODO improve speed https://answer-id.com/59773438
extern void exit(int);
extern int fprintf(FILE*,const char*,...);
extern int fflush(void*);
//////////////////////////////////////////////////////////////////////////
#if 0
char **envp_store;
void ffic_setup(char **envp){
	if(SIZEOF_POINTER!=sizeof(void*)){ printf("ERROR: SIZEOF_POINTER(%d) not correct, should be (%d)\n",SIZEOF_POINTER,(int) sizeof(void*)); }
	envp_store = envp;//
	for(char**env=envp;env && (*env);env++) {
		printf("DEBUG: %s\n",*env);
		//if(strcmp(*env,"OS=Windows_NT")==0){ ffic_os = ffic_os_win; }
		//if(strcmp(*env,"COMMAND_MODE=unix2003")==0){ ffic_os = ffic_os_osx; }
	}
}
#endif

char* _ffic_strcat(char* buffer, const char *source, const char* append) {
	char* ptr = buffer;
	while (*source) *(ptr++) = *(source++); while (*append) *(ptr++) = *(append++);
	*ptr = '\0';
	return buffer;
}
ffic_ptr ffic_void(){return 0;};
ffic_ptr(*ffic_raw(const char* part1, const char* funcname, const char* part2))()
{
	//ffic_ptr rt = (ffic_table_get(_tbl_ffic,_ffic_strcat(libfilename,funcname,part2)))
	//if(!rt){
	//rt = ffic_dlsym(ffic_dlopen(libfilename,0x101),funcname)
	//ffic_table_put(_tbl_ffic,_ffic_strcat(libfilename,funcname,part2))
	//}
	//return rt;
	//if(ffic_os==ffic_os_unknown){ printf("ERROR: need to call ffic_setup() first\n");exit(1); }
	char * libfilename = ffic_tmp_string(512);
	//char libfilename[512] = {0};
	_ffic_strcat(libfilename, (part1)? part1 : ffic_libcname, (part2)? part2 : ffic_sosuffix );
	ffic_ptr rt = ffic_dlsym(ffic_dlopen(libfilename,0x100 | 0x1/*RTLD_LAZY*/), funcname);
	return rt;
}
void* ffic_std[3];//
void* ffic_os_std(int t){
	if(ffic_std[t]) return ffic_std[t];
	ffic_std[t] = ffic_raw(0,(ffic_os==ffic_os_win)?"_fdopen":"fdopen",0)(ffic_raw(0,(ffic_os==ffic_os_win)?"_dup":"dup",0)(t),(t==0)?"r":"w");
	return ffic_std[t];
}
ffic_ptr ffic_usleep(int nano_seconds)
{
	if(ffic_os==ffic_os_win) ffic_raw("kernel32","Sleep",0)(nano_seconds/1000);
	else ffic_raw(ffic_libcname,"usleep",0)(nano_seconds);
	return 0;
};
ffic_ptr ffic_msleep(int microseconds)
{
	if (ffic_os==ffic_os_win) ffic_raw("kernel32","Sleep",0)(microseconds);
	else ffic_raw(ffic_libcname,"usleep",0)(microseconds*1000);
	return 0;
};
ffic_ptr ffic_sleep(int seconds)
{
	if(ffic_os==ffic_os_win) ffic_raw("kernel32","Sleep",0)(seconds*1000);
	else ffic_raw(ffic_libcname,"usleep",0)(seconds*1000000);
	return 0;
}
struct timeval { long tv_sec; long tv_usec; };
ffic_u64 ffic_microtime(void);
//int g_flag_ffic_init=0;
ffic_ptr(*ffic(const char* libname, const char* funcname, ...))()
{
	//if(g_flag_ffic_init==0){
	//	g_flag_ffic_init=1;
	//	//ffic_table_put(_tbl_ffic,'c.stderr',ffic_os_std(2));
	//	//ffic_table_put(_tbl_ffic,'c.stdout',ffic_os_std(1));
	//	//ffic_table_put(_tbl_ffic,'c.stdin',ffic_os_std(0));
	//	//ffic_table_put(_tbl_ffic,'c.microtime',ffic_microtime);
	//	//ffic_table_put(_tbl_ffic,'c.usleep',ffic_usleep);
	//	//ffic_table_put(_tbl_ffic,'c.sleep',ffic_sleep);
	//	////ffic_table_put(_tbl_ffic,'c.fileno',ffic_sleep);
	//	////ffic_table_put(_tbl_ffic,'c.setmode',ffic_sleep);
	//}
	//TODO ffic_table_get(libname . funcname)
	ffic_ptr addr = 0;
	if(!strcmp("c",libname)){
		libname = 0;
		if(!strcmp("stderr",funcname) || !strcmp("2",funcname)){ return ffic_os_std(2); }
		else if(!strcmp("stdout",funcname) || !strcmp("1",funcname)){ return ffic_os_std(1); }
		else if(!strcmp("stdin",funcname) || !strcmp("0",funcname)){ return ffic_os_std(0); }
		else if(!strcmp("microtime",funcname)){ return (ffic_ptr) ffic_microtime; }
		else if(!strcmp("usleep",funcname)){ return ffic_usleep; }
		else if(!strcmp("sleep",funcname)){ return ffic_sleep; }
		else if(!strcmp("msleep",funcname)){ return ffic_msleep; }
		else if(ffic_os == ffic_os_win && !strcmp("fileno",funcname)){ funcname = "_fileno"; }
		else if(!strcmp("setmode",funcname)){
			if(ffic_os == ffic_os_win){ funcname = "_setmode"; }else{ addr = ffic_void; }
		}
		else if(ffic_os == ffic_os_win && !strcmp("strdup",funcname)){ funcname = "_strdup"; }
	}
	//if(addr==0) addr = ffic_raw(libname,funcname,0);
	if(!addr) addr = ffic_raw(libname,funcname,0);
	//if(0==addr)
	if(!addr)
	{
		fprintf(ffic_os_std(1),"WARN: Not found %s.%s\n", libname, funcname);fflush(ffic_os_std(1));
		return ffic_void;
	}
	return addr;
}
ffic_u64 ffic_microtime(void)
{
	struct timeval tv;
	if(ffic_os == ffic_os_win){
		if (!ffic_gettimeofday) ffic_gettimeofday = ffic_raw("kernel32","GetSystemTimePreciseAsFileTime",0);
		if (!ffic_gettimeofday) ffic_gettimeofday = ffic_raw("kernel32","GetSystemTimeAsFileTime",0);
		static const ffic_u64 epoch = 116444736000000000;
		struct _FILETIME { unsigned long dwLowDateTime; unsigned long dwHighDateTime; } file_time;
		ffic_gettimeofday(&file_time);
		ffic_u64 since_1601 = ( (ffic_u64) file_time.dwHighDateTime << 32) | (ffic_u64) file_time.dwLowDateTime;
		ffic_u64 since_1970 = ((ffic_u64) since_1601 - epoch);
		ffic_u64 microseconds_since_1970 = since_1970 / 10;
		tv.tv_sec = (microseconds_since_1970 / (ffic_u64) 1000000);
		tv.tv_usec = microseconds_since_1970 % (ffic_u64) 1000000;
	}else{
		if (!ffic_gettimeofday) ffic_gettimeofday = ffic("c","gettimeofday");
		ffic_gettimeofday(&tv, 0);
	}
	return ((ffic_u64)tv.tv_sec*(ffic_u64)1000 + (((ffic_u64)tv.tv_usec)/(ffic_u64)1000)%(ffic_u64)1000);
}
#  ifndef libc
#  define libc(f) ffic("c",#f)
#  endif
# endif
#endif //} FFIC==1
#ifndef libc
# define libc(f) f
#endif

#endif//FFIC_H