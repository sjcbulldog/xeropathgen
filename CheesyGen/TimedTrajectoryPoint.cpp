#include "TimedTrajectoryPoint.h"
#include <cassert>

namespace xero
{
	namespace paths
	{
		TimedTrajectoryPoint::TimedTrajectoryPoint(const TrajectorySamplePoint& pt, double t, double p, double v, double a, double j) : TrajectorySamplePoint(pt)
		{
			time_ = t;
			position_ = p;
			velocity_ = v;
			acceleration_ = a;
			jerk_ = j;
		}

		TimedTrajectoryPoint::~TimedTrajectoryPoint()
		{
		}

		TimedTrajectoryPoint TimedTrajectoryPoint::interpolateByTime(const TimedTrajectoryPoint& other, double time) const
		{
			double percent;
			
			if (other.getTime() - getTime() == 0.0)
				percent = 0.0;
			else
				percent = (time - getTime()) / (other.getTime() - getTime());
			assert(percent >= 0.0 && percent <= 1.0);

			Pose2dWithCurvature p2d = interpolate(other, percent);
			TrajectorySamplePoint spt(p2d, 0, 0);

			double p = getPosition() + (other.getPosition() - getPosition()) * percent;
			double v = getVelocity() + (other.getVelocity() - getVelocity()) * percent;
			double a = getAcceleration() + (other.getAcceleration() - getAcceleration()) * percent;
			double j = getJerk() + (other.getJerk() - getJerk()) * percent;
			TimedTrajectoryPoint result(spt, time, p, v, a, j);
			return result;
		}
	}
}
