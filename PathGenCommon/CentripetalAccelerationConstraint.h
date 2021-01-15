#pragma once

#include "PathConstraint.h"
#include "UnitConverter.h"

namespace xero {
	namespace paths {

		class CentripetalAccelerationConstraint : public PathConstraint
		{
		public:
			// maxcen is in Newtons
			// weight is in pounds
			CentripetalAccelerationConstraint(double maxcen, double weight, const std::string &units);
			virtual ~CentripetalAccelerationConstraint();

			virtual double getMaxVelocity(const Pose2dWithTrajectory& state) ;
			virtual MinMaxAcceleration getMinMacAccel(const Pose2dWithTrajectory& state, double velocity) ;
			virtual void convert(const std::string& from, const std::string& to) {
				maxcen_ = UnitConverter::convert(maxcen_, from, to);
			}

		private:
			double weight_;
			double maxcen_;
			std::string units_;
		};
	}
}


