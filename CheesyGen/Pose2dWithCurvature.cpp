#include "Pose2dWithCurvature.h"

namespace xero
{
	namespace paths
	{

		Pose2dWithCurvature::Pose2dWithCurvature()
		{
			curvature_ = 0.0;
			dcurvature_ds_ = 0.0;
		}

		Pose2dWithCurvature::Pose2dWithCurvature(const Pose2d& pose, double curvature)
		{
			pose_ = pose;
			curvature_ = curvature;
			dcurvature_ds_ = 0.0;
		}

		Pose2dWithCurvature::Pose2dWithCurvature(const Pose2d& pose, double curvature, double dcurvature_ds)
		{
			pose_ = pose;
			curvature_ = curvature;
			dcurvature_ds_ = dcurvature_ds;
		}

		Pose2dWithCurvature::Pose2dWithCurvature(const Translation2d& trans, const Rotation2d& rot, double curvature)
		{
			pose_ = Pose2d(trans, rot);
			curvature_ = curvature;
			dcurvature_ds_ = 0.0;
		}

		Pose2dWithCurvature::Pose2dWithCurvature(const Translation2d& trans, const Rotation2d& rot, double curvature, double dcurvature_ds)
		{
			pose_ = Pose2d(trans, rot);
			curvature_ = curvature;
			dcurvature_ds_ = dcurvature_ds;
		}

		Pose2dWithCurvature::~Pose2dWithCurvature()
		{
		}

		Pose2dWithCurvature Pose2dWithCurvature::interpolate(const Pose2dWithCurvature& other, double percent) const
		{
			Pose2d pose = pose_.interpolate(other.getPose(), percent);
			double cur = MathUtils::interpolate(getCurvature(), other.getCurvature(), percent);
			double dcurds = MathUtils::interpolate(getDcurvatureDs(), other.getDcurvatureDs(), percent);
			return Pose2dWithCurvature(pose, cur, dcurds);
		}

		double Pose2dWithCurvature::distance(const Pose2dWithCurvature& other) const
		{
			return pose_.distance(other.getPose());
		}

		double Pose2dWithCurvature::getField(const std::string& field) const
		{
			double v;

			if (field == "curvature")
			{
				v = curvature_;
			}
			else if (field == "dsdcurvature")
			{
				v = dcurvature_ds_;
			}
			else
			{
				v = pose_.getField(field);
			}
			return v;
		}
	}
}
