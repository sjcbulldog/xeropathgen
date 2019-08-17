#pragma once

#include <string>
#include <vector>

/// \file

namespace xero {
    namespace paths {
        /// \brief This class calculates and tracks a trapezoidal velocity profile
        /// A trapezoidal velocity profile covers a desired distance by following a motion profile of
        /// accelerating to a crusing velocity, crusing at this fixed velocity, and decelerating to 
        /// this the descired end velocity at the desired distance.
        /// \sa https://hackaday.io/project/5765-flexsea-wearable-robotics-toolkit/log/24796-trajectory-generation-trapezoidal-speed-profile
        class TrapezoidalProfile {
        public:
            /// \brief Create a new profile with the given acceleration, deceleration, and max velocity
            /// \param accel the acceleration to use any time the velocity needs to increase
            /// \param decel the deceleration to use any time the velocity needs to decrease
            /// \param max_velocity the magnitude of the maximum velocity allowed
            TrapezoidalProfile(double accel, double decel, double max_velocity) ;

            /// \brief destroy the velocity profile object
            virtual ~TrapezoidalProfile() ;

            /// \brief update the profile to cover the distance given
            /// This method must be called before any of the methods that return
            /// information about distance, velocity, or acceleration are called
            /// \param dist the distance to cover with the velocity profile
            /// \param start_velocity the starting velocity for the profile
            /// \param end_velocity the final velocity for the profile
            void update(double dist, double start_velocity, double end_velocity) ;

            /// \brief return the acceleration for the profile at the given time
            /// If the time is prior to zero for the profile, the max acceleration value
            /// is returned.  If the time is past the end of the profile, the max deceleartion
            /// value is returned.
            /// \param t the time in question
            /// \returns the acceleration at the given point in time
            double getAccel(double t) const ;

            /// \brief return the velocity at the given point in time
            /// If the time is less than zero, the initial velocity is returned.  If the
            /// time exceeds the time of the profile, the final velocity is returned.
            /// \param t the time in question
            /// \returns the velocity at a given point in time
            double getVelocity(double t) const ;

            /// \brief return the distance at the given point in time
            /// If the time is less than zero, the zero is returned.  If the time 
            /// exceeds the time of the profile, then th e distance of the profile is
            /// returned.  Note, this is true even if the final velocity of the
            /// profile is non-zero as the profile has no knowledge of the change in
            /// velocity after the end of a profile.
            /// \param t the time in question
            /// \returns the velocity at a given point in time          
            double getDistance(double t) const ;

            /// \brief convert the profile to a human readable string
            /// \returns a human readable string
            std::string toString() ;

            /// \brief get the amount of time in the acceleration phase of the profile
            /// \returns time accelerating
            double getTimeAccel() const {
                return ta_ ;
            }

            /// \brief get the amount of time in the cruising phase of the profile (no acceleration)
            /// \returns time cruising
            double getTimeCruise() const {
                return tc_ ;
            }

            /// \brief get the amount of time in the deceleration phase of the profile
            /// \returns time deceleration
            double getTimeDecel() const {
                return td_ ;
            }

            /// \brief get the total time for the profile
            /// \returns total time for the profile
            double getTotalTime() const {
                return ta_ + tc_ + td_ ;
            }

			double getTotalDistance() const {
				return getDistance(getTotalTime());
			}

            /// \brief get the actual maximum velocity of this profile
            /// If the distance is small enough, the profile will never reach the max
            /// velocity specified when the profile was created.  This method returns the
            /// actual maximum velocity for the current profile.
            /// \returns actual maximum velocity of this profile
            double getActualMaxVelocity() const {
                return isneg_ ? -actual_max_velocity_ : actual_max_velocity_ ;
            }

            /// \brief return the time when the given distance is reached
            /// If the distance given exceeds the distance of the profile, the time for the
            /// profile is given.  If the distance given is less than zero, the a negative
            /// time value may be returned.  This is useful for time machine robots.
            /// \param dist the distance of interest
            /// \returns the time when the distance given is reached
            double getTimeForDistance(double dist) const ;

            /// \brief return the starting velocity for the profile
            /// \returns starting velocity for the robot
            double getStartVelocity() const {
                return start_velocity_ ;
            }

            /// \brief return the end velocity for the profile
            /// \returns end velocity for the robot
            double getEndVelocity() const {
                return end_velocity_ ;
            }

        private:
            double pickRoot(const std::vector<double> &roots) const;

        private:
            bool isneg_ ;
            
            double max_accel_ ;
            double max_decel_ ;
            double max_velocity_ ;
            double distance_ ;

            double actual_max_velocity_ ;
            double start_velocity_ ;
            double end_velocity_ ;

            double ta_ ;
            double td_ ;
            double tc_ ;

            std::string type_ ;
        } ;
    }
}