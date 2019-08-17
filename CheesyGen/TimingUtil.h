#pragma once

#include "TimedTrajectory.h"
#include "Trajectory.h"
#include "DistanceView.h"
#include "PathConstraint.h"
#include <list>
#include <memory>

namespace xero {
	namespace paths
	{
		class TimingUtil
		{
		public:
			TimingUtil() = delete;
			~TimingUtil() = delete ;

			static TimedTrajectory timeParameterizeTrajectory(DistanceView& distview, const ConstraintCollection &constraints, double step_size,
									double startvel, double endvel, double maxvel, double maxaccel);

			static TimedTrajectory timeParameterizeTrajectory(std::vector<TrajectorySamplePoint> &states, const ConstraintCollection& constraints,
									double startvel, double endvel, double maxvel, double maxaccel);

			static TimedTrajectory createConstantTimeTrajectory(const TimedTrajectory& traj, double timestep);

		private:
			static double constexpr kEpsilon = 1e-6;
		};
	}
}

