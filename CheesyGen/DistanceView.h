#pragma once

#include "Trajectory.h"
#include "TrajectorySamplePoint.h"
#include <vector>

namespace xero {
	namespace paths
	{
		class DistanceView
		{
		public:
			DistanceView(const Trajectory &traj);
			virtual ~DistanceView();

			double length() const {
				return distances_[distances_.size() - 1];
			}

			TrajectorySamplePoint sample(double distance);

		public:
			const Trajectory& trajectory_;
			std::vector<double> distances_;
		};
	}
}

