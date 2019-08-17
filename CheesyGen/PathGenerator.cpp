#include "PathGenerator.h"
#include "QuinticHermiteSpline.h"
#include "SplineGenerator.h"

namespace xero
{
	namespace paths
	{

		PathGenerator::PathGenerator(double maxx, double maxy, double maxtheta)
		{
			maxDx_ = maxx;
			maxDy_ = maxy;
			maxDTheta_ = maxtheta;
		}


		PathGenerator::~PathGenerator()
		{
		}

		Trajectory PathGenerator::generate(const std::vector<Pose2d>& waypoints) const
		{
			std::vector<std::shared_ptr<QuinticHermiteSpline>> splines;
			for (size_t i = 0; i < waypoints.size() - 1; i++)
			{
				auto spline = std::make_shared<QuinticHermiteSpline>(waypoints[i], waypoints[i + 1]);
				splines.push_back(spline);
			}
			QuinticHermiteSpline::optimizeSpline(splines);
			std::vector<Pose2dWithCurvature> path = SplineGenerator::parameterizeSpline(splines, maxDx_, maxDy_, maxDTheta_);
			return Trajectory(path);
		}
	}
}
