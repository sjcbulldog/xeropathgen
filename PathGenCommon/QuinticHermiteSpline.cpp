#include "QuinticHermiteSpline.h"
#include <cassert>

namespace xero
{
	namespace paths
	{
		QuinticHermiteSpline::QuinticHermiteSpline(double v0, double v1, double dv0, double dv1, double ddv0, double ddv1)
		{
			v0_ = v0;
			v1_ = v1;
			dv0_ = dv0;
			dv1_ = dv1;
			ddv0_ = ddv0;
			ddv1_ = ddv1;
			compute();
		}

		void QuinticHermiteSpline::compute()
		{
			a_ = -6 * v0_ - 3 * dv0_ - 0.5 * ddv0_ + 0.5 * ddv1_ - 3 * dv1_ + 6 * v1_;
			b_ = 15 * v0_ + 8 * dv0_ + 1.5 * ddv0_ - ddv1_ + 7 * dv1_ - 15 * v1_;
			c_ = -10 * v0_ - 6 * dv0_ - 1.5 * ddv0_ + 0.5 * ddv1_ - 4 * dv1_ + 10 * v1_;
			d_ = 0.5 * ddv0_;
			e_ = dv0_;
			f_ = v0_;
		}

		double QuinticHermiteSpline::eval(double t)
		{
			return a() * t * t * t * t * t + b() * t * t * t * t + c() * t * t * t + d() * t * t + e() * t + f();
		}

		double QuinticHermiteSpline::derivative(double t)
		{
			return 5 * a() * t * t * t * t + 4 * b() * t * t * t + 3 * c() * t * t + 2 * d() * t + e();
		}

		double QuinticHermiteSpline::derivative2(double t)
		{
			return 20 * a() * t * t * t + 12 * b() * t * t + 6 * c() * t + 2 * d();
		}

		double QuinticHermiteSpline::derivative3(double t)
		{
			return 60 * a() * t * t + 24 * b() * t + 6 * c();
		}
	}
}