CC   = c99
INCS = -Isource/
DEFS =
SRCS = $(wildcard tests/*.c)
OBJS = $(SRCS:.c=.o)

runtests: $(OBJS)
	$(CC) -o $@ $^
	./$@

tests/%.o: tests/%.c source/%.h
	$(CC) $(INCS) $(DEFS) -c -o $@ $<

tests/%.o: tests/%.c
	$(CC) $(INCS) $(DEFS) -c -o $@ $<

clean:
	$(RM) runtests $(OBJS)
