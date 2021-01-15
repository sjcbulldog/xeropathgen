#include "DistanceView.h"

namespace xero
{
	namespace paths
	{
		DistanceView::DistanceView(const std::vector<Pose2d>& points, double step)
		{
			distances_.push_back(0.0);
			for (size_t i = 1; i < points.size(); i++)
				distances_.push_back(points[i].distance(points[i - 1]) + distances_[i - 1]);

			size_t index = 0;
			for (double d = 0.0; d <= distances_.back(); d += step)
			{
				while (d > distances_[index + 1])
					index++;

				double percent = (d - distances_[index]) / (distances_[index + 1] - distances_[index]);
				Pose2d newpt = points[index].interpolate(points[index + 1], percent);
				points_.push_back(Pose2dWithCurvature(newpt, 0));
			}

			distances_.clear();
			distances_.push_back(0.0);
			for (size_t i = 1; i < points_.size(); i++)
				distances_.push_back(points_[i].distance(points_[i - 1]) + distances_[i - 1]);

			for (int i = 1; i < points_.size() - 1; i++)
				points_[i].setCurvature(Pose2dWithCurvature::curvature(points_[i - 1], points_[i], points_[i + 1]));

		}

		Pose2dWithCurvature DistanceView::operator[](double dist) const
		{
			Pose2d result;
			size_t low = 0;
			size_t high = distances_.size();

			while (high - low > 1)
			{
				size_t mid = (high + low) / 2;
				if (dist > distances_[mid])
					low = mid;
				else
					high = mid;
			}

			if (high == distances_.size())
				result = points_[low];
			else
			{
				double percent = (dist - distances_[low]) / (distances_[high] - distances_[low]);
				result = points_[low].interpolate(points_[high], percent);
			}

			return result;
		}

		Pose2dWithCurvature DistanceView::operator[](size_t index) const {
			return points_[index];
		}
	}
}