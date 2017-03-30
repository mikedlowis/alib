#include "atf.h"
#include <stdc.h>
#include <time.h>

char* ARGV0;

int main(int argc, char** argv)
{
    (void)argc;
    (void)argv;
    uint seed = (uint)time(NULL);
    srand(seed);
    printf("Random Number Generation Seed: %u\n", seed);
    RUN_EXTERN_TEST_SUITE(SList);
    RUN_EXTERN_TEST_SUITE(BSTree);
    RUN_EXTERN_TEST_SUITE(Hash);
    RUN_EXTERN_TEST_SUITE(Utf8);
    return (PRINT_TEST_RESULTS());
}
