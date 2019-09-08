CXXFLAGS = -std=c++17 -mmacosx-version-min=10.12

ifeq ($(CONFIG),)
$(error CONFIG not set, must be Debug or Release)
endif

SOURCES = \
	DistanceVelocityConstraint.cpp\
	DistanceView.cpp\
	DriveBaseData.cpp\
	JSON.cpp\
	JSONPathReader.cpp\
	JSONValue.cpp\
	MathUtils.cpp\
	PathBase.cpp\
	PathTrajectory.cpp\
	Pose2d.cpp\
	Pose2dWithTrajectory.cpp\
	QuadraticSolver.cpp\
	QuinticHermiteSpline.cpp\
	RobotPath.cpp\
	Rotation2d.cpp\
	SCurveProfile.cpp\
	SplinePair.cpp\
	SwerveDriveModifier.cpp\
	TankDriveModifier.cpp\
	TrajectoryUtils.cpp\
	Translation2d.cpp\
	TrapezoidalProfile.cpp\
	Twist2d.cpp\
	UnitConverter.cpp\
	WaypointReader.cpp

OBJECTS = $(addprefix $(CONFIG)/,$(SOURCES:.cpp=.o))

PathGenCommon.a: $(OBJECTS)
	rm -f $(CONFIG)/PathGenCommon.a
	ar r $(CONFIG)/PathGenCommon.a $(OBJECTS)

$(CONFIG)/%.o: %.cpp
	@echo "    "Compiling $<
	@mkdir -p $(dir $@)
	@$(CXX) -c -o $@ $(CXXFLAGS) $<