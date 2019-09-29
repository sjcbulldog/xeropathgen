#pragma once
#include "Pose2dWithTrajectory.h"

namespace xero
{
	namespace paths
	{
		class Pose2dConstrained : public Pose2dWithTrajectory
		{
		public:
			Pose2dConstrained() {
				accel_min_ = std::numeric_limits<double>::min();
				accel_max_ = std::numeric_limits<double>::max();
			}

			virtual ~Pose2dConstrained() {
			}

			double accelMin() const {
				return accel_min_;
			}

			double accelMax() const {
				return accel_max_;
			}

			void setAccelMin(double d) {
				accel_min_ = d;
			}

			void setAccelMax(double d) {
				accel_max_ = d;
			}

		private:
			double accel_min_;
			double accel_max_;
		};
	}
}

