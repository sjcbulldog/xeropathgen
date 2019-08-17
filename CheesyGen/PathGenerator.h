#pragma once

#include "Pose2dWithCurvature.h"
#include "Trajectory.h"

#include <vector>

namespace xero
{
	namespace paths
	{

		class PathGenerator
		{
		public:
			PathGenerator(double maxx, double maxy, double maxtheta);
			~PathGenerator();

			Trajectory generate(const std::vector<Pose2d>& waypoints) const ;

		private:
			double maxDx_;
			double maxDy_;
			double maxDTheta_;
		};
	}
}

