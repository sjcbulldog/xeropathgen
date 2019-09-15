#include <gtest/gtest.h>
#include <Translation2d.h>

namespace xero
{
	namespace paths
	{
		namespace test
		{
			TEST(Translation2d, Constructors)
			{
				Translation2d p1;
				EXPECT_DOUBLE_EQ(0.0, p1.getX());
				EXPECT_DOUBLE_EQ(0.0, p1.getY());

				Translation2d p2(5.0, -6.0);
				EXPECT_DOUBLE_EQ(5.0, p2.getX());
				EXPECT_DOUBLE_EQ(-6.0, p2.getY());

				Translation2d p3(p2);
				EXPECT_DOUBLE_EQ(5.0, p3.getX());
				EXPECT_DOUBLE_EQ(-6.0, p3.getY());

				p1 = Translation2d(10.0, 5.0);
				p2 = Translation2d(15.0, 11.0);
				Translation2d p4(p1, p2);
				EXPECT_DOUBLE_EQ(5.0, p4.getX());
				EXPECT_DOUBLE_EQ(6.0, p4.getY());

				Translation2d p5(Rotation2d::fromDegrees(0.0), 10.0);
				EXPECT_NEAR(10.0, p5.getX(), 1e-8);
				EXPECT_NEAR(0.0, p5.getY(), 1e-8);

				Translation2d p6(Rotation2d::fromDegrees(90.0), 40.0);
				EXPECT_NEAR(0.0, p6.getX(), 1e-8);
				EXPECT_NEAR(40.0, p6.getY(), 1e-8);

				Translation2d p7(Rotation2d::fromDegrees(45.0), 20.0);
				EXPECT_NEAR(14.142135623709505, p7.getX(), 1e-8);
				EXPECT_NEAR(14.142135623709505, p7.getY(), 1e-8);
			}

			TEST(Translation2d, Dot)
			{
				Translation2d p1(1.0, 2.0);
				Translation2d p2(3.0, 4.0);

				EXPECT_NEAR(11.0, Translation2d::dot(p1, p2), 1e-8);
			}

			TEST(Translation2d, Cross)
			{
				Translation2d p1(1.0, 2.0);
				Translation2d p2(3.0, 4.0);

				EXPECT_NEAR(-2.0, Translation2d::cross(p1, p2), 1e-8);
			}

			TEST(Translation2d, Distance)
			{
				Translation2d p1(1.0, 2.0);
				Translation2d p2(1.0, 4.0);
				EXPECT_NEAR(2.0, p1.distance(p2), 1e-8);

				p1 = Translation2d(1.0, 4.0);
				p2 = Translation2d(1.0, 2.0);
				EXPECT_NEAR(2.0, p1.distance(p2), 1e-8);

				p1 = Translation2d(4.0, -2.0);
				p2 = Translation2d(2.0, -2.0);
				EXPECT_NEAR(2.0, p1.distance(p2), 1e-8);

				p1 = Translation2d(2.0, 2.0);
				p2 = Translation2d(4.0, 2.0);
				EXPECT_NEAR(2.0, p1.distance(p2), 1e-8);

				p1 = Translation2d(2.0, 6.0);
				p2 = Translation2d(5.0, 10.0);
				EXPECT_NEAR(5.0, p1.distance(p2), 1e-8);
			}

			TEST(Translation2d, EpsilonEqual)
			{
				Translation2d p1(1.0, 2.0);
				Translation2d p2(1.0, 2.0);
				EXPECT_EQ(true, p1.epsilonEqual(p2));

				p1 = Translation2d(2.0, 3.0);
				p2 = Translation2d(2.0, 3.1);
			}

			TEST(Translation2d, ToRotation)
			{
			}

			TEST(Translation2d, Normalize)
			{
			}
		}
	}
}