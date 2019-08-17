#pragma once

#include "MinMaxAcceleration.h"
#include "Pose2dWithTrajectory.h"
#include <memory>
#include <vector>

namespace xero
{
	namespace paths
	{
		class PathConstraint
		{
		public:
			PathConstraint() {
			}

			virtual ~PathConstraint() {
			}

			virtual double getMaxVelocity(const Pose2dWithTrajectory& state)  = 0;
			virtual MinMaxAcceleration getMinMacAccel(const Pose2dWithTrajectory& state, double velocity) = 0;
			virtual void convert(const std::string& from, const std::string& to) = 0;
		};

		typedef std::vector<std::shared_ptr<PathConstraint>> ConstraintCollection;
	}
}

