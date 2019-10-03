#include <gtest/gtest.h>
#include <TrapezoidalProfile.h>

using namespace xero::paths;

TEST(TrapezoidProfileTests, PositiveBasicVTest)
{
	TrapezoidalProfile profile(2, -4, 100);

	profile.update(96.0, 0.0, 0.0);
	EXPECT_DOUBLE_EQ(8.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(4.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(16.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(16.0, profile.getDistance(4.0));
	EXPECT_DOUBLE_EQ(8.0, profile.getVelocity(4.0));
	EXPECT_DOUBLE_EQ(88.0, profile.getDistance(10.0));
	EXPECT_DOUBLE_EQ(8.0, profile.getVelocity(10.0));
}

TEST(TrapezoidProfileTests, NegativeBasicVTest)
{
	TrapezoidalProfile profile(2, -4, 100);

	profile.update(-96.0, 0.0, 0.0);
	EXPECT_DOUBLE_EQ(8.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(4.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(-16.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(-16.0, profile.getDistance(4.0));
	EXPECT_DOUBLE_EQ(-8.0, profile.getVelocity(4.0));
	EXPECT_DOUBLE_EQ(-88.0, profile.getDistance(10.0));
	EXPECT_DOUBLE_EQ(-8.0, profile.getVelocity(10.0));
}

TEST(TrapezoidProfileTests, PositiveNonZeroStartEndVTest)
{
	TrapezoidalProfile profile(2, -6, 100);

	profile.update(309.0, 12.0, 22.0);
	EXPECT_DOUBLE_EQ(11.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(2.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(34.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(189.0, profile.getDistance(9.0));
	EXPECT_DOUBLE_EQ(30.0, profile.getVelocity(9.0));
	EXPECT_DOUBLE_EQ(284.0, profile.getDistance(12.0));
	EXPECT_DOUBLE_EQ(28.0, profile.getVelocity(12.0));
}

TEST(TrapezoidProfileTests, NegativeNonZeroStartEndVTest)
{
	TrapezoidalProfile profile(2, -6, 100);

	profile.update(-309.0, -12.0, -22.0);
	EXPECT_DOUBLE_EQ(11.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(2.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(-34.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(-189.0, profile.getDistance(9.0));
	EXPECT_DOUBLE_EQ(-30.0, profile.getVelocity(9.0));
	EXPECT_DOUBLE_EQ(-284.0, profile.getDistance(12.0));
	EXPECT_DOUBLE_EQ(-28.0, profile.getVelocity(12.0));
}

TEST(TrapezoidProfileTests, DecelOnly)
{
	TrapezoidalProfile profile(2, -4, 40);

	profile.update(200, 40.0, 0.0);
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(10.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(40.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(128.0, profile.getDistance(4.0));
	EXPECT_DOUBLE_EQ(24.0, profile.getVelocity(4.0));
	EXPECT_DOUBLE_EQ(182.0, profile.getDistance(7.0));
	EXPECT_DOUBLE_EQ(12.0, profile.getVelocity(7.0));

}

TEST(TrapezoidProfileTests, CruiseDecel)
{
	TrapezoidalProfile profile(2, -8, 40);

	profile.update(500, 40.0, 0.0);
	EXPECT_DOUBLE_EQ(0.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(10.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(5.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(40.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(200.0, profile.getDistance(5.0));
	EXPECT_DOUBLE_EQ(40.0, profile.getVelocity(5.0));
	EXPECT_DOUBLE_EQ(484.0, profile.getDistance(13.0));
	EXPECT_DOUBLE_EQ(16.0, profile.getVelocity(13.0));
}

TEST(TrapezoidProfileTests, Trapezoid)
{
	TrapezoidalProfile profile(2, -4, 20);

	profile.update(550, 0.0, 0.0);
	EXPECT_DOUBLE_EQ(10.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(20.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(5.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(20.0, profile.getActualMaxVelocity());
	EXPECT_DOUBLE_EQ(25.0, profile.getDistance(5.0));
	EXPECT_DOUBLE_EQ(10.0, profile.getVelocity(5.0));
	EXPECT_DOUBLE_EQ(300.0, profile.getDistance(20.0));
	EXPECT_DOUBLE_EQ(20.0, profile.getVelocity(20.0));
	EXPECT_DOUBLE_EQ(542.0, profile.getDistance(33.0));
	EXPECT_DOUBLE_EQ(8.0, profile.getVelocity(33.0));
}

TEST(TrapezoidProfileTests, ComplexTrapezoid)
{
	TrapezoidalProfile profile(9, -13, 95);

	profile.update(2564, 5.0, 69.0);
	EXPECT_DOUBLE_EQ(10.0, profile.getTimeAccel());
	EXPECT_DOUBLE_EQ(20.0, profile.getTimeCruise());
	EXPECT_DOUBLE_EQ(2.0, profile.getTimeDecel());
	EXPECT_DOUBLE_EQ(95.0, profile.getActualMaxVelocity());
}

TEST(TrapezoidProfileTests, Impossible)
{
	TrapezoidalProfile profile(100, -100, 100);
	EXPECT_THROW(profile.update(1.0, 100.0, 0.0), std::runtime_error);
}