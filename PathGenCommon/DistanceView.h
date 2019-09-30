#pragma once

#include "Pose2d.h"
#include <vector>

namespace xero
{
	namespace paths
	{
		class DistanceView
		{
		public:
			DistanceView(const std::vector<Pose2d>& points, double delta);
			double length() const {
				return distances_.back();
			}

			double getPosition(size_t index) const {
				return distances_[index];
			}

			Pose2d operator[](double dist) const;
			Pose2d operator[](size_t index) const;
			size_t size() const {
				return points_.size();
			}

		private:
			std::vector<double> distances_;
			std::vector<Pose2d> points_;
		};
	}
}

