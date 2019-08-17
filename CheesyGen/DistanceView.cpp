#include "DistanceView.h"
#include <cassert>

namespace xero {
	namespace paths {

		DistanceView::DistanceView(const Trajectory &traj) : trajectory_(traj)
		{
			distances_.push_back(0.0);
			for (size_t i = 1; i < traj.size(); i++)
			{
				const TrajectoryPoint& prev = traj[i - 1];
				const TrajectoryPoint& current = traj[i];
				double dist = distances_[i - 1] + current.distance(prev);
				distances_.push_back(dist);
			}
		}

		DistanceView::~DistanceView()
		{
		}

		TrajectorySamplePoint DistanceView::sample(double distance)
		{
			TrajectorySamplePoint ret;

			if (distance >= distances_[distances_.size() - 1])
				ret = trajectory_[trajectory_.size() - 1];
			else if (distance <= 0.0)
				ret = trajectory_[0];
			else
			{
				for (size_t i = 1; i < trajectory_.size(); i++)
				{
					const TrajectoryPoint& s = trajectory_[i];
					if (distances_[i] > distance)
					{
						const TrajectoryPoint& prev = trajectory_[i - 1];
						if (MathUtils::epsilonEqual(distances_[i], distances_[i - 1]))
							return s;

						double percent = (distance - distances_[i - 1]) / (distances_[i] - distances_[i - 1]);
						ret = TrajectorySamplePoint(prev.interpolate(s, percent), i - 1, i);
						break;
					}
				}
			}

			return ret;
		}
	}
}
