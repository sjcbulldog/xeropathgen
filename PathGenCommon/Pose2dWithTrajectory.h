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
				curvature_ = 0.0;
				swrotation_ = 0.0;
			}

			Pose2dWithTrajectory(const Pose2d& pose, double time, double pos, double vel, double acc, double jerk) {
				pose_ = pose;
				time_ = time;
				position_ = pos;
				velocity_ = vel;
				acceleration_ = acc;
				jerk_ = jerk;
				curvature_ = 0.0;
				swrotation_ = 0.0;
			}

			Pose2dWithTrajectory(const Pose2d& pose, double time, double pos, double vel, double acc, double jerk, double cur, double rot) {
				pose_ = pose;
				time_ = time;
				position_ = pos;
				velocity_ = vel;
				acceleration_ = acc;
				jerk_ = jerk;
				curvature_ = cur;
				swrotation_ = rot;
			}

			virtual ~Pose2dWithTrajectory() {
			}

			const Pose2d &pose() const {
				return pose_;
			}

			void setPose(const Pose2d& p) {
				pose_ = p;
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

			double curvature() const {
				return curvature_;
			}

			void setSwRotation(double d) {
				swrotation_ = d;
			}

			double swrotation() const {
				return swrotation_;
			}

			double jerk() const {
				return jerk_;
			}

			double velocity() const {
				return velocity_;
			}

			void setPosition(double p) {
				position_ = p;
			}

			double acceleration() const {
				return acceleration_;
			}

			void setVelocity(double d) {
				velocity_ = d;
			}

			void setAcceleration(double a) {
				acceleration_ = a;
			}

			void setCurvature(double c) {
				curvature_ = c;
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
			double curvature_;
			double swrotation_;
		};
	}
}
