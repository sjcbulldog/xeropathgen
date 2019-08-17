#pragma once

#include "RobotPath.h"
#include "RobotParams.h"

namespace xero
{
	namespace paths
	{
		class DriveModifier
		{
		public:
			DriveModifier() {
			}

			virtual ~DriveModifier() {
			}

			virtual bool modify(const RobotParams &robot, std::shared_ptr<RobotPath> path, const std::string &units) = 0;

		private:
		};
	}
}

