CXXFLAGS = -std=c++17 -mmacosx-version-min=10.12

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

OBJECTS = $(SOURCES:.cpp=.o)

PathGenCommon.a: $(OBJECTS)
	rm -f PathGenCommon.a
	ar r PathGenCommon.a $(OBJECTS)

