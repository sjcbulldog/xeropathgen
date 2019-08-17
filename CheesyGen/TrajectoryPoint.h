#pragma once

#include "Pose2dWithCurvature.h"
#include "ICsv.h"

namespace xero {
	namespace paths {
		class TrajectoryPoint : public Pose2dWithCurvature
		{
		public:
			TrajectoryPoint(const Pose2dWithCurvature& pt, size_t index) : Pose2dWithCurvature(pt)
			{
				index_ = index;
			}

			virtual ~TrajectoryPoint()
			{
			}

			size_t getIndex() const {
				return index_;
			}

		private:
			size_t index_;
		};
	}
}

