#pragma once

namespace xero 
{
	namespace paths
	{
		class MathUtils
		{
		public:
			MathUtils() = delete;
			~MathUtils() = delete;

			static double constexpr kEpsilon = 1e-5;
			static double constexpr kPI = 3.14159265358979323846264338327950288419716939937510582097494459230;

			static bool epsilonEqual(double a, double b, double ep = kEpsilon);
			static double interpolate(double first, double second, double percent);
			static double radiansToDegrees(double rads);
			static double degreesToRadians(double degrees);

			static double boundDegrees(double d) {
				if (d > 180.0)
				{
					while (d > 180.0)
						d -= 360;
				}
				else if (d <= -180)
				{
					while (d <= -180.0)
						d += 360;
				}

				return d;
			}

			static double BoundRadians(double r)
			{
				if (r > MathUtils::kPI)
					r -= 2 * MathUtils::kPI;
				else if (r <= -MathUtils::kPI)
					r += 2 * MathUtils::kPI;

				return r;
			}
		};
	}
}

