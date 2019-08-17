#pragma once
#include "Pose2d.h"

namespace xero
{
	namespace paths
	{
		class Pose2dWithTrajectory : public ICsv
		{
		public:
			Pose2dWithTrajectory()
			{
				time_ = 0.0;
				position_ = 0.0;
				velocity_ = 0.0;
				acceleration_ = 0.0;
				jerk_ = 0.0;
			}

			Pose2dWithTrajectory(const Pose2d& pose, double time, double pos, double vel, double acc, double jerk) {
				pose_ = pose;
				time_ = time;
				position_ = pos;
				velocity_ = vel;
				acceleration_ = acc;
				jerk_ = jerk;
			}

			const Pose2d &pose() const {
				return pose_;
			}

			const Translation2d& translation() const {
				return pose_.getTranslation();
			}

			const Rotation2d& rotation() const {
				return pose_.getRotation();
			}

			double x() const {
				return pose_.getTranslation().getX();
			}

			double y() const {
				return pose_.getTranslation().getY();
			}

			double time() const {
				return time_;
			}

			double position() const {
				return position_;
			}

			double velocity() const {
				return velocity_;
			}

			double acceleration() const {
				return acceleration_;
			}

			double jerk() const {
				return jerk_;
			}

			double getField(const std::string& field) const;

			Pose2dWithTrajectory interpolate(const Pose2dWithTrajectory& other, double percent) const;

		private:
			Pose2d pose_;
			double time_;
			double position_;
			double velocity_;
			double acceleration_;
			double jerk_;
		};
	}
}