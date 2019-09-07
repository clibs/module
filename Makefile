CFLAGS += -I deps
CFLAGS += -I .

default: test

example: example/main example/logger

example/logger: example/logger.c
	$(CC) $(CFLAGS) -o $@ $^
	./$@

example/main: example/example.c example/main.c
	$(CC) $(CFLAGS) -o $@ $^
	./$@

clean:
	rm -f example/logger example/main test

test: test.c $(wildcard deps/ok/*.c)
	$(CC) $(CFLAGS) -o $@ $^
	./$@

