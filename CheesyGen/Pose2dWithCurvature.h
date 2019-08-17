#pragma once

#include "Pose2d.h"
#include "ICsv.h"

namespace xero
{
	namespace paths
	{
		class Pose2dWithCurvature : public ICsv
		{
		public:
			Pose2dWithCurvature();
			Pose2dWithCurvature(const Pose2d &pose, double curvature);
			Pose2dWithCurvature(const Pose2d& pose, double curvature, double dcurvature_ds);
			Pose2dWithCurvature(const Translation2d &trans, const Rotation2d &rot, double curvature);
			Pose2dWithCurvature(const Translation2d& trans, const Rotation2d& rot, double curvature, double dcurvature_ds);
			~Pose2dWithCurvature();

			const Pose2d& getPose() const {
				return pose_;
			}

			double getCurvature() const {
				return curvature_;
			}

			double getDcurvatureDs() const {
				return dcurvature_ds_;
			}

			Pose2dWithCurvature transformBy(const Pose2d& transform) const {
				return Pose2dWithCurvature(pose_.transformBy(transform), getCurvature(), getCurvature());
			}

			Pose2dWithCurvature mirror() const {
				return Pose2dWithCurvature(pose_.mirror(), -getCurvature(), -getCurvature());
			}

			const Translation2d& getTranslation() const {
				return pose_.getTranslation();
			}

			const Rotation2d& getRotation() const {
				return pose_.getRotation();
			}

			Pose2dWithCurvature interpolate(const Pose2dWithCurvature& other, double percent) const;
			double distance(const Pose2dWithCurvature& other) const ;

			virtual double getField(const std::string& field) const;


		private:
			Pose2d pose_;
			double curvature_;
			double dcurvature_ds_;
		};
	}
}
