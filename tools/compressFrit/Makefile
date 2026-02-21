CC ?= gcc

CFLAGS = -Wall -Wextra -Werror -Wno-sign-compare -std=c11 -O2

LIBS =
LDFLAGS =

SRCS = $(wildcard *.c)
HDRS = $(wildcard *.h)

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: compressFrit$(EXE)
	@:

compressFrit-debug$(EXE): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -DDEBUG $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

compressFrit$(EXE): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) compressFrit compressFrit.exe
