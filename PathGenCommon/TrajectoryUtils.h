#pragma once

#include "SplinePair.h"
#include "Pose2d.h"
#include <vector>

namespace xero
{
	namespace paths
	{
		class TrajectoryUtils
		{
		public:
			TrajectoryUtils() = delete;
			~TrajectoryUtils() = delete;

			static std::vector<Pose2d> parameterize(const std::vector<std::shared_ptr<SplinePair>>& splines,
													double maxDx, double maxDy, double maxDTheta);

		private:
			static void getSegmentArc(std::shared_ptr<SplinePair> pair, std::vector<Pose2d>& results,
									  double t0, double t1, double maxDx, double maxDy, double maxDTheta);
		};
	}
}

