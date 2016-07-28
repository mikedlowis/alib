/**
  @file atf.h
  @brief Aardvark Test Framework main interface file.
  $Revision$
  $HeadURL$
*/
#ifndef TEST_H
#define TEST_H

#include <stddef.h>
#include <stdbool.h>

typedef void (*suite_t)(void);

extern char* Curr_Test;

void atf_init(int argc, char** argv);

void atf_run_suite(suite_t suite);

void atf_test_start(char* file, unsigned int line, char* name);

bool atf_test_assert(bool success, char* expr_str, char* file, int line);

void atf_test_fail(char* expr, char* file, int line);

int atf_print_results(void);

#define CHECK(expr) \
    if(atf_test_assert((expr), #expr, __FILE__, __LINE__)) break

#define TEST_SUITE(name) void name(void)

#define TEST(desc) \
    for(atf_test_start(__FILE__,__LINE__,#desc); Curr_Test != NULL; Curr_Test = NULL)

#define RUN_EXTERN_TEST_SUITE(name) \
    do { extern TEST_SUITE(name); atf_run_suite(&name); } while(0)

#define RUN_TEST_SUITE(name) \
    atf_run_suite(&name)

#define PRINT_TEST_RESULTS atf_print_results

#endif /* TEST_H */
