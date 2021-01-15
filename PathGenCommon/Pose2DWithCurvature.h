#pragma once

#include "Pose2d.h"

namespace xero
{
	namespace paths
	{
		class Pose2dWithCurvature : public Pose2d
		{
		public:
			Pose2dWithCurvature() {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(double x, double y) : Pose2d(x, y) {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(double x, double y, const Rotation2d& rot) : Pose2d(x, y, rot) {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(const Translation2d& pos) : Pose2d(pos) {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(const Rotation2d& rot) : Pose2d(rot) {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(const Translation2d& pos, const Rotation2d& rot) : Pose2d(pos, rot) {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(const Pose2d& other) : Pose2d(other) {
				curvature_ = 0.0;
			}

			Pose2dWithCurvature(const Pose2d&other, double cur) : Pose2d(other) {
				curvature_ = cur;
			}

			virtual ~Pose2dWithCurvature() {
			}

			void setCurvature(double c) {
				curvature_ = c;
			}

			double curvature() const {
				return curvature_;
			}

			Pose2dWithCurvature interpolate(const Pose2dWithCurvature& other, double percent) const {
				Pose2d pose = Pose2d::interpolate(other, percent);
				if (percent < 0)
					percent = 0;
				else if (percent > 1)
					percent = 1;
				double c = curvature() + (other.curvature() - curvature()) * percent;

				return Pose2dWithCurvature(pose, c);
			}

		private:
			double curvature_;
		};
	}
}
