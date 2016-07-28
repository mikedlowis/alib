/**
  @brief Collection of useful C types and functions.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/
#ifndef UTIL_H
#define UTIL_H

/* Standard Macros and Types */
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <errno.h>
#include <limits.h>
#include <assert.h>

/* Useful Standard Functions */
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

/* Type Definitions
 *****************************************************************************/
typedef unsigned short ushort;
typedef unsigned char uchar;
typedef unsigned long ulong;
typedef unsigned int uint;
typedef signed char schar;
typedef long long vlong;
typedef unsigned long long uvlong;

/* Generic Death Function
 *****************************************************************************/
static void die(const char* msgfmt, ...) {
    va_list args;
    va_start(args, msgfmt);
    #ifdef CLEANUP_HOOK
    CLEANUP_HOOK();
    #endif
    fprintf(stderr, "Error: ");
    vfprintf(stderr, msgfmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

/* Generic Warning Function
 *****************************************************************************/
static void warn(const char* msgfmt, ...) {
    va_list args;
    va_start(args, msgfmt);
    fprintf(stderr, "Warning: ");
    vfprintf(stderr, msgfmt, args);
    fprintf(stderr, "\n");
    va_end(args);
}

/* Signal Handling
 *****************************************************************************/
static void esignal(int sig, void (*func)(int)) {
    errno = 0;
    func  = signal(sig, func);
    if (func == SIG_ERR || errno > 0)
        die("failed to register signal handler for signal %d", sig);
}

static int eraise(int sig) {
    int ret;
    if ((ret = raise(sig)) != 0)
        die("failed to raise signal %d", sig);
    return ret;
}

/* Dynamic Allocation
 *****************************************************************************/
static void* ecalloc(size_t num, size_t size) {
    void* ret;
    if (NULL == (ret = calloc(num,size)))
        die("out of memory");
    return ret;
}

static void* emalloc(size_t size) {
    void* ret;
    if (NULL == (ret = malloc(size)))
        die("out of memory");
    return ret;
}

static void* erealloc(void* ptr, size_t size) {
    void* ret;
    if (NULL == (ret = realloc(ptr,size)))
        die("out of memory");
    return ret;
}

/* File Handling
 *****************************************************************************/
static FILE* efopen(const char* filename, const char* mode) {
    FILE* file;
    errno = 0;
    if (NULL == (file = fopen(filename, mode)) || errno != 0)
        die("failed to open file: %s", filename);
    return file;
}

static char* efreadline(FILE* input) {
    size_t size  = 8;
    size_t index = 0;
    char*  str   = (char*)emalloc(size);
    memset(str, 0, 8);
    if (feof(input)) {
        free(str);
        return NULL;
    }
    while (true) {
        char ch = fgetc(input);
        if (ch == EOF) break;
        str[index++] = ch;
        str[index]   = '\0';
        if (index+1 >= size) {
            size = size << 1;
            str  = erealloc(str, size);
        }
        if (ch == '\n') break;
    }
    return str;
}

/* String Handling
 *****************************************************************************/
static char* estrdup(const char *s) {
    char* ns = (char*)emalloc(strlen(s) + 1);
    strcpy(ns,s);
    return ns;
}

/* Option Parsing
 *
 * This following macros implement a simple POSIX-style option parsing strategy.
 * They are heavily influenced and inspired by the arg.h file from suckless.org
 * (http://git.suckless.org/libsl/tree/arg.h). That file is in turn inspired by
 * the corresponding macros defined in plan9.
 *
 * The interface assumes that the main function will have the following
 * prototype:
 *
 * int main(int argc, char** argv);
 *
 * An example usage of the interface would look something like the follwoing:
 *
 * char* ARGV0;
 * int main(int argc, char** argv) {
 *     OPTBEGIN {
 *         case 'a': printf("Simple option\n"); break;
 *         case 'b': printf("Option with arg: %s\n", OPTARG()); break;
 *         default:  printf("Unknown option!\n");
 *     } OPTEND;
 *     return 0;
 * }
 */

/* This variable contains the value of argv[0] so that it can be referenced
 * again once the option parsing is done. This variable must be defined by the
 * program.
 *
 * NOTE: Ensure that you define this variable with external linkage (i.e. not
 * static) */
extern char* ARGV0;

/* This is a helper function used by the following macros to parse the next
 * option from the command line. */
static inline char* _getopt_(int* p_argc, char*** p_argv) {
    if (!(*p_argv)[0][1] && !(*p_argv)[1]) {
        return (char*)0;
    } else if ((*p_argv)[0][1]) {
        return &(*p_argv)[0][1];
    } else {
        *p_argv = *p_argv + 1;
        *p_argc = *p_argc - 1;
        return (*p_argv)[0];
    }
}

/* This macro is almost identical to the ARGBEGIN macro from suckless.org. If
 * it ain't broke, don't fix it. */
#define OPTBEGIN                                                              \
    for (                                                                     \
        ARGV0 = *argv, argc--, argv++;                                        \
        argv[0] && argv[0][1] && argv[0][0] == '-';                           \
        argc--, argv++                                                        \
    ) {                                                                       \
        int brk_; char argc_ , **argv_, *optarg_;                             \
        if (argv[0][1] == '-' && !argv[0][2]) {                               \
            argv++, argc--; break;                                            \
        }                                                                     \
        for (brk_=0, argv[0]++, argv_=argv; argv[0][0] && !brk_; argv[0]++) { \
            if (argv_ != argv) break;                                         \
            argc_ = argv[0][0];                                               \
            switch (argc_)

/* Terminate the option parsing. */
#define OPTEND }}

/* Get the current option character */
#define OPTC() (argc_)

/* Get an argument from the command line and return it as a string. If no
 * argument is available, this macro returns NULL */
#define OPTARG() \
    (optarg_ = _getopt_(&argc,&argv), brk_ = (optarg_!=0), optarg_)

/* Get an argument from the command line and return it as a string. If no
 * argument is available, this macro executes the provided code. If that code
 * returns, then abort is called. */
#define EOPTARG(code) \
    (optarg_ = _getopt_(&argc,&argv), \
    (!optarg_ ? ((code), abort(), (char*)0) : (brk_ = 1, optarg_)))

/* Helper macro to recognize number options */
#define OPTNUM \
    case '0':  \
    case '1':  \
    case '2':  \
    case '3':  \
    case '4':  \
    case '5':  \
    case '6':  \
    case '7':  \
    case '8':  \
    case '9'

/* Helper macro to recognize "long" options ala GNU style. */
#define OPTLONG \
    case '-'

/* Error Handling
 *****************************************************************************/
#ifdef NDEBUG
    #define debug(msg, ...) \
        ((void)0)
#else
    #define debug(msg, ...) \
        fprintf(stderr, "DEBUG %s:%d: " msg "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define errnostr() \
    (errno == 0 ? "None" : strerror(errno))

#define print_error(msg, ...) \
    fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " msg "\n", __FILE__, __LINE__, errnostr(), ##__VA_ARGS__)

#define check(expr, msg, ...) \
    if(!(expr)) { print_error(msg, ##__VA_ARGS__); errno=0; goto error; }

#define sentinel(msg, ...) \
    { print_error(msg, ##__VA_ARGS__); errno=0; goto error; }

/* Miscellaneous
 *****************************************************************************/
#ifndef nelem
    #define nelem(x) \
        (sizeof(x)/sizeof((x)[0]))
#endif

#ifndef container_of
    #define container_of(obj, type, member) \
        (type*)((uintptr_t)obj - offsetof(type, member))
#endif

#define concat(a,b) \
    a##b

#define ident(a) \
    concat(id, a)

#define unique_id \
    ident(__LINE__)

#ifndef static_assert
    #define static_assert(expr) \
        typedef char unique_id[( expr )?1:-1]
#endif

#endif
