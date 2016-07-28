/**
  @brief Infinite lookahead and backtracking parser implementation.
  @author Michael D. Lowis
  @license BSD 2-clause License
*/
#ifndef PARSE_H
#define PARSE_H

#include <stdbool.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>

/*
    NOTE: The tokval_t is left undefined in this header file. It is intended
    that that type be defined by users of this file prior to including it. In
    this way, the user can define it in whatever way makes sense for their
    parser.
*/

typedef struct {
    int type;
    const char* file;
    size_t line;
    size_t col;
    tokval_t value;
} token_t;

typedef void (*lexfn_t)(void* data, token_t* tok);

typedef struct {
    /* Lexing  data */
    lexfn_t lexfunc;
    void* lexdata;
    /* Token buffering */
    size_t current;
    size_t tokcount;
    size_t tokcap;
    token_t* tokens;
    /* Backtracking data */
    size_t* markers;
    size_t markcount;
    size_t markcap;
} parser_t;

static void parse_error(parser_t* ctx, const char* msgfmt, ...) {
    va_list args;
    va_start(args, msgfmt);
    fprintf(stderr, "parse error: ");
    vfprintf(stderr, msgfmt, args);
    fprintf(stderr, "\n");
    va_end(args);
    exit(EXIT_FAILURE);
}

static void parse_init(parser_t* ctx, lexfn_t lexfunc, void* lexdata) {
    /* Lexer data */
    ctx->lexfunc = lexfunc;
    ctx->lexdata = lexdata;
    /* Token buffer data */
    ctx->current  = 0;
    ctx->tokcount = 0;
    ctx->tokcap   = 8;
    ctx->tokens   = calloc(ctx->tokcap, sizeof(token_t));
    /* Backtracking data */
    ctx->markcount = 0;
    ctx->markcap   = 8;
    ctx->markers   = calloc(ctx->markcap, sizeof(size_t));
}

static void seek(parser_t* ctx, size_t idx) {
    ctx->current = idx;
}

static bool speculating(parser_t* ctx) {
    return (ctx->markcount > 0);
}

static void fill(parser_t* ctx, size_t num) {
    for (size_t i = 0; i < num; i++) {
        if ((ctx->tokcount+1) >= ctx->tokcap) {
            ctx->tokcap *= 2;
            ctx->tokens  = (token_t*)realloc(ctx->tokens, ctx->tokcap * sizeof(token_t));
        }
        ctx->lexfunc(ctx->lexdata, &(ctx->tokens[ctx->tokcount]));
        ctx->tokcount++;
    }
}

static void sync(parser_t* ctx, size_t idx) {
    size_t next_index = ctx->current + idx - 1;
    size_t max_index  = (ctx->tokcount - 1);
    if (ctx->tokcount == 0)
        fill(ctx, idx);
    else if (next_index >= max_index)
        fill(ctx, next_index - max_index);
}

static void consume(parser_t* ctx) {
    ctx->current++;
    if ((ctx->current == ctx->tokcount) && !speculating(ctx)) {
        ctx->current  = 0;
        ctx->tokcount = 0;
    }
    sync(ctx,1);
}

static token_t* peektok(parser_t* ctx, size_t idx) {
    sync(ctx,idx);
    return &(ctx->tokens[ctx->current + idx - 1]);
}

static int peektype(parser_t* ctx, size_t idx) {
    return peektok(ctx, idx)->type;
}

static bool accept(parser_t* ctx, int toktype) {
    if (peektype(ctx,1) == toktype) {
        consume(ctx);
        return true;
    }
    return false;
}

static void expect(parser_t* ctx, int toktype) {
    if (peektype(ctx,1) == toktype)
        consume(ctx);
    else
        error(ctx, "Unexpected token. Expected %d, received %d\n", peektype(ctx,1), toktype);
}

static size_t mark(parser_t* ctx) {
    if ((ctx->markcount+1) >= ctx->markcap) {
        ctx->markcap *= 2;
        ctx->markers = (size_t*)realloc(ctx->markers, ctx->markcap * sizeof(size_t));
    }
    ctx->markers[ctx->markcount++] = ctx->current;
    return ctx->current;
}

static void release(parser_t* ctx) {
    size_t marker = ctx->markers[ctx->markcount-1];
    ctx->markcount--;
    seek(ctx, marker);
}

#endif /* PARSE_H */
