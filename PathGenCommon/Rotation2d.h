#pragma once

#include "MathUtils.h"

namespace xero 
{
	namespace paths
	{
		class Rotation2d
		{
		public:
			Rotation2d();
			Rotation2d(double x, double y, bool normalize);
			Rotation2d(const Rotation2d& other);
			~Rotation2d();

			double getCos() const { return cos_angle_; }
			double getSin() const { return sin_angle_; }
			double getTan() const;
			double toRadians() const;
			double toDegrees() const;

			static Rotation2d fromRadians(double angle);
			static Rotation2d fromDegrees(double angle);

			Rotation2d rotateBy(const Rotation2d& other) const;
			Rotation2d normal() const;
			Rotation2d inverse() const;
			Rotation2d interpolate(const Rotation2d& other, double percent) const;

			bool isParallel(const Rotation2d& other) const;
			bool epsilonEquals(const Rotation2d& other, double eps = MathUtils::kEpsilon) const;


		private:
			double sin_angle_;
			double cos_angle_;
		};
	}
}
