.PHONY: all clean install uninstall

CFLAGS += -g -O2 -msse4.2 -Wall -Wextra -Wpedantic -std=c17 -Wshadow

ifeq ($(CC), c)
	CC := cc
endif

all: build build/dither

build:
	mkdir build

clean:
	rm -rf build/*

build/dither.o: build/main.o
	cp $< $@

build/dither: build/pngio.o

# Implicit rules
build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^
