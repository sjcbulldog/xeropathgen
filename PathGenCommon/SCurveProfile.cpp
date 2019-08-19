#include "SCurveProfile.h"
#include "TrapezoidalProfile.h"
#include <cmath>

namespace xero
{
	namespace paths
	{
		SCurveProfile::SCurveProfile(double jerkmax, double accmax, double accmin, double velmax) 
		{
			t_ = std::vector<double>{ std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan("") };
			j_ = std::vector<double>{ jerkmax, 0, -jerkmax, 0, -jerkmax, 0, jerkmax };
			a_ = std::vector<double>{ std::nan(""), accmax, std::nan(""), 0.0, std::nan(""), accmin, std::nan("") };
			v_ = std::vector<double>{ std::nan(""), std::nan(""), std::nan(""), velmax, std::nan(""), std::nan(""), std::nan("") };
			p_ = std::vector<double>{ std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan("") };

			step_ = 0.01;
		}

		SCurveProfile::~SCurveProfile()
		{

		}

		void SCurveProfile::update(double dist, double start_velocity, double end_velocity)
		{
			rampUp(start_velocity);
			rampDown(end_velocity);

			p_[3] = dist - p_[0] - p_[1] - p_[2] - p_[4] - p_[5] - p_[6];
			t_[3] = p_[3] / v_[3];
		}

		//
		// Go from start_velocity to to velmax_.  The acceleration
		// will be a trapezoidal profile.
		//
		void SCurveProfile::rampUp(double start_velocity)
		{
			double veldist = v_[3] - start_velocity;

			TrapezoidalProfile profile(j_[0], j_[2], a_[1]);
			profile.update(veldist, 0.0, 0.0);

			t_[0] = profile.getTimeAccel();
			t_[1] = profile.getTimeCruise();
			t_[2] = profile.getTimeDecel();
		}

		void SCurveProfile::rampDown(double end_velocity)
		{
			double veldist = end_velocity - v_[3];
			TrapezoidalProfile profile(j_[4], j_[6], a_[5]);
			profile.update(veldist, 0.0, 0.0);

			t_[4] = profile.getTimeAccel();
			t_[5] = profile.getTimeCruise();
			t_[6] = profile.getTimeDecel();
		}
	}
}