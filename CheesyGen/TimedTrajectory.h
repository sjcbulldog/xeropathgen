#pragma once

#include "TimedTrajectoryPoint.h"
#include <vector>

namespace xero
{
	namespace paths
	{
		class TimedTrajectory
		{
		public:
			TimedTrajectory();
			TimedTrajectory(std::vector<TimedTrajectoryPoint>& points);
			virtual ~TimedTrajectory();

			const std::vector<TimedTrajectoryPoint> &getPoints() const {
				return points_;
			}

			void setJerk(size_t index, double v) {
				points_[index].setJerk(v);
			}

		private:
			std::vector<TimedTrajectoryPoint> points_;
		};
	}
}

