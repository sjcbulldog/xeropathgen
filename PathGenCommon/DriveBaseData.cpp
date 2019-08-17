#include "DriveBaseData.h"
#include <cassert>

namespace xero {
	namespace paths {
		std::vector<std::string> DriveBaseData::tank_drive_trajectories_ =
		{
			"main",
			"left",
			"right"
		};

		std::vector<std::string> DriveBaseData::swerve_drive_trajectories_ =
		{
			"main",
			"fl",
			"fr",
			"bl",
			"br"
		};

		const std::vector<std::string> DriveBaseData::getTrajectories(RobotParams::DriveType type)
		{
			switch (type)
			{
			case RobotParams::DriveType::TankDrive:
				return tank_drive_trajectories_;

			case RobotParams::DriveType::SwerveDrive:
				return swerve_drive_trajectories_;
			}

			assert(false);
			return std::vector<std::string>();
		}
	}
}
