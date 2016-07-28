// Unit Test Framework Includes
#include "atf.h"
#include <util.h>

// File To Test
#include <utf8.h>

static bool is_rejected(char* seq) {
    Rune rune = 0;
    size_t length = 0;
    while (*seq && !utf8decode(&rune, &length, *seq))
        seq++;
    return (rune == RUNE_ERR);
}

//-----------------------------------------------------------------------------
// Begin Unit Tests
//-----------------------------------------------------------------------------
TEST_SUITE(Utf8) {
    TEST(Verify_all_runes_in_the_unicode_database_can_be_encoded_and_decoded)
    {
        FILE* db = fopen("UnicodeData-8.0.0.txt", "r");
        while (!feof(db)) {
            char* rec = efreadline(db);
            ulong val = strtoul(rec, NULL, 16);
            /* Encode the raw val to utf8 first */
            char utf8[UTF_MAX] = {0};
            CHECK(utf8encode(utf8, (Rune)val) > 0);
            /* Decode the encoded utf8 */
            Rune rune = 0;
            size_t index = 0, len = 0;
            while (!utf8decode(&rune, &len, utf8[index]))
                index++;
            /* Check that the values match */
            if ((Rune)val != rune)
                printf("0x%04lx == 0x%04lx - %s", val, (ulong)rune, rec);
            CHECK((Rune)val == rune);
            free(rec);
        }
    }

    TEST(Verify_overlong_sequences_are_rejected)
    {
        for (unsigned char ch = 1; ch < 0x80; ch++) {
            unsigned char overlong2[] = { 0xC0, ch, 0x00 };
            CHECK(is_rejected((char*)overlong2));

            unsigned char overlong3[] = { 0xE0, 0x80, ch, 0x00 };
            CHECK(is_rejected((char*)overlong3));

            unsigned char overlong4[] = { 0xF0, 0x80, 0x80, ch, 0x00 };
            CHECK(is_rejected((char*)overlong4));

            unsigned char overlong5[] = { 0xF8, 0x80, 0x80, 0x80, ch, 0x00 };
            CHECK(is_rejected((char*)overlong5));

            unsigned char overlong6[] = { 0xFC, 0x80, 0x80, 0x80, 0x80, ch, 0x00 };
            CHECK(is_rejected((char*)overlong6));

            unsigned char overlong7[] = { 0xFE, 0x80, 0x80, 0x80, 0x80, 0x80, ch, 0x00 };
            CHECK(is_rejected((char*)overlong7));
        }
    }
}
