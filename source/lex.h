/**
  @brief Helper utilities for writing a handwritten lexer.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/
#ifndef LEX_H
#define LEX_H

#include <utf8.h>
#include <strbuf.h>

typedef struct lex_ctx_t {
    FILE* input;
    size_t line;
    size_t col;
    struct lex_ctx_t* next;
} lex_ctx_t;

typedef struct {
    strbuf_t buf;
    Rune crune;
    lex_ctx_t* ctx;
} lexer_t;

static void lex_error(parser_t* ctx, const char* msgfmt, ...) {
    va_list args;
    va_start(args, msgfmt);
    fprintf(stderr, "lex error: ");
    vfprintf(stderr, msgfmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

static void lex_open(lexer_t* l, FILE* f) {
    lex_ctx_t* ctx = (lex_ctx_t*)calloc(1, sizeof(lex_ctx_t));
    assert(ctx != NULL);
    ctx->input = f;
    ctx->line  = 1;
    ctx->col   = 1;
    ctx->next  = l->ctx;
    l->ctx     = ctx;
}

static void lex_close(lexer_t* l) {
    if (!l || !l->ctx) return;
    lex_ctx_t* ctx = l->ctx;
    fclose(ctx->input);
    l->ctx = ctx->next;
    free(ctx);
}

static Rune peekc(lexer_t* l) {
    if (l->crune == 0) {
        /* Handle end of current file */
        if (feof(l->ctx->input))
            lex_close();
        /* Handle end of last file */
        if (lex->ctx == NULL)
            return RUNE_EOF;
        /* Get the next rune from the current file */
        l->crune = fgetrune(l->input);
        if (l->crune == '\n') {
            l->line++;
            l->col = 0;
        }
        l->col++;
    }
    return l->crune;
}

static Rune takec(lexer_t* l) {
    Rune r = peekc(l);
    strbuf_add_rune(&(l->buf), r);
    l->crune = 0;
    return r;
}

static bool take_if(lexer_t* l, bool (*testfn)(Rune)) {
    if (testfn(peekc(l))) {
        takec(l);
        return true;
    }
    return false;
}

static bool take_if_not(lexer_t* l, bool (*testfn)(Rune)) {
    if (!testfn(peekc(l))) {
        takec(l);
        return true;
    }
    return false;
}

static void take_while(lexer_t* l, bool (*testfn)(Rune)) {
    while(take_if(testfn));
}

static void take_till(lexer_t* l, bool (*testfn)(Rune)) {
    while(take_if_not(testfn));
}

static void take_tillc(lexer_t* l, Rune r) {
    while(r != peekc(l))
        takec(l);
}

static void skipc(lexer_t* l) {
    Rune r = peekc(l);
    strbuf_add_rune(&(l->buf), r);
    l->crune = 0;
}

static bool skip_if(lexer_t* l, bool (*testfn)(Rune)) {
    if (testfn(peekc(l))) {
        skipc(l);
        return true;
    }
    return false;
}

static bool skip_if_not(lexer_t* l, bool (*testfn)(Rune)) {
    if (!testfn(peekc(l))) {
        skipc(l);
        return true;
    }
    return false;
}

static void skip_while(lexer_t* l, bool (*testfn)(Rune)) {
    while(skip_if(testfn));
}

static void skip_till(lexer_t* l, bool (*testfn)(Rune)) {
    while(skip_if_not(testfn));
}

static void skip_tillc(lexer_t* l, Rune r) {
    while(r != peekc(l))
        skipc(l);
}

static bool is_whitespace(Rune r) {
    return (r == ' ' || r == '\t' || r == '\r' || r == '\n');
}

static bool is_alpha(Rune r) {
    return ('a' <= r && r <= 'z') || ('A' <= r && r <= 'Z');
}

static bool is_dec(Rune r) {
    return ('0' <= r && r <= '9');
}

static bool is_oct(Rune r) {
    return ('0' <= r && r <= '8');
}

static bool is_hex(Rune r) {
    return (is_dec(r) || ('a' <= r && r <= 'f') || ('A' <= r && r <= 'F'));
}

static bool is_alnum(Rune r) {
    return (is_alpha(r) || isdec(r));
}

#endif
