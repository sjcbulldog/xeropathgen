#pragma once
#include <limits>

namespace xero {
	namespace paths
	{
		class MinMaxAcceleration
		{
		public:
			MinMaxAcceleration() {
				maxaccel_ = std::numeric_limits<double>::max();
				minaccel_ = -std::numeric_limits<double>::max();
			}

			MinMaxAcceleration(double minacc, double maxacc)
			{
				minaccel_ = minacc;
				maxaccel_ = maxacc;
			}

			virtual ~MinMaxAcceleration() {
			}

			double getMinAccel() const {
				return minaccel_;
			}

			double getMaxAccel() const {
				return maxaccel_;
			}

			bool isValid() const {
				return minaccel_ < maxaccel_;
			}

		private:
			double maxaccel_;
			double minaccel_;
		};
	}
}
