/**
    Simple implementation of string buffers in c.
    
    Copyright 2017, Michael D. Lowis
    
    Permission to use, copy, modify, and/or distribute this software
    for any purpose with or without fee is hereby granted, provided
    that the above copyright notice and this permission notice appear
    in all copies.
    
    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
    WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
    WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE
    AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL
    DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA
    OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER
    TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
    PERFORMANCE OF THIS SOFTWARE.
*/

typedef struct {
    size_t index;
    size_t capacity;
    char* string;
} strbuf_t;

static void strbuf_reset(strbuf_t* buf) {
    buf->index    = 0;
    buf->capacity = 0;
    buf->string   = NULL;
}

static void strbuf_clear(strbuf_t* buf) {
    buf->index = 0;
    if (NULL != buf->string)
        buf->string[0] = '\0';
}

static char* strbuf_string(strbuf_t* buf) {
    return buf->string;
}

static char* strbuf_finish(strbuf_t* buf) {
    char* str = buf->string;
    strbuf_reset(buf);
    return str;
}

static void strbuf_add_char(strbuf_t* buf, char ch) {
    /* Make sure there's space for the new char */
    if (buf->string == NULL) {
        buf->capacity = 8u;
        buf->string   = (char*)calloc(buf->capacity, 1u);
    } else if (buf->index+1u >= buf->capacity) {
        buf->capacity = buf->capacity << 1u;
        buf->string = (char*)realloc(buf->string, buf->capacity);
    }
    /* Append the char */
    buf->string[buf->index++] = ch;
    buf->string[buf->index]   = '\0';
}

static void strbuf_add_string(strbuf_t* buf, char* str) {
    while (*str)
        strbuf_add_char(buf, *(str++));
}

static void strbuf_add_rune(strbuf_t* buf, Rune rune) {
    char utf[UTF_MAX] = {0};
    utf8encode(utf, rune);
    strbuf_add_string(buf, utf);
}


static void strbuf_cat(strbuf_t* dest, strbuf_t* src) {
    strbuf_add_string(dest, strbuf_string(src));
}
