#pragma once

#include "PathConstraint.h"
#include "UnitConverter.h"
#include "Pose2dWithTrajectory.h"

namespace xero {
	namespace paths {

		class DistanceVelocityConstraint : public PathConstraint
		{
		public:
			DistanceVelocityConstraint(double after, double before, double velocity);
			virtual ~DistanceVelocityConstraint();

			virtual double getMaxVelocity(const Pose2dWithTrajectory& state) ;
			virtual MinMaxAcceleration getMinMacAccel(const Pose2dWithTrajectory& state, double velocity) ;

			virtual void convert(const std::string& from, const std::string& to) {
				after_distance_ = UnitConverter::convert(after_distance_, from, to);
				before_distance_ = UnitConverter::convert(before_distance_, from, to);
				velocity_ = UnitConverter::convert(velocity_, from, to);
			}

			double getBefore() const {
				return before_distance_;
			}

			void setBefore(double d) {
				before_distance_ = d;
			}

			double getAfter() const {
				return after_distance_;
			}

			void setAfter(double d) {
				after_distance_ = d;
			}

			double getVelocity() const {
				return velocity_;
			}

			void setVelocity(double d) {
				velocity_ = d;
			}

		private:
			double after_distance_;
			double before_distance_;
			double velocity_;
		};
	}
}

