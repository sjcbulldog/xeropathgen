#pragma once
#include <vector>

namespace xero
{
	namespace paths
	{
		class QuinticHermiteSpline
		{
		public:
			QuinticHermiteSpline(double v0, double v1, double dv0, double dv1, double ddv0, double ddv1);

			double eval(double t);
			double derivative(double t);
			double derivative2(double t);
			double derivative3(double t);

			double v0() { return v0_; }
			double v1() { return v1_; }
			double dv0() { return dv0_; }
			double dv1() { return dv1_; }
			double ddv0() { return ddv0_; }
			double ddv1() { return ddv1_; }

			void ddv0(double v) { ddv0_ = v; compute(); }
			void ddv1(double v) { ddv1_ = v; compute(); }
			double a() { return a_; }
			double b() { return b_; }
			double c() { return c_; }
			double d() { return d_; }
			double e() { return e_; }
			double f() { return f_; }

		private:
			void compute();

		private:
			double a_, b_, c_, d_, e_, f_;
			double v0_, v1_, dv0_, dv1_, ddv0_, ddv1_;
		};
	}
}

