CC     = c99
CFLAGS = 
INCS   = -Isource/
SRCS   = $(wildcard tests/*.c)
OBJS   = $(SRCS:.c=.o)

runtests: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $^
	./$@

tests/%.o: tests/%.c
	$(CC) $(CFLAGS) $(INCS) -c -o $@ $^

clean:
	$(RM) runtests $(OBJS)
