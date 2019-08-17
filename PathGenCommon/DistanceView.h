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

			Pose2d operator[](double dist) const;

		private:
			std::vector<double> distances_;
			std::vector<Pose2d> points_;
		};
	}
}

