#include "ConstrainedPoint.h"

namespace xero
{
	namespace paths
	{
		ConstrainedPoint::ConstrainedPoint(const TrajectorySamplePoint* pt)
		{
			traj_point_ = pt;
			distance_ = 0.0;
			max_velocity_ = 0.0;
			max_accel_ = 0.0;
			min_accel_ = 0.0;
		}

		ConstrainedPoint::ConstrainedPoint(const TrajectorySamplePoint* pt, double dist, double maxvel, double minaccel, double maxaccel)
		{
			traj_point_ = pt;
			distance_ = dist;
			max_velocity_ = maxvel;
			min_accel_ = minaccel;
			max_accel_ = maxaccel;
		}

		ConstrainedPoint::~ConstrainedPoint()
		{
		}
	}
}
