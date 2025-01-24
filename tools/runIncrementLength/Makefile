CC ?= gcc

CFLAGS = -Wall -Wextra -Werror -Wno-sign-compare -std=c11 -O2

LIBS =

SRCS = main.c ril.c util.c
HEADERS = global.h options.h ril.h util.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: runIncrementLength$(EXE)
	@:

runIncrementLength-debug$(EXE): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -ggdb -DDEBUG $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

runIncrementLength$(EXE): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) runIncrementLength runIncrementLength.exe
