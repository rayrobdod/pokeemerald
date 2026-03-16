CXX ?= g++
CC ?= gcc

CXXFLAGS := -Wall -std=c++17 -O2

CCFLAGS += -Wno-int-to-pointer-cast
CCFLAGS += -D__INTELLISENSE__
CCFLAGS += -std=c17
CCFLAGS += -O2
CCFLAGS += -ffunction-sections
CCFLAGS += -fdata-sections

LDFLAGS += -Wl,--gc-sections

INCLUDES := -I .

SRCS += easychat.cpp
SRCS += json11.cpp
SRCS += util.cpp
SRCS += wordinfo.cpp

HEADERS += fatal_error.h
HEADERS += json11.h
HEADERS += util.h
HEADERS += wordinfo.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: easychat$(EXE)
	@:

pokemon.o: ../../src/pokemon.c Makefile
	$(CC) $(CCFLAGS) -I ../../include $< -c -o $@

wordinfo_pokemon.o: wordinfo_pokemon.cpp
	$(CXX) $(CXXFLAGS) -I ../../include $(INCLUDES) $< -c -o $@

easychat$(EXE): $(SRCS) $(HEADERS) pokemon.o wordinfo_pokemon.o
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) pokemon.o wordinfo_pokemon.o -o $@ $(LDFLAGS)

clean:
	$(RM) easychat easychat.exe
