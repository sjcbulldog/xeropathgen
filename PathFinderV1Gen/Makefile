
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

OBJECTS = $(CXXSOURCES:.cpp=.o) $(CSOURCES:.c=.o)

CXXFLAGS = -I../PathGenCommon -Iinclude -std=c++17
CFLAGS = -Iinclude

PathFinderV1Gen: $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) ../PathGenCommon/PathGenCommon.a

