#pragma once

#include "DriveModifier.h"

namespace xero
{
	namespace paths
	{

		class TankDriveModifier : public DriveModifier
		{
		public:
			TankDriveModifier();
			virtual ~TankDriveModifier();

			virtual bool modify(const RobotParams& robot, std::shared_ptr<RobotPath> path, const std::string& units);
		};
	}
}

