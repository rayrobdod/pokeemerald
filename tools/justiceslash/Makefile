CXX ?= g++

CXXFLAGS = -std=c++17 -Wall -Wextra -Werror -Wno-unused -O2

LIBS =
LDFLAGS =

SRCS = main.cpp AlgebraicFunc.cpp Curve.cpp Vector.cpp
HEADERS = AlgebraicFunc.h Curve.h PixelMap.h Vector.h

ifeq ($(OS),Windows_NT)
EXE := .exe
else
EXE :=
endif

.PHONY: all clean

all: justiceslash$(EXE)
	@:

justiceslash$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(SRCS) -o $@ $(LDFLAGS) $(LIBS)

clean:
	$(RM) justiceslash justiceslash.exe
