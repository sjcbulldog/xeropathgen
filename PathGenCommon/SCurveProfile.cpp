#include "SCurveProfile.h"
#include "TrapezoidalProfile.h"
#include <cmath>
#include <cassert>
#include <numeric>
#include <stdexcept>
#include <iostream>

namespace xero
{
	namespace paths
	{
		SCurveProfile::SCurveProfile(double jerkmax, double jerkmin, double accmax, double accmin, double velmax) 
		{
			assert(jerkmin < 0.0);
			assert(accmin < 0.0);

			t_ = std::vector<double>{ 0.0, std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan("") };
			j_ = std::vector<double>{ jerkmax, 0.0, jerkmin, 0, jerkmin, 0, jerkmax, 0.0 };
			a_ = std::vector<double>{ 0.0, accmax, accmax, 0.0, 0.0, accmin, accmin, 0.0 };
			v_ = std::vector<double>{ 0.0, std::nan(""), std::nan(""), std::nan(""), velmax, std::nan(""), std::nan(""), std::nan("") };
			p_ = std::vector<double>{ 0.0, std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan(""), std::nan("") };

			step_ = 0.01;

			jerkmax_ = jerkmax;
			jerkmin_ = jerkmin;
			accmin_ = accmin;
			accmax_ = accmax;
		}

		SCurveProfile::~SCurveProfile()
		{
		}

		void SCurveProfile::update(double dist, double start_velocity, double end_velocity)
		{
			v_[0] = start_velocity;
			v_[7] = end_velocity;

			while (v_[4] > 0)
			{
				rampUp();
				rampDown();

				p_[4] = dist - p_[1] - p_[2] - p_[3] - p_[5] - p_[6] - p_[7];
				if (p_[4] >= 0)
				{
					t_[4] = p_[4] / v_[4];
					break;
				}

				v_[4] -= 1;
			}

			if (v_[4] <= 0.0)
				throw std::runtime_error("jerk plus acceleration settings do not provide for solutions");

			double timetotal = 0.0;
			double disttotal = 0.0;
			for (size_t i = 0; i < t_.size(); i++)
			{
				timetotal += t_[i];
				t_[i] = timetotal;

				disttotal += p_[i];
				p_[i] = disttotal;
			}
		}

		//
		// Go from start_velocity to to velmax_.  The acceleration
		// will be a trapezoidal profile.
		//
		void SCurveProfile::rampUp()
		{
			double dt;
			double veldist = v_[4] - v_[0];

			TrapezoidalProfile profile(jerkmax_, jerkmin_, accmax_);
			profile.update(veldist, 0.0, 0.0);

			t_[1] = profile.getTimeAccel();
			t_[2] = profile.getTimeCruise() ;
			t_[3] = profile.getTimeDecel() ;

			double vpeak = profile.getActualMaxVelocity();
			a_[1] = vpeak;
			a_[2] = vpeak;

			for (size_t n = 1; n <= 3; n++)
			{
				dt = t_[n];
				v_[n] = v_[n-1] + a_[n-1] * dt + 1.0 / 2.0 * j_[n - 1] * dt * dt;
				p_[n] = v_[n-1] * dt + a_[n-1] * dt * dt / 2.0 + j_[n - 1] * dt * dt * dt / 6.0  ;
			}
		}

		void SCurveProfile::rampDown()
		{
			double dt;
			double veldist = -(v_[7] - v_[4]);

			TrapezoidalProfile profile(-jerkmin_,-jerkmax_, -accmin_);
			profile.update(veldist, 0.0, 0.0);

			t_[5] = profile.getTimeAccel();
			t_[6] = profile.getTimeCruise();
			t_[7] = profile.getTimeDecel();

			double vpeak = profile.getActualMaxVelocity();
			a_[5] = -vpeak;
			a_[6] = -vpeak;

			for (size_t n = 5; n <= 7; n++)
			{
				dt = t_[n];
				v_[n] = v_[n - 1] + a_[n - 1] * dt + 1.0 / 2.0 * j_[n - 1] * dt * dt;
				p_[n] = v_[n - 1] * dt + a_[n - 1] * dt * dt / 2.0 + j_[n - 1] * dt * dt * dt / 6.0 ;
			}
		}

		double SCurveProfile::getAccel(double t) const {
			size_t region = findRegion(t);
			double ret = 0.0;
			double dt = t - t_[region - 1];

			ret = a_[region - 1] + dt * j_[region - 1];
			return ret;
		}

		double SCurveProfile::getVelocity(double t) const {
			size_t region = findRegion(t);
			double ret = 0.0;
			double dt = t - t_[region - 1];

			ret = v_[region - 1] + a_[region - 1] * dt + j_[region - 1] * dt * dt / 2.0;
			return ret;
		}

		double SCurveProfile::getDistance(double t) const {
			size_t region = findRegion(t);
			double dt = t - t_[region - 1];

			double ret = p_[region - 1] + v_[region - 1] * dt + a_[region - 1] * dt * dt / 2.0 + j_[region - 1] * dt * dt * dt / 6.0;
			return ret;
		}

		double SCurveProfile::getTotalTime() const {
			return t_[7];
		}

		double SCurveProfile::getTotalDistance() const {
			return p_[7];
		}

		double SCurveProfile::getStartVelocity() const {
			return v_[0];
		}

		double SCurveProfile::getEndVelocity() const {
			return v_[7];
		}

		size_t SCurveProfile::findRegion(double t) const {
			assert(t >= 0.0);
			assert(t <= t_[7] + kEpsilon);

			for (size_t i = 1; i <= 7; i++)
			{
				if (t < t_[i])
					return i;
			}

			return t_.size() - 1;
		}
	}
}