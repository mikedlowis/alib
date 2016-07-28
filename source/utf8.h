/**
  @brief Simple UTF-8 encoding and decoding routines.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/
#ifndef UTF8_H
#define UTF8_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

typedef uint32_t Rune;

#define UTF_MAX   6u
#define RUNE_SELF ((Rune)0x80)
#define RUNE_ERR  ((Rune)0xFFFD)
#define RUNE_MAX  ((Rune)0x10FFFF)
#define RUNE_EOF  ((Rune)EOF)

static const uint8_t UTF8_SeqBits[] = { 0x00u, 0x80u, 0xC0u, 0xE0u, 0xF0u, 0xF8u, 0xFCu, 0xFEu };
static const uint8_t UTF8_SeqMask[] = { 0x00u, 0xFFu, 0x1Fu, 0x0Fu, 0x07u, 0x03u, 0x01u, 0x00u };
static const uint8_t UTF8_SeqLens[] = { 0x01u, 0x00u, 0x02u, 0x03u, 0x04u, 0x05u, 0x06u, 0x00u };

static bool runevalid(Rune val) {
    return (val <= RUNE_MAX)
        && ((val & 0xFFFEu) != 0xFFFEu)
        && ((val < 0xD800u) || (val > 0xDFFFu))
        && ((val < 0xFDD0u) || (val > 0xFDEFu));
}

static size_t runelen(Rune rune) {
    if(!runevalid(rune))
        return 0;
    else if(rune <= 0x7F)
        return 1;
    else if(rune <= 0x07FF)
        return 2;
    else if(rune <= 0xFFFF)
        return 3;
    else
        return 4;
}

static uint8_t utfseq(uint8_t byte) {
    for (int i = 1; i < 8; i++)
        if ((byte & UTF8_SeqBits[i]) == UTF8_SeqBits[i-1])
            return UTF8_SeqLens[i-1];
    return 0;
}

static size_t utf8encode(char str[UTF_MAX], Rune rune) {
    size_t len = runelen(rune);
    str[0] = (len == 1 ? 0x00 : UTF8_SeqBits[len])
           | (UTF8_SeqMask[len] & (rune >> (6 * (len-1))));
    for (int i = 1; i < len; i++)
        str[i] = 0x80u | (0x3Fu & (rune >> (6 * (len-i-1))));
    return len;
}

static bool utf8decode(Rune* rune, size_t* length, int byte) {
    /* Handle the start of a new rune */
    if (*length == 0) {
        /* If we were fed in an EOF as a start byte, handle it here */
        if (byte == EOF) {
            *rune = RUNE_EOF;
        } else {
            /* Otherwise, decode the first byte of the rune */
            *length = utfseq(byte);
            *rune   = (*length == 0) ? RUNE_ERR : (byte & UTF8_SeqMask[*length]);
            (*length)--;
        }
    /* Handle continuation bytes */
    } else if ((byte & 0xC0) == 0x80) {
        /* add bits from continuation byte to rune value
         * cannot overflow: 6 byte sequences contain 31 bits */
        *rune = (*rune << 6) | (byte & 0x3F); /* 10xxxxxx */
        (*length)--;
        /* Sanity check the final rune value before finishing */
        if ((*length == 0) && !runevalid(*rune))
            *rune = RUNE_ERR;
    /* Didn't get the continuation byte we expected */
    } else {
        *rune = RUNE_ERR;
    }
    /* Tell the caller whether we finished or not */
    return ((*length == 0) || (*rune == RUNE_ERR));
}

static size_t utflen(const char* s) {
    size_t len = 0, rlen = 0;
    Rune rune = 0;
    while (*s && !utf8decode(&rune, &len, *(s++)))
        len++;
    return len;
}

static Rune fgetrune(FILE* f) {
    Rune rune = 0;
    size_t length = 0;
    while (!utf8decode(&rune, &length, fgetc(f)));
    return rune;
}

static void fputrune(Rune rune, FILE* f) {
    char utf[UTF_MAX] = {0};
    utf8encode(utf, rune);
    fprintf(f, "%s", utf);
}

#endif /* UTF8_H */
