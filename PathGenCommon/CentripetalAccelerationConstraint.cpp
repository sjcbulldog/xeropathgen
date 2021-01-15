#include "CentripetalAccelerationConstraint.h"
#include <iostream>
#include <cmath>

namespace xero
{
	namespace paths
	{
		CentripetalAccelerationConstraint::CentripetalAccelerationConstraint(double maxcen, double weight, const std::string &lengthunits, const std::string &weightunits)
		{
			maxcen_ = maxcen;
			weight_ = UnitConverter::convert(weight, weightunits, "kg");
			units_ = lengthunits;
		}

		CentripetalAccelerationConstraint::~CentripetalAccelerationConstraint()
		{
		}

		double CentripetalAccelerationConstraint::getMaxVelocity(const Pose2dWithTrajectory& state)
		{
			double radius = 1 / state.curvature();
			double radius_m = UnitConverter::convert(radius, units_, "m");
			double vel = std::sqrt(std::abs(maxcen_ * radius_m / weight_));
			return vel;
		}

		MinMaxAcceleration CentripetalAccelerationConstraint::getMinMacAccel(const Pose2dWithTrajectory& state, double velocity)
		{
			(void)state;
			(void)velocity;

			MinMaxAcceleration acc;
			return acc;
		}
	}
}