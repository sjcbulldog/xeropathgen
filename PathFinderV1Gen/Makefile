UNAME_S := $(shell uname -s)

CXXFLAGS = -std=c++17 -I../PathGenCommon -Iinclude
CFLAGS = -Iinclude

ifeq ($(UNAME_S),"Darwin")
CXXFLAGS += -mmacosx-version-min=10.12
endif

ifeq ($(CONFIG),)
$(error CONFIG not set, must be Debug or Release)
endif

CXXSOURCES = $(wildcard *.cpp)
CSOURCES = $(wildcard src/*.c) $(wildcard src/*/*.c)
OBJECTS = $(addprefix $(CONFIG)/,$(CXXSOURCES:.cpp=.o)) $(addprefix $(CONFIG)/,$(CSOURCES:.c=.o))
APPNAME = PathFinderV1Gen

$(CONFIG)/$(APPNAME): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) ../PathGenCommon/$(CONFIG)/PathGenCommon.a

$(CONFIG)/%.o: %.cpp
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

$(CONFIG)/src/%.o: src/%.c
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CC) -c -o $@ $(CFLAGS) $<

clean:
	rm -rf $(OBJECTS)
