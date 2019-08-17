#pragma once

#include "Pose2d.h"
#include "Pose2dWithCurvature.h"
#include <vector>
#include <memory>

namespace xero {
	namespace paths
	{
		class QuinticHermiteSpline
		{
		public:
			QuinticHermiteSpline(const Pose2d& p0, const Pose2d& p1);
			QuinticHermiteSpline(double x0, double x1, double dx0, double dx1, double ddx0, double ddx1,
				double y0, double y1, double dy0, double dy1, double ddy0, double ddy1);
			QuinticHermiteSpline(const QuinticHermiteSpline& spline);
			~QuinticHermiteSpline();

			Pose2d getStartPose() const ;
			Pose2d getEndPose() const ;
			Translation2d getPoint(double t) const ;
			Rotation2d getHeading(double t) const ;
			double getVelocity(double t) const ;
			double getCurvature(double t) const ;
			double getDCurvature(double t) const ;

			Pose2d getPose2d(double t) const {
				return Pose2d(getPoint(t), getHeading(t));
			}

			Pose2dWithCurvature getPose2dWithCurvature(double t) const {
				return Pose2dWithCurvature(getPose2d(t), getCurvature(t), getDCurvature(t) / getVelocity(t));
			}

			static double sumDCurvature2(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines);
			static double optimizeSpline(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines);

		private:
			struct ControlPoint
			{
				double ddx;
				double ddy;

				ControlPoint()
				{
					ddx = 0;
					ddy = 0;
				}

				ControlPoint(double vx, double vy)
				{
					ddx = vx;
					ddy = vy;
				}
			};

		private:
			double getDCurvature2(double t);
			double sumDCurvature2();
			static void runOptimizationIteration(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines);
			static double fitParabola(const Translation2d &p1, const Translation2d &p2, const Translation2d &p3);
			void compute();
			double fdx(double t) const {
				return 5 * ax* t* t* t* t + 4 * bx * t * t * t + 3 * cx * t * t + 2 * dx * t + ex;
			}

			double fdy(double t) const {
				return 5 * ay* t* t* t* t + 4 * by * t * t * t + 3 * cy * t * t + 2 * dy * t + ey;
			}

			double ddx(double t) const {
				return 20 * ax* t* t* t + 12 * bx * t * t + 6 * cx * t + 2 * dx;
			}

			double ddy(double t) const {
				return 20 * ay* t* t* t + 12 * by * t * t + 6 * cy * t + 2 * dy;
			}

			double dddx(double t) const {
				return 60 * ax* t* t + 24 * bx * t + 6 * cx;
			}

			double dddy(double t) const {
				return 60 * ay* t* t + 24 * by * t + 6 * cy;
			}

		private:
			static constexpr double kStepSize = 1.0;
			static constexpr double kMinDelta = 0.001;
			static constexpr int kSamples = 100;
			static constexpr size_t kMaxIterations = 100;

		private:
			double x0;
			double x1;
			double dx0;
			double dx1;
			double ddx0;
			double ddx1;
			double y0;
			double y1;
			double dy0;
			double dy1;
			double ddy0;
			double ddy1;
			double ax;
			double bx;
			double cx;
			double dx;
			double ex;
			double fx;
			double ay;
			double by;
			double cy;
			double dy;
			double ey;
			double fy;
		};
	}
}
