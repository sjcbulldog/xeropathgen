UNAME_S := $(shell uname -s)

CXXFLAGS = -std=c++17 -I../PathGenCommon

ifeq ($(UNAME_S),"Darwin")
CXXFLAGS += -mmacosx-version-min=10.12
endif

ifeq ($(CONFIG),)
$(error CONFIG not set, must be Debug or Release)
endif

SOURCES = $(wildcard *.cpp)
OBJECTS = $(addprefix $(CONFIG)/,$(SOURCES:.cpp=.o))
APPNAME = PoofsGenerator

$(CONFIG)/$(APPNAME): $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) ../PathGenCommon/$(CONFIG)/PathGenCommon.a

$(CONFIG)/%.o: %.cpp
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) -c -o $@ $(CXXFLAGS) $<

clean:
	rm -rf $(OBJECTS)
