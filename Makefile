.PHONY: all clean install uninstall

CFLAGS += -g -O2 -Wall -Wextra -Wpedantic -std=c17 -Wshadow

ifeq ($(CC), c)
	CC := cc
endif

all:

build:
	mkdir build

clean:
	rm -rf build/*

# Implicit rules
build/%.o: src/%.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $@ $^
