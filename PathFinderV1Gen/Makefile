CXXFLAGS = -std=c++17 -mmacosx-version-min=10.12 -I../PathGenCommon -Iinclude
CFLAGS = -mmacosx-version-min=10.12 -Iinclude

ifeq ($(CONFIG),)
$(error CONFIG not set, must be Debug or Release)
endif

CXXSOURCES = \
	PathFinderV1Gen.cpp\
	PathGenerator.cpp

CSOURCES = \
	src/error.c\
	src/generator.c\
	src/io.c\
	src/mathutil.c\
	src/spline.c\
	src/trajectory.c\
	src/fit/hermite.c

OBJECTS = $(addprefix $(CONFIG)/,$(CXXSOURCES:.cpp=.o)) $(addprefix $(CONFIG)/,$(CSOURCES:.c=.o))

$(CONFIG)/PathFinderV1Gen: $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) ../PathGenCommon/PathGenCommon.a


$(CONFIG)/%.o: %.cpp
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) -c -o $@ $(CXXFLAGS) $<


$(CONFIG)/%.o: %.c
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CC) -c -o $@ $(CFLAGS) $<
