#include "SplinePair.h"
#include <cmath>

namespace xero
{
	namespace paths
	{
#ifdef NOTYET
		template <typename I> std::string n2hexstr(I w, size_t hex_len = sizeof(I) << 1) {
			static const char* digits = "0123456789ABCDEF";
			std::string rc(hex_len, '0');
			for (size_t i = 0, j = (hex_len - 1) * 4; i < hex_len; ++i, j -= 4)
				rc[i] = digits[(w >> j) & 0x0f];
			return rc;
		}
#endif

		SplinePair::SplinePair(Pose2d p0, Pose2d p1)
		{
			double dist = p0.distance(p1);
			double scale = 1.2 * dist;
			double x0 = p0.getTranslation().getX();
			double x1 = p1.getTranslation().getX();
			double dx0 = p0.getRotation().getCos() * scale;
			double dx1 = p1.getRotation().getCos() * scale;
			double ddx0 = 0.0;
			double ddx1 = 0.0;
			x_ = new QuinticHermiteSpline(x0, x1, dx0, dx1, ddx0, ddx1);

			double y0 = p0.getTranslation().getY();
			double y1 = p1.getTranslation().getY();
			double dy0 = p0.getRotation().getSin() * scale;
			double dy1 = p1.getRotation().getSin() * scale;
			double ddy0 = 0.0;
			double ddy1 = 0.0;
			y_ = new QuinticHermiteSpline(y0, y1, dy0, dy1, ddy0, ddy1);

			has_step_ = false;
			step_ = 0.1;
		}

		SplinePair::SplinePair(const QuinticHermiteSpline& x, const QuinticHermiteSpline& y)
		{
			x_ = new QuinticHermiteSpline(x);
			y_ = new QuinticHermiteSpline(y);

			has_step_ = false;
			step_ = 0.1;
		}

		SplinePair::~SplinePair()
		{
			delete x_;
			delete y_;
		}

		Translation2d SplinePair::evalPosition(double t)
		{
			double xval = x_->eval(t);
			double yval = y_->eval(t);

			return Translation2d(xval, yval);
		}

		Rotation2d SplinePair::evalHeading(double t)
		{
			double xval = x_->derivative(t);
			double yval = y_->derivative(t);

			return Rotation2d(xval, yval, true);
		}

		double SplinePair::getCurvature(double t) 
		{
			return (dx(t) * ddy(t) - ddx(t) * dy(t)) / ((dx(t) * dx(t) + dy(t) * dy(t)) * std::sqrt((dx(t) * dx(t) + dy(t) * dy(t))));
		}

		double SplinePair::getDCurvature(double t) 
		{
			double dx2dy2 = (dx(t) * dx(t) + dy(t) * dy(t));
			double num = (dx(t) * dddy(t) - dddx(t) * dy(t)) * dx2dy2 - 3 * (dx(t) * ddy(t) - ddx(t) * dy(t)) * (dx(t) * ddx(t) + dy(t) * ddy(t));
			return num / (dx2dy2 * dx2dy2 * std::sqrt(dx2dy2));
		}

		double SplinePair::getDCurvature2(double t) 
		{
			double dx2dy2 = (dx(t) * dx(t) + dy(t) * dy(t));
			double num = (dx(t) * dddy(t) - dddx(t) * dy(t)) * dx2dy2 - 3 * (dx(t) * ddy(t) - ddx(t) * dy(t)) * (dx(t) * ddx(t) + dy(t) * ddy(t));
			return num * num / (dx2dy2 * dx2dy2 * dx2dy2 * dx2dy2 * dx2dy2);
		}

		Pose2d SplinePair::getStartPose()
		{
			return Pose2d(evalPosition(0), evalHeading(0));
		}

		Pose2d SplinePair::getEndPose()
		{
			return Pose2d(evalPosition(1), evalHeading(1));
		}
	}
}
