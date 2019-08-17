#include "Twist2d.h"
#include "MathUtils.h"
#include <cmath>

namespace xero {
	namespace paths
	{
		Twist2d Twist2d::kIdentity(0.0, 0.0, 0.0);

		Twist2d::Twist2d()
		{
			dx_ = 0.0;
			dy_ = 0.0;
			dtheta_ = 0.0;
		}

		Twist2d::Twist2d(double dx, double dy, double dtheta)
		{
			dx_ = dx;
			dy_ = dy;
			dtheta_ = dtheta;
		}

		Twist2d::~Twist2d()
		{
		}

		Twist2d Twist2d::scaled(double scale) const
		{
			return Twist2d(dx_ * scale, dy_ * scale, dtheta_ * scale);
		}

		double Twist2d::normal() const
		{
			if (dy_ == 0.0)
				return std::abs(dx_);

			return std::hypot(dx_, dy_);
		}

		double Twist2d::curvature() const
		{
			double dn = normal();

			if (std::abs(dtheta_) < MathUtils::kEpsilon && dn < MathUtils::kEpsilon)
				return 0.0;

			return dtheta_ / dn;
		}
	}
}
