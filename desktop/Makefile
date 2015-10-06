CC=gcc
CFLAGS=-std=gnu99
LIBS=

.PHONY: default clean
default: retroboy

OBJS=src/main.o

retroboy: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

src/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*~" -type f -delete
	rm retroboy

format:
	clang-format -i src/*.c src/*.h