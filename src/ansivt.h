/*
    Minimal set of routines for working with ANSI terminal escape codes for 
    controlling terminal operation on POSIX systems.
        
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
#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <termios.h>

static void ttyresp(char* buf, int bufsz) {
    for (int i = 0; i < bufsz; i++) {
        if (read(STDIN_FILENO, buf+i, 1) != 1) break;
        if (buf[i] == 'R') break;
    }
}

static void ttysendrecv(const char* req, const char* resp, ...) {
    char buf[32] = {0};
    (void)write(STDOUT_FILENO, req, strlen(req));
    ttyresp(buf, sizeof(buf)-1);
    va_list args;
    va_start(args, resp);
    vsscanf(buf, resp, args);
    va_end(args);
}

static bool ttyrawmode(bool enable) {
    static bool enabled = false;
    static struct termios orig_termios;
    if (enabled && !enable) {
        tcsetattr(STDOUT_FILENO, TCSAFLUSH, &orig_termios);
    } else if (!enabled && enable){
        if (tcgetattr(STDOUT_FILENO, &orig_termios) < 0) goto done;
        struct termios raw = orig_termios;
        raw.c_iflag    &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
        raw.c_oflag    &= ~(OPOST);
        raw.c_cflag    |= (CS8);
        raw.c_lflag    &= ~(ECHO | ICANON | IEXTEN | ISIG);
        raw.c_cc[VMIN]  = 0;
        raw.c_cc[VTIME] = 1;
        if (tcsetattr(STDOUT_FILENO,TCSAFLUSH, &raw) < 0) goto done;
        enabled = true;
    }
done:
    return enabled;
}

enum {
    /* Special Keys */
    KEY_F1               = (0xFFFF-0),
    KEY_F2               = (0xFFFF-1),
    KEY_F3               = (0xFFFF-2),
    KEY_F4               = (0xFFFF-3),
    KEY_F5               = (0xFFFF-4),
    KEY_F6               = (0xFFFF-5),
    KEY_F7               = (0xFFFF-6),
    KEY_F8               = (0xFFFF-7),
    KEY_F9               = (0xFFFF-8),
    KEY_F10              = (0xFFFF-9),
    KEY_F11              = (0xFFFF-10),
    KEY_F12              = (0xFFFF-11),
    KEY_INSERT           = (0xFFFF-12),
    KEY_DELETE           = (0xFFFF-13),
    KEY_HOME             = (0xFFFF-14),
    KEY_END              = (0xFFFF-15),
    KEY_PGUP             = (0xFFFF-16),
    KEY_PGDN             = (0xFFFF-17),
    KEY_ARROW_UP         = (0xFFFF-18),
    KEY_ARROW_DOWN       = (0xFFFF-19),
    KEY_ARROW_RIGHT      = (0xFFFF-20),
    KEY_ARROW_LEFT       = (0xFFFF-21),
    KEY_MOUSE_LEFT       = (0xFFFF-22),
    KEY_MOUSE_RIGHT      = (0xFFFF-23),
    KEY_MOUSE_MIDDLE     = (0xFFFF-24),
    KEY_MOUSE_RELEASE    = (0xFFFF-25),
    KEY_MOUSE_WHEEL_UP   = (0xFFFF-26),
    KEY_MOUSE_WHEEL_DOWN = (0xFFFF-27),

    /* ASCII Control Characters */
    KEY_CTRL_TILDE       = 0x00,
    KEY_CTRL_2           = 0x00,
    KEY_CTRL_A           = 0x01,
    KEY_CTRL_B           = 0x02,
    KEY_CTRL_C           = 0x03,
    KEY_CTRL_D           = 0x04,
    KEY_CTRL_E           = 0x05,
    KEY_CTRL_F           = 0x06,
    KEY_CTRL_G           = 0x07,
    KEY_BACKSPACE        = 0x08,
    KEY_CTRL_H           = 0x08,
    KEY_TAB              = 0x09,
    KEY_CTRL_I           = 0x09,
    KEY_CTRL_J           = 0x0A,
    KEY_CTRL_K           = 0x0B,
    KEY_CTRL_L           = 0x0C,
    KEY_ENTER            = 0x0D,
    KEY_CTRL_M           = 0x0D,
    KEY_CTRL_N           = 0x0E,
    KEY_CTRL_O           = 0x0F,
    KEY_CTRL_P           = 0x10,
    KEY_CTRL_Q           = 0x11,
    KEY_CTRL_R           = 0x12,
    KEY_CTRL_S           = 0x13,
    KEY_CTRL_T           = 0x14,
    KEY_CTRL_U           = 0x15,
    KEY_CTRL_V           = 0x16,
    KEY_CTRL_W           = 0x17,
    KEY_CTRL_X           = 0x18,
    KEY_CTRL_Y           = 0x19,
    KEY_CTRL_Z           = 0x1A,
    KEY_ESC              = 0x1B,
    KEY_CTRL_LSQ_BRACKET = 0x1B,
    KEY_CTRL_3           = 0x1B,
    KEY_CTRL_4           = 0x1C,
    KEY_CTRL_BACKSLASH   = 0x1C,
    KEY_CTRL_5           = 0x1D,
    KEY_CTRL_RSQ_BRACKET = 0x1D,
    KEY_CTRL_6           = 0x1E,
    KEY_CTRL_7           = 0x1F,
    KEY_CTRL_SLASH       = 0x1F,
    KEY_CTRL_UNDERSCORE  = 0x1F,
    KEY_SPACE            = 0x20,
    KEY_BACKSPACE2       = 0x7F,
    KEY_CTRL_8           = 0x7F,
};

static int readkey(void) {
    int nread;
    char c, seq[3];
    while ((nread = read(STDIN_FILENO,&c,1)) == 0);
    if (nread < 0) return EOF;
    if (c != KEY_ESC) return c;
    /* Otherwise parse the escape sequence */
    while (true) {
        /* If this is just an ESC, we'll timeout here. */
        if (read(STDIN_FILENO, seq,   1) == 0) return KEY_ESC;
        if (read(STDIN_FILENO, seq+1, 1) == 0) return KEY_ESC;
        /* Parse the escape sequence */
        if ('[' == seq[0]) {
            if ((seq[1] >= '0') && (seq[1] <= '9')) {
                /* Extended escape, read additional byte. */
                if (read(STDIN_FILENO, seq+2, 1) == 0) return KEY_ESC;
                if (seq[2] == '~') {
                    switch(seq[1]) {
                        case '3': return KEY_DELETE;
                        case '5': return KEY_PGUP;
                        case '6': return KEY_PGDN;
                    }
                }
            } else {
                switch(seq[1]) {
                    case 'A': return KEY_ARROW_UP;
                    case 'B': return KEY_ARROW_DOWN;
                    case 'C': return KEY_ARROW_RIGHT;
                    case 'D': return KEY_ARROW_LEFT;
                    case 'H': return KEY_HOME;
                    case 'F': return KEY_END;
                }
            }
        } else if ('O' == seq[0]) {
            switch(seq[1]) {
                case 'H': return KEY_HOME;
                case 'F': return KEY_END;
            }
        }
    }
    /* should never get here */
    return EOF;
}

/* Screen Buffer Handling
 *****************************************************************************/
static struct {
    size_t capacity;
    size_t length;
    char* buffer;
} ScreenBuf;

static void scrsize(int* rows, int* cols) {
    /* save cursor pos, try to move to lower right, read pos, restore pos */
    ttysendrecv("\033[s\033[999;999H\033[6n\033[u", "\033[%d;%dR", rows, cols);
}

static void scrdraw(void) {
    write(STDOUT_FILENO, ScreenBuf.buffer, ScreenBuf.length);
    ScreenBuf.length = 0;
}

static void scrputc(char ch) {
    /* Make sure there's space for the new char */
    if (ScreenBuf.buffer == NULL) {
        ScreenBuf.capacity = 8u;
        ScreenBuf.buffer   = (char*)calloc(ScreenBuf.capacity, 1u);
    } else if (ScreenBuf.length+1u >= ScreenBuf.capacity) {
        ScreenBuf.capacity = ScreenBuf.capacity << 1u;
        ScreenBuf.buffer = (char*)realloc(ScreenBuf.buffer, ScreenBuf.capacity);
    }
    /* Append the char */
    ScreenBuf.buffer[ScreenBuf.length++] = ch;
    ScreenBuf.buffer[ScreenBuf.length]   = '\0';
}

static int scrputs(char* str) {
    int nwrite = 0;
    while (*str) {
        scrputc(*(str++));
        nwrite++;
    }
    return nwrite;
}

static int scrprintf(const char* fmt, ...) {
    va_list args;
    va_start(args, fmt);
    size_t strsz = vsnprintf(NULL, 0, fmt, args);
    va_end(args);
    char* str = malloc(strsz+1);
    va_start(args, fmt);
    int nwrite = vsnprintf(str, strsz+1, fmt, args);
    va_end(args);
    scrputs(str);
    free(str);
    return nwrite;
}

static void scrclrup(void) {
    scrputs("\033[0J");
}

static void scrclrdn(void) {
    scrputs("\033[1J");
}

static void scrclr(void) {
    scrputs("\033[2J");
}

static void scrclrlnl(void) {
    scrputs("\033[0K");
}

static void scrclrlnr(void) {
    scrputs("\033[1K");
}

static void scrclrln(void) {
    scrputs("\033[2K");
}

/* Cursor Handling
 *****************************************************************************/
static void csrhome(void) {
    scrputs("\033[H");
}

static void csrmove(int l, int c) {
    scrprintf("\033[%d;%dH", l, c);
}

static void csrup(int n) {
    scrprintf("\033[%dA", n);
}

static void csrdn(int n) {
    scrprintf("\033[%dB", n);
}

static void csrleft(int n) {
    scrprintf("\033[%dC", n);
}

static void csrright(int n) {
    scrprintf("\033[%dD", n);
}

static void csrsave(void) {
    scrputs("\033[s");
}

static void csrrestore(void) {
    scrputs("\033[u");
}

static void csrhide(void) {
    scrputs("\033[?25l");
}

static void csrshow(void) {
    scrputs("\033[?25h");
}

/* Attribute Handling
 *****************************************************************************/
enum {
    NORMAL   = 0,
    BOLD     = 1,
    UNDRLINE = 4,
    BLINK    = 5,
    REVERSE  = 7,
    CONCEAL  = 8,
};

enum {
    BLACK   = 0,
    RED     = 1,
    GREEN   = 2,
    YELLOW  = 3,
    BLUE    = 4,
    MAGENTA = 5,
    CYAN    = 6,
    WHITE   = 7,
};

static void attrset(int a, int fg, int bg) {
    scrprintf("\033[%d;%d;%dm",a, 30+fg, 40+bg);
}

static void attrclr(void) {
    scrputs("\033[m");
}

static void attrsetstyle(int a) {
    scrprintf("\033[%dm", a);
}

static void attrsetfg(int fg) {
    scrprintf("\033[%dm", fg + 30);
}

static void attrsetbg(int bg) {
    scrprintf("\033[%dm", bg + 40);
}
