CXXFLAGS = -std=c++17 -mmacosx-version-min=10.12 -I../PathGenCommon

ifeq ($(CONFIG),)
$(error CONFIG not set, must be Debug or Release)
endif


SOURCES = \
	PathVelocitySegment.cpp\
	XeroGenV1.cpp\
	XeroGenV1PathGenerator.cpp

OBJECTS = $(addprefix $(CONFIG)/,$(SOURCES:.cpp=.o))

$(CONFIG)/XeroGenV1: $(OBJECTS)
	g++ $(CXXFLAGS) -o $@ $(OBJECTS) ../PathGenCommon/PathGenCommon.a

$(CONFIG)/%.o: %.cpp
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) -c -o $@ $(CXXFLAGS) $<
