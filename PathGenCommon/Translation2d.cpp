#include "Translation2d.h"
#include <cmath>
#include <algorithm>

namespace xero {
	namespace paths
	{
		Translation2d Translation2d::kIdentity(0.0, 0.0);

		Translation2d::Translation2d()
		{
			x_ = 0.0;
			y_ = 0.0;
		}

		Translation2d::Translation2d(double x, double y)
		{
			x_ = x;
			y_ = y;
		}

		Translation2d::Translation2d(const Translation2d& other)
		{
			x_ = other.getX();
			y_ = other.getY();
		}

		Translation2d::Translation2d(const Translation2d& start, const Translation2d& end)
		{
			x_ = end.getX() - start.getX();
			y_ = end.getY() - start.getY();
		}

		Translation2d::Translation2d(const Rotation2d& rot, double magnitude)
		{
			x_ = magnitude * rot.getCos();
			y_ = magnitude * rot.getSin();
		}

		Translation2d::~Translation2d()
		{
		}
		
		Rotation2d Translation2d::toRotation() const
		{
			double angle = std::atan2(y_, x_);
			return Rotation2d::fromRadians(angle);
		}

		double Translation2d::normalize() const
		{
			return std::sqrt(x_ * x_ + y_ * y_);
		}

		double Translation2d::normalizeSquared() const
		{
			return x_ * x_ + y_ * y_;
		}

		Translation2d Translation2d::translateBy(const Translation2d& translation) const
		{
			return Translation2d(x_ + translation.getX(), y_ + translation.getY());
		}

		Translation2d Translation2d::rotateBy(const Rotation2d& rotation) const
		{
			double x = x_ * rotation.getCos() - y_ * rotation.getSin();
			double y = x_ * rotation.getSin() + y_ * rotation.getCos();
			return Translation2d(x, y);
		}

		Translation2d Translation2d::inverse() const
		{
			return Translation2d(-x_, -y_);
		}

		Translation2d Translation2d::interpolate(const Translation2d& other, double percent) const
		{
			if (percent <= 0.0)
				return *this;
			else if (percent >= 1.0)
				return other;

			return extrapolate(other, percent);
		}

		Translation2d Translation2d::extrapolate(const Translation2d& other, double percent) const
		{
			double x = percent * (other.getX() - x_);
			double y = percent * (other.getY() - y_);
			return Translation2d(x, y);
		}

		Translation2d Translation2d::scale(double scale) const
		{
			return Translation2d(x_ * scale, y_ * scale);
		}

		Rotation2d Translation2d::direction() const
		{
			return Rotation2d(x_, y_, true);
		}

		double Translation2d::dot(const Translation2d& a, const Translation2d &b)
		{
			return a.getX()* b.getX() + a.getY() * b.getY();
		}

		double Translation2d::cross(const Translation2d& a, const Translation2d& b)
		{
			return a.getX()* b.getY() - a.getY() * b.getX();
		}

		double Translation2d::distance(const Translation2d& other) const
		{
			double dx = x_ - other.getX();
			double dy = y_ - other.getY();

			return std::sqrt(dx * dx + dy * dy);
		}

		bool Translation2d::epsilonEqual(const Translation2d& other, double eps) const
		{
			return MathUtils::epsilonEqual(getX(), other.getX(), eps) && MathUtils::epsilonEqual(getY(), other.getY(), eps);
		}

		Rotation2d Translation2d::getAngle(const Translation2d& a, const Translation2d& b)
		{
			double dy = b.getY() - a.getY();
			double dx = b.getX() - a.getX();
			double angle = std::atan2(dy, dx);
			return Rotation2d::fromRadians(angle);
		}
	}
}
