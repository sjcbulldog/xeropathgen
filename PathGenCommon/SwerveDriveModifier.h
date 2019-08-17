#pragma once
#include "DriveModifier.h"
#include "TrapezoidalProfile.h"

namespace xero {
	namespace paths {
		class SwerveDriveModifier : public DriveModifier
		{
		public:
			SwerveDriveModifier();
			virtual ~SwerveDriveModifier();

			virtual bool modify(const RobotParams& robot, std::shared_ptr<RobotPath> path, const std::string &units);
			void setRotationalPercent(double p) {
				percent_ = p;
			}

		private:
			enum Wheel
			{
				FL,
				FR,
				BL,
				BR
			};

		private:
			std::shared_ptr<TrapezoidalProfile> createRotationProfile(const RobotParams& params, std::shared_ptr<RobotPath> path);
			double RotationalToGround(const RobotParams robot, double rv);
			double GroundToRotational(const RobotParams robot, double rv);
			Translation2d GetWheelPerpendicularVector(const RobotParams& robot, Wheel w, double magnitude);

		private:
			double percent_;
		};
	}
}

