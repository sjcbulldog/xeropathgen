#pragma once

namespace xero
{
	namespace paths
	{
		class SCurveProfile
		{
		public:
			SCurveProfile(double maxjerk, double maxaccel, double maxvel);
			virtual ~SCurveProfile();

			bool update(double dist, double startvel = 0.0, double endvel = 0.0);

			double getJerk();
			double getAccel();
			double getVelocity();
			double getPosition();
			double getTotalTime();

		private:
			double maxjerk_;
			double maxaccel_;
			double maxvel_;
		};
	}
}

