/**
  @file atf.c
  @brief See header for details
*/
#include "atf.h"
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#ifndef NO_SIGNALS
#include <signal.h>
#endif

char* Curr_Test = NULL;
char* Curr_File = NULL;
unsigned int Curr_Line = 0;
static unsigned int Total = 0;
static unsigned int Failed = 0;

#ifndef NO_SIGNALS
static void handle_signal(int sig) {
    /* Determine the signal name */
    char* sig_name = NULL;
    switch(sig) {
        case SIGABRT: sig_name = "SIGABRT"; break;
        case SIGBUS:  sig_name = "SIGBUS";  break;
        case SIGFPE:  sig_name = "SIGFPE";  break;
        case SIGILL:  sig_name = "SIGILL";  break;
        case SIGSEGV: sig_name = "SIGSEGV"; break;
        case SIGSYS:  sig_name = "SIGSYS";  break;
        /* If we don't recognize it then just return and let the default handler
           catch it. */
        default:      return;
    }
    /* Error and exit. No summary will be printed but the user will know which
       test has crashed. */
    fprintf(stderr,"%s:%d:0:%s:CRASH (signal: %d - %s)\n", Curr_File, Curr_Line, Curr_Test, sig, sig_name);
    Failed++;
    (void)atf_print_results();
    exit(1);
}
#endif

void atf_init(int argc, char** argv) {
    /* I reserve the right to use these later */
    (void)argc;
    (void)argv;

#ifndef NO_SIGNALS
    /* Init signal handler */
    signal(SIGABRT, handle_signal);
    signal(SIGBUS,  handle_signal);
    signal(SIGFPE,  handle_signal);
    signal(SIGILL,  handle_signal);
    signal(SIGSEGV, handle_signal);
    signal(SIGSYS,  handle_signal);
#endif
}

void atf_run_suite(suite_t suite) {
    suite();
}

void atf_test_start(char* file, unsigned int line, char* name) {
    Curr_File = file;
    Curr_Line = line;
    Curr_Test = name;
    Total++;
}

bool atf_test_assert(bool success, char* expr, char* file, int line) {
    bool failed = !success;
    if (failed) atf_test_fail(expr,file,line);
    return failed;
}

void atf_test_fail(char* expr, char* file, int line) {
    Failed++;
    printf("%s:%d:0:%s:FAIL:( %s )\n", file, line, Curr_Test, expr); \
}

int atf_print_results(void) {
    static const char* results_string =
    "\nUnit Test Summary"
    "\n-----------------"
    "\nTotal:  %d"
    "\nPassed: %d"
    "\nFailed: %d"
    "\n\n";
    printf(results_string, Total, Total - Failed, Failed);
    return Failed;
}

