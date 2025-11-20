CXX ?= g++

CXXFLAGS := -Wall -std=c++17 -O2

INCLUDES := -I .

SRCS := easychat.cpp json11.cpp util.cpp mode/wordinfo.cpp

HEADERS := global.h json11.h mode.h util.h

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
