#include "TrajectorySamplePoint.h"
#include "TrajectoryPoint.h"

namespace xero
{
	namespace paths
	{
		TrajectorySamplePoint::TrajectorySamplePoint()
		{
			index_floor_ = std::numeric_limits<size_t>::min();
			index_ceil_ = std::numeric_limits<size_t>::max();
		}

		TrajectorySamplePoint::TrajectorySamplePoint(const TrajectoryPoint &p) : Pose2dWithCurvature(p)
		{
			index_floor_ = p.getIndex();
			index_ceil_ = p.getIndex();
		}

		TrajectorySamplePoint::TrajectorySamplePoint(const Pose2dWithCurvature& p, size_t vfloor, size_t vceil) : Pose2dWithCurvature(p)
		{
			index_floor_ = vfloor;
			index_ceil_ = vceil;
		}

		TrajectorySamplePoint::~TrajectorySamplePoint()
		{
		}
	}
}
