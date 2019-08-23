#include "PathTrajectory.h"
#include <cmath>

namespace xero
{
	namespace paths
	{
		size_t PathTrajectory::getIndex(double time)
		{
			if (size() == 0)
				return std::numeric_limits<size_t>::max();

			if (time < points_.front().time())
				return std::numeric_limits<size_t>::max();

			if (time > points_.back().time())
				return std::numeric_limits<size_t>::max();

			double delta = std::numeric_limits<double>::max();
			size_t ret = 0;
			for (size_t i = 0; i < size(); i++)
			{
				double dt = std::fabs(points_[i].time() - time);
				if (dt < delta)
				{
					delta = dt ;
					ret = i;
				}
			}

			return ret;
		}
	}
}
