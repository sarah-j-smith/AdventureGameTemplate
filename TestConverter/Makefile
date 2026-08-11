# https://oneuptime.com/blog/post/2026-07-28-gnu-make-c-cpp-header-dependencies/view

PROJECT_NAME = TestConverter

# BUILD_MODE = RELEASE
BUILD_MODE := DEBUG

# Default to MacOSX, ymmv
PLATFORM_OS = OSX
# PLATFORM_OS = WINDOWS
# PLATFORM_OS = LINUX

# Define default make program: Mingw32-make
MAKE = mingw32-make

# Define default C compiler: gcc - on Windows use mingw
# NOTE: define g++ compiler if using C++
CXX = g++

CPPFLAGS := -Iinclude

CXXFLAGS = -Wall -Wextra -std=c++20 -stdlib=libc++

ifeq ($(BUILD_MODE),DEBUG)
	CXXFLAGS += -g -O0
else
	CXXFLAGS += -s -O1
endif

ifeq ($(PLATFORM_OS),LINUX)
	MAKE = make
	INCLUDE_PATHS += -isystem. 
endif

ifeq ($(PLATFORM_OS),OSX)
	MAKE = make
	CXX = clang++
endif

INCLUDE_PATHS = -I.

LDFLAGS :=
LDLIBS :=


SOURCES := $(wildcard src/*.cpp)
OBJECTS := $(patsubst src/%.cpp,build/%.o,$(SOURCES))
DEPFILES := $(OBJECTS:.o=.d)

.PHONY: all clean

all: build/$(PROJECT_NAME)

build/$(PROJECT_NAME): $(OBJECTS)
	$(CXX) $(LDFLAGS) $^ $(LDLIBS) -o $@

build/%.o: src/%.cpp
	@mkdir -p $(@D)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) \
	  -MMD -MP -MF $(@:.o=.d) -MT $@ \
	  -c $< -o $@

-include $(DEPFILES)

clean:
	rm -rf build 