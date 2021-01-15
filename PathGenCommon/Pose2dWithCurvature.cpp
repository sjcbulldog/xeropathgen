#include "Pose2DWithCurvature.h"

namespace xero
{
	namespace paths
	{
		double Pose2dWithCurvature::triangleArea(const Pose2d& a, const Pose2d& b, const Pose2d& c)
		{
			return ((b.getTranslation().getX() - a.getTranslation().getX()) * (c.getTranslation().getY() - a.getTranslation().getY())) -
				((b.getTranslation().getY() - a.getTranslation().getY()) * (c.getTranslation().getX() - a.getTranslation().getX()));
		}

		double Pose2dWithCurvature::curvature(const Pose2d& a, const Pose2d& b, const Pose2d& c)
		{
			double area = triangleArea(a, b, c);

			double len1 = a.distance(b);
			double len2 = b.distance(c);
			double len3 = a.distance(c);

			return 4.0 * area / (len1 * len2 * len3);
		}
	}
}
