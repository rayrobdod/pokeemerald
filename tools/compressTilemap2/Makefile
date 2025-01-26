CC ?= gcc

CFLAGS = -Wall -Wextra -Werror -Wno-sign-compare -std=c11 -O2

LIBS =

SRCS = main.c compress.c util.c
HEADERS = global.h compress.h util.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: compressTilemap2$(EXE)
	@:

compressTilemap2-debug$(EXE): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) -ggdb -DDEBUG $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

compressTilemap2$(EXE): $(SRCS) $(HEADERS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) compressTilemap2$(EXE) compressTilemap2-debug$(EXE)
