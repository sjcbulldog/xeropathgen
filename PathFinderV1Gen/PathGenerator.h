#pragma once

#include "PathTrajectory.h"

namespace xero
{
	namespace paths
	{
		class PathGenerator
		{
		public:
			PathGenerator(int steps, double timestep);
			~PathGenerator();

			PathTrajectory generate(const std::vector<Pose2d>& points, double maxvel, double maxaccel, double maxjerk);

		private:
			int steps_;
			double timestep_;
		};
	}
}

