#pragma once

#include "RobotParams.h"
#include <string>
#include <vector>

namespace xero
{
	namespace paths
	{
		class DriveBaseData
		{
		public:
			DriveBaseData() = delete;
			~DriveBaseData() = delete;


			static constexpr const char* TankDriveName = "Tank Drive";
			static constexpr const char* SwerveDriveName = "Swerve Drive";
			static constexpr const char* InvalidDriveName = "Invalid Drive";

			static const std::vector<std::string> getTrajectories(RobotParams::DriveType type);


			static RobotParams::DriveType nameToType(const std::string& name)
			{
				RobotParams::DriveType ret = RobotParams::DriveType::Invalid;

				if (name == TankDriveName)
					ret = RobotParams::DriveType::TankDrive;
				else if (name == SwerveDriveName)
					ret = RobotParams::DriveType::SwerveDrive;
				else
					assert(false);

				return ret;
			}


			static std::string typeToName(RobotParams::DriveType t)
			{
				std::string ret = "Error";

				switch (t)
				{
				case RobotParams::DriveType::TankDrive:
					ret = TankDriveName;
					break;

				case RobotParams::DriveType::SwerveDrive:
					ret = SwerveDriveName;
					break;

				case RobotParams::DriveType::Invalid:
					ret = InvalidDriveName;
					break;

				default:
					assert(false);
					break;
				}

				return ret;
			}

		private:
			static std::vector<std::string> tank_drive_trajectories_;
			static std::vector<std::string> swerve_drive_trajectories_;
		};
	}
}

