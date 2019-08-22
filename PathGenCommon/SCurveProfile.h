#pragma once

#include "SpeedProfileGenerator.h"
#include <vector>
#include <cassert>

namespace xero
{
	namespace paths
	{
		class SCurveProfile : public SpeedProfileGenerator
		{
		private:
			static constexpr double kEpsilon = 1e-5;

		public:
			SCurveProfile(double jerkmax, double jerkmin, double accmax, double accmin, double velmax, double velmin, double thresh = 5.0);

			virtual ~SCurveProfile();

			/// \brief update the profile to cover the distance given
			/// This method must be called before any of the methods that return
			/// information about distance, velocity, or acceleration are called
			/// \param dist the distance to cover with the velocity profile
			/// \param start_velocity the starting velocity for the profile
			/// \param end_velocity the final velocity for the profile
			virtual void update(double dist, double start_velocity, double end_velocity);

			/// \brief return the acceleration for the profile at the given time
			/// If the time is prior to zero for the profile, the max acceleration value
			/// is returned.  If the time is past the end of the profile, the max deceleartion
			/// value is returned.
			/// \param t the time in question
			/// \returns the acceleration at the given point in time
			virtual double getAccel(double t) const;

			/// \brief return the velocity at the given point in time
			/// If the time is less than zero, the initial velocity is returned.  If the
			/// time exceeds the time of the profile, the final velocity is returned.
			/// \param t the time in question
			/// \returns the velocity at a given point in time
			virtual double getVelocity(double t) const;

			/// \brief return the distance at the given point in time
			/// If the time is less than zero, the zero is returned.  If the time 
			/// exceeds the time of the profile, then th e distance of the profile is
			/// returned.  Note, this is true even if the final velocity of the
			/// profile is non-zero as the profile has no knowledge of the change in
			/// velocity after the end of a profile.
			/// \param t the time in question
			/// \returns the velocity at a given point in time          
			virtual double getDistance(double t) const;

			/// \brief get the total time for the profile
			/// \returns total time for the profile
			virtual double getTotalTime() const;

			virtual double getTotalDistance() const;

			/// \brief return the starting velocity for the profile
			/// \returns starting velocity for the robot
			virtual double getStartVelocity() const;

			/// \brief return the end velocity for the profile
			/// \returns end velocity for the robot
			virtual double getEndVelocity() const;

		private:
			void rampUp();
			void rampDown();
			size_t findRegion(double t) const ;
			bool tryOne(double dist, double maxv);

		private:
			std::vector<double> j_;
			std::vector<double> t_;
			std::vector<double> a_;
			std::vector<double> v_;
			std::vector<double> p_;

			double jerkmax_;
			double jerkmin_;
			double accmin_;
			double accmax_;
			double velmin_;
			double thresh_;

			double step_;
		};
	}
}

