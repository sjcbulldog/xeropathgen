SOURCES = \
	PathVelocitySegment.cpp\
	XeroGenV1.cpp\
	XeroGenV1PathGenerator.cpp

OBJECTS = $(SOURCES:.cpp=.o)

CXXFLAGS = -std=c++17 -I../PathGenCommon

XeroGenV1: $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) ../PathGenCommon/PathGenCommon.a


