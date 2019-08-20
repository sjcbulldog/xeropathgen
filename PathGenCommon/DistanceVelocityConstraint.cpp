#include "DistanceVelocityConstraint.h"

namespace xero {
	namespace paths {
		DistanceVelocityConstraint::DistanceVelocityConstraint(double after, double before, double velocity)
		{
			after_distance_ = after;
			before_distance_ = before;
			velocity_ = velocity;
		}

		DistanceVelocityConstraint::~DistanceVelocityConstraint()
		{
		}

		double DistanceVelocityConstraint::getMaxVelocity(const Pose2dWithTrajectory& state)
		{
			if (state.position() > after_distance_ && state.position() < before_distance_)
				return velocity_;

			return std::numeric_limits<double>::max();
		}

		MinMaxAcceleration DistanceVelocityConstraint::getMinMacAccel(const Pose2dWithTrajectory& state, double velocity)
		{
			(void)velocity;
			(void)state;
			return MinMaxAcceleration();
		}
	}
}
