#pragma once

#include "TrajectorySamplePoint.h"

namespace xero 
{
	namespace paths
	{
		class ConstrainedPoint
		{
		public:
			ConstrainedPoint(const TrajectorySamplePoint* pt);
			ConstrainedPoint(const TrajectorySamplePoint* pt, double dist, double maxvel, double minaccel, double maxaccel);
			virtual ~ConstrainedPoint();

			const TrajectorySamplePoint* getState() const {
				return traj_point_;
			}

			double getDistance() const {
				return distance_;
			}

			void setDistance(double d) {
				distance_ = d;
			}

			double getMaxVelocity() const {
				return max_velocity_;
			}

			void setMaxVelocity(double v) {
				max_velocity_ = v;
			}

			double getMaxAccel() const {
				return max_accel_;
			}

			void setMaxAccel(double v) {
				max_accel_ = v;
			}

			double getMinAccel() const {
				return min_accel_;
			}

			void setMinAccel(double v) {
				min_accel_ = v;
			}

		private:
			const TrajectorySamplePoint* traj_point_;
			double distance_;
			double max_velocity_;
			double max_accel_;
			double min_accel_;
		};
	}
}

