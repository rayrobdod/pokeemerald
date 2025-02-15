CC ?= gcc

CFLAGS = -Wall -Wextra -Werror -Wno-sign-compare -std=c11 -O2
CDEBUGFLAGS += -ggdb -DDEBUG

LIBS =

BUILDDIR = build

SRCS = main.c util.c
HEADERS = global.h compress.h util.h
OBJS = \
	$(BUILDDIR)/compress8.o \
	$(BUILDDIR)/compress16.o \
	$(patsubst %.c,$(BUILDDIR)/%.o,$(SRCS)) \

DEBUGOBJS = $(patsubst %.o,%-debug.o,$(OBJS))

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: compressTilemap4$(EXE)
	@:

$(BUILDDIR):
	mkdir -p $@

$(BUILDDIR)/compress8.o : compress.c $(HEADERS) Makefile | $(BUILDDIR)
	$(CC) $(CFLAGS) -DARRAY=ByteArray -DSIZE=8 -DTYPE=char -c -o $@ $<

$(BUILDDIR)/compress16.o : compress.c $(HEADERS) Makefile | $(BUILDDIR)
	$(CC) $(CFLAGS) -DARRAY=ShortArray -DSIZE=16 -DTYPE=short -c -o $@ $<

$(BUILDDIR)/%.o : %.c $(HEADERS) | $(BUILDDIR)
	$(CC) $(CFLAGS) -c -o $@ $<

$(BUILDDIR)/compress8-debug.o : compress.c $(HEADERS) Makefile | $(BUILDDIR)
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) -DARRAY=ByteArray -DSIZE=8 -DTYPE=char -c -o $@ $<

$(BUILDDIR)/compress16-debug.o : compress.c $(HEADERS) Makefile | $(BUILDDIR)
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) -DARRAY=ShortArray -DSIZE=16 -DTYPE=short -c -o $@ $<

$(BUILDDIR)/%-debug.o : %.c $(HEADERS) | $(BUILDDIR)
	$(CC) $(CFLAGS) $(CDEBUGFLAGS) -c -o $@ $<

compressTilemap4-debug$(EXE): $(DEBUGOBJS)
	$(CC) $(DEBUGOBJS) -o $@ $(LDFLAGS) $(LIBS)

compressTilemap4$(EXE): $(OBJS)
	$(CC) $(OBJS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) -r build
	$(RM) compressTilemap4$(EXE) compressTilemap4-debug$(EXE)
