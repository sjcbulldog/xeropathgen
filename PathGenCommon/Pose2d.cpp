#include "Pose2d.h"
#include "Twist2d.h"
#include <limits>
#include <cmath>
#include <stdexcept>

namespace xero
{
	namespace paths
	{

		Pose2d::Pose2d()
		{
		}

		Pose2d::Pose2d(double x, double y)
		{
			pos_ = Translation2d(x, y);
		}

		Pose2d::Pose2d(double x, double y, const Rotation2d& rot)
		{
			pos_ = Translation2d(x, y);
			rotation_ = rot;
		}

		Pose2d::Pose2d(const Translation2d& pos)
		{
			pos_ = pos;
		}

		Pose2d::Pose2d(const Rotation2d& rot)
		{
			rotation_ = rot;
		}

		Pose2d::Pose2d(const Translation2d& pos, const Rotation2d& rot)
		{
			pos_ = pos;
			rotation_ = rot;
		}

		Pose2d::Pose2d(const Pose2d& other)
		{
			pos_ = other.getTranslation();
			rotation_ = other.getRotation();
		}

		Pose2d::~Pose2d()
		{
		}

		Pose2d Pose2d::transformBy(const Pose2d& other) const
		{
			Translation2d pos = pos_.translateBy(other.getTranslation().rotateBy(rotation_));
			Rotation2d rot = rotation_.rotateBy(other.getRotation());
			return Pose2d(pos, rot);
		}

		Pose2d Pose2d::inverse() const
		{
			Rotation2d rotinv = rotation_.inverse();
			Translation2d pos = pos_.inverse().rotateBy(rotinv);
			return Pose2d(pos, rotinv);
		}

		Pose2d Pose2d::normal() const
		{
			return Pose2d(pos_, rotation_.normal());
		}

		Translation2d Pose2d::intersection(const Pose2d& other) const
		{
			if (rotation_.isParallel(other.getRotation()))
				return Translation2d(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

			if (std::abs(rotation_.getCos()) < std::abs(other.getRotation().getCos()))
				return intersection(*this, other);

			return intersection(other, *this);
		}

		bool Pose2d::espilonEquals(const Pose2d& other, double eps) const
		{
			return pos_.epsilonEqual(other.getTranslation(), eps) && rotation_.epsilonEquals(other.getRotation(), eps);
		}

		bool Pose2d::espilonEquals(const Pose2d& other, double poseps, double roteps) const
		{
			return pos_.epsilonEqual(other.getTranslation(), poseps) && rotation_.epsilonEquals(other.getRotation(), roteps);
		}

		Pose2d Pose2d::expfn(const Twist2d& delta)
		{
			double sin_theta = std::sin(delta.getTheta());
			double cos_theta = std::cos(delta.getTheta());
			double s, c;
			if (std::abs(delta.getTheta()) < MathUtils::kEpsilon) {
				s = 1.0 - 1.0 / 6.0 * delta.getTheta() * delta.getTheta();
				c = .5 * delta.getTheta();
			}
			else {
				s = sin_theta / delta.getTheta();
				c = (1.0 - cos_theta) / delta.getTheta();
			}
			Translation2d trans(delta.getX() * s - delta.getY() * c, delta.getX() * c + delta.getY() * s);
			Rotation2d rot(cos_theta, sin_theta, false);
			return Pose2d(trans, rot);
		}

		Twist2d Pose2d::logfn(const Pose2d& transform)
		{
			double dtheta = transform.getRotation().toRadians();
			double half_dtheta = 0.5 * dtheta;
			double cos_minus_one = transform.getRotation().getCos() - 1.0;
			double halftheta_by_tan_of_halfdtheta;
			if (std::abs(cos_minus_one) < MathUtils::kEpsilon) {
				halftheta_by_tan_of_halfdtheta = 1.0 - 1.0 / 12.0 * dtheta * dtheta;
			}
			else {
				halftheta_by_tan_of_halfdtheta = -(half_dtheta * transform.getRotation().getSin()) / cos_minus_one;
			}
			Rotation2d rot(halftheta_by_tan_of_halfdtheta, -half_dtheta, false);
			Translation2d trans = transform.getTranslation().rotateBy(rot);
			return Twist2d(trans.getX(), trans.getY(), dtheta);
		}

		Pose2d Pose2d::interpolate(const Pose2d& other, double percent) const
		{
			if (percent <= 0) {
				return *this;
			}
			else if (percent >= 1) {
				return other;
			}

			Twist2d twist = Pose2d::logfn(inverse().transformBy(other));
			return transformBy(Pose2d::expfn(twist.scaled(percent)));
		}

		double Pose2d::distance(const Pose2d &other) const
		{
			return Pose2d::logfn(inverse().transformBy(other)).normal();
		}

		Pose2d Pose2d::mirror() const
		{
			Translation2d trans(pos_.getX(), -pos_.getY());
			return Pose2d(trans, rotation_.inverse());
		}

		Translation2d Pose2d::intersection(const Pose2d& a, const Pose2d& b)
		{
			Rotation2d a_r = a.getRotation();
			Rotation2d b_r = b.getRotation();
			Translation2d a_t = a.getTranslation();
			Translation2d b_t = b.getTranslation();

			double tan_b = b_r.getTan();
			double t = ((a_t.getX() - b_t.getX()) * tan_b + b_t.getY() - a_t.getY())
				/ (a_r.getSin() - a_r.getCos() * tan_b);

			if (std::isnan(t))
				return Translation2d(std::numeric_limits<double>::infinity(), std::numeric_limits<double>::infinity());

			Translation2d a_r_pos(a_r.getCos(), a_r.getSin());
			return a_t.translateBy(a_r_pos.scale(t));
		}

		bool Pose2d::isColinear(const Pose2d& other)
		{
			if (!getRotation().isParallel(other.getRotation()))
				return false;

			Twist2d twist = logfn(inverse().transformBy(other));
			return (MathUtils::epsilonEqual(twist.getY(), 0.0) && MathUtils::epsilonEqual(twist.getTheta(), 0.0));
		}

		double Pose2d::getField(const std::string& field) const
		{
			double v;

			if (field == "x")
			{
				v = pos_.getX();
			}
			else if (field == "y")
			{
				v = pos_.getY();
			}
			else if (field == "heading")
			{
				v = rotation_.toDegrees();
			}
			else 
			{
				std::string msg = "field '" + field + "' is not a defined field";
				throw std::runtime_error(msg.c_str());
			}

			return v;
		}
	}
}
