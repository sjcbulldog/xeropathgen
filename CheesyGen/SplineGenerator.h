#pragma once

#include "Pose2dWithCurvature.h"
#include "QuinticHermiteSpline.h"
#include <vector>
#include <memory>

namespace xero
{
	namespace paths
	{

		class SplineGenerator
		{
		public:
			SplineGenerator() = delete ;
			~SplineGenerator() = delete ;

			static std::vector<Pose2dWithCurvature> parameterizeSpline(const QuinticHermiteSpline& spline, double maxDx, double maxDy, 
							double maxDTheta, double t0, double t1);

			static std::vector<Pose2dWithCurvature> parameterizeSpline(std::vector<std::shared_ptr<QuinticHermiteSpline>> &splines, double maxDx, double maxDy,
				double maxDTheta);

		private:
			static void getSegmentArc(const QuinticHermiteSpline& spline, std::vector<Pose2dWithCurvature>& results, double t0, double t1,
				double maxDx, double maxDy, double maxDtheta);

		private:
			static constexpr double kMaxDX = 2.0; //inches
			static constexpr double kMaxDY = 0.05; //inches
			static constexpr double kMaxDTheta = 0.1; //radians!
			static constexpr int kMinSampleSize = 1;
		};
	}
}

