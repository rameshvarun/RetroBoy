CC=g++
CFLAGS=-std=c++11 -Wall $(shell pkg-config --cflags luajit sdl2 jsoncpp) -I/usr/local/Cellar/physfs/2.0.3/include
LFLAGS=$(shell pkg-config --libs luajit sdl2 jsoncpp) -L/usr/local/Cellar/physfs/2.0.3/lib/ -lphysfs -pagezero_size 10000 -image_base 100000000

.PHONY: default clean
default: retroboy

OBJS=src/main.o src/device.o

retroboy: $(OBJS)
	$(CC) -o $@ $^ $(CFLAGS) $(LFLAGS)

src/%.o: src/%.cpp
	$(CC) -c -o $@ $< $(CFLAGS)

clean:
	find . -name "*.o" -type f -delete
	find . -name "*~" -type f -delete
	rm retroboy

format:
	find src -name "*.cpp" -exec clang-format -i {} \;
	find src -name "*.hpp" -exec clang-format -i {} \;
