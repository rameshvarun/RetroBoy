CC=clang
CFLAGS=-std=gnu99 $(shell pkg-config --cflags luajit sdl2 jansson) -I/usr/local/Cellar/physfs/2.0.3/include
LFLAGS=$(shell pkg-config --libs luajit sdl2 jansson) -L/usr/local/Cellar/physfs/2.0.3/lib/ -lphysfs -pagezero_size 10000 -image_base 100000000

.PHONY: default clean
default: retroboy

OBJS=src/main.o

retroboy: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

src/%.o: src/%.c
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*~" -type f -delete
	rm retroboy

format:
	clang-format -i src/*.c