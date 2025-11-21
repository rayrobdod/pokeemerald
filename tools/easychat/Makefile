CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2

INCLUDES := -I .

SRCS += easychat.cpp
SRCS += json11.cpp
SRCS += util.cpp
SRCS += wordinfo.cpp

HEADERS += global.h
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

easychat$(EXE): $(SRCS) $(HEADERS)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(SRCS) -o $@ $(LDFLAGS)

clean:
	$(RM) easychat easychat.exe
