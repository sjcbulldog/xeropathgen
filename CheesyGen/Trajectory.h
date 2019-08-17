#pragma once

#include "TrajectoryPoint.h"
#include "ICsv.h"
#include <vector>

namespace xero
{
	namespace paths
	{
		class Trajectory
		{
		public:
			Trajectory(std::vector<Pose2dWithCurvature>& points)
			{
				for (size_t i = 0; i < points.size(); i++)
				{
					TrajectoryPoint pt(points[i], i);
					points_.push_back(pt);
				}
			}

			virtual ~Trajectory()
			{
			}

			size_t size() const
			{
				return points_.size();
			}

			const TrajectoryPoint& operator[](size_t index) const
			{
				return points_[index];
			}

			const std::vector<TrajectoryPoint>& getPoints() const {
				return points_;
			}

		private:
			std::vector<TrajectoryPoint> points_;
		};
	}
}


