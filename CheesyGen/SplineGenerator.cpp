#include "SplineGenerator.h"

namespace xero
{
	namespace paths
	{
		std::vector<Pose2dWithCurvature> SplineGenerator::parameterizeSpline(const QuinticHermiteSpline& spline, double maxDx, double maxDy,
			double maxDTheta, double t0, double t1)
		{
			std::vector<Pose2dWithCurvature> result;

			result.push_back(spline.getPose2dWithCurvature(0.0));
			double dt = (t1 - t0);
			double incr = dt / kMinSampleSize;
			for (double t = 0; t < t1; t += incr)
				getSegmentArc(spline, result, t, t + incr, maxDx, maxDy, maxDTheta);

			return result;
		}

		std::vector<Pose2dWithCurvature> SplineGenerator::parameterizeSpline(std::vector<std::shared_ptr<QuinticHermiteSpline>>& splines, 
			double maxDx, double maxDy,	double maxDTheta)
		{
			std::vector<Pose2dWithCurvature> result;
			if (splines.size() == 0)
				return result;

			result.push_back(splines[0]->getPose2dWithCurvature(0.0));
			for (auto spline : splines)
			{
				std::vector<Pose2dWithCurvature> samples = parameterizeSpline(*spline, maxDx, maxDy, maxDTheta, 0.0, 1.0);
				samples.erase(samples.begin());
				std::move(samples.begin(), samples.end(), std::back_inserter(result));
			}

			return result;
		}

		void SplineGenerator::getSegmentArc(const QuinticHermiteSpline& spline, std::vector<Pose2dWithCurvature>& results, double t0, double t1,
			double maxDx, double maxDy, double maxDTheta)
		{
			const Translation2d& p0 = spline.getPoint(t0);
			const Translation2d& p1 = spline.getPoint(t1);
			const Rotation2d& r0 = spline.getHeading(t0);
			const Rotation2d& r1 = spline.getHeading(t1);
			Pose2d transformation = Pose2d(Translation2d(p0, p1).rotateBy(r0.inverse()), r1.rotateBy(r0.inverse()));
			Twist2d twist = Pose2d::logfn(transformation);
			if (twist.getY() > maxDy || twist.getX() > maxDx || twist.getTheta() > maxDTheta) {
				getSegmentArc(spline, results, t0, (t0 + t1) / 2, maxDx, maxDy, maxDTheta);
				getSegmentArc(spline, results, (t0 + t1) / 2, t1, maxDx, maxDy, maxDTheta);
			}
			else {
				results.push_back(spline.getPose2dWithCurvature(t1));
			}
		}
	}
}
