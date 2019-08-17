#pragma once

#include "TrajectorySamplePoint.h"

namespace xero {
	namespace paths {
		class TimedTrajectoryPoint : public TrajectorySamplePoint
		{
		public:
			TimedTrajectoryPoint(const TrajectorySamplePoint& pt, double t, double p, double v, double a, double j);
			virtual ~TimedTrajectoryPoint();

			double getTime() const {
				return time_;
			}

			void setTime(double t) {
				time_ = t;
			}

			double getVelocity() const {
				return velocity_;
			}

			void setVelocity(double v) {
				velocity_ = v;
			}

			double getAcceleration() const {
				return acceleration_;
			}

			void setAcceleration(double a) {
				acceleration_ = a;
			}

			double getPosition() const {
				return position_;
			}

			void setPosition(double v) {
				position_ = v;
			}

			double getJerk() const {
				return jerk_;
			}

			void setJerk(double v) {
				jerk_ = v;
			}

			virtual double getField(const std::string &field) const {
				double v;

				if (field == "time")
				{
					v = time_;
				}
				else if (field == "position")
				{
					v = position_;
				}
				else if (field == "velocity")
				{
					v = velocity_;
				}
				else if (field == "acceleration")
				{
					v = acceleration_;
				}
				else if (field == "jerk")
				{
					v = jerk_;
				}
				else
				{
					v = TrajectorySamplePoint::getField(field);
				}

				return v;
			}

			TimedTrajectoryPoint interpolateByTime(const TimedTrajectoryPoint& other, double time) const;

		private:
			double time_;
			double position_;
			double velocity_;
			double acceleration_;
			double jerk_;
		};
	}
}

