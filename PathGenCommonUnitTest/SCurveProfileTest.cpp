#include <gtest/gtest.h>
#include <SCurveProfile.h>

TEST(SCurveProfile, SimpleToMaxVel)
{
	//
	// This profile does have time for the robot to reach maximum velocity
	//
	double time = 3.1;
	double distance = 240.0;
	double maxvel = 120.0;

	// Max Jerk, Min Jerk, Max Accel, Min Accel, VelMax, VelMin
	xero::paths::SCurveProfile profile(1200.0, -1200.0, 120.0, -120.0, maxvel, 10.0);

	// Distance, Start Velocity, End Velocity
	profile.update(distance, 0.0, 0.0);

	EXPECT_NEAR(distance, profile.getTotalDistance(), 1e-8);

	EXPECT_NEAR(time, profile.getTotalTime(), 1e-8);

	EXPECT_NEAR(maxvel, profile.getVelocity(time/2), 1e-8);

	EXPECT_NEAR(distance, profile.getDistance(time), 1e-8);

	EXPECT_NEAR(0.0, profile.getVelocity(time), 1e-8);

	EXPECT_NEAR(0.0, profile.getAccel(time), 1e-8);
}

TEST(SCurveProfile, SimpleLessMaxVel)
{
	//
	// This profile does not have time for the robot to reach maximum velocity
	//
	double time = 1.7404331140350879;
	double distance = 80.0;
	double maxvel = 120.0;

	// Max Jerk, Min Jerk, Max Accel, Min Accel, VelMax, VelMin
	xero::paths::SCurveProfile profile(1200.0, -1200.0, 120.0, -120.0, maxvel, 10.0);

	// Distance, Start Velocity, End Velocity
	profile.update(distance, 0.0, 0.0);

	EXPECT_NEAR(distance, profile.getTotalDistance(), 1e-8);

	EXPECT_NEAR(time, profile.getTotalTime(), 1e-8);

	EXPECT_NEAR(89.0625, profile.getVelocity(time / 2), 1e-8);

	EXPECT_NEAR(distance, profile.getDistance(time), 1e-8);

	EXPECT_NEAR(0.0, profile.getVelocity(time), 1e-8);

	EXPECT_NEAR(0.0, profile.getAccel(time), 1e-8);
}