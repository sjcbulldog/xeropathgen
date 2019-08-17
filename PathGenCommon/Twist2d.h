#pragma once

/// \file

namespace xero {
	namespace paths
	{
		/// \brief This class represents a location and an angle
		class Twist2d
		{
		public:
			Twist2d();
			Twist2d(double dx, double dy, double dtheta);
			~Twist2d();

			double getX() const {
				return dx_;
			}

			double getY() const {
				return dy_;
			}

			double getTheta() const {
				return dtheta_;
			}

			static Twist2d kIdentity;

			Twist2d scaled(double scale) const;
			double normal() const;
			double curvature() const;

		private:
			double dx_;
			double dy_;
			double dtheta_;
		};
	}
}

