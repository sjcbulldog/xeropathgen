#include "TimingUtil.h"
#include "Trajectory.h"
#include "ConstrainedPoint.h"
#include <cmath>
#include <algorithm>
#include <cassert>

namespace xero {
	namespace paths {

		TimedTrajectory TimingUtil::createConstantTimeTrajectory(const TimedTrajectory& traj, double timestep)
		{
			const std::vector<TimedTrajectoryPoint>& pts = traj.getPoints();
			double endtime = pts[pts.size() - 1].getTime();
			std::vector<TimedTrajectoryPoint> newpts;
			size_t previndex = 0;

			if (traj.getPoints().size() == 1)
			{
				TimedTrajectoryPoint pt = pts[0].interpolateByTime(pts[0], 0.0);
				newpts.push_back(pt);
			}
			else
			{
				for (double t = 0.0; t <= endtime; t += timestep)
				{
					if (t < pts[0].getTime())
					{
						newpts.push_back(pts[0]);
					}
					else if (t > endtime)
					{
						newpts.push_back(pts[pts.size() - 1]);
					}
					else
					{
						while (previndex < pts.size() - 1)
						{
							if (t >= pts[previndex].getTime() && t < pts[previndex + 1].getTime())
								break;

							previndex++;
						}

						assert(previndex != pts.size());
						newpts.push_back(pts[previndex].interpolateByTime(pts[previndex + 1], t));
					}
				}
			}

			return TimedTrajectory(newpts);
		}

		TimedTrajectory TimingUtil::timeParameterizeTrajectory(DistanceView& distview, const ConstraintCollection &constraints, double step_size,
			double startvel, double endvel, double maxvel, double maxaccel)
		{
			size_t num_states = static_cast<size_t>(std::ceil(distview.length() / step_size)) + 1 ;
			std::vector<TrajectorySamplePoint> points;

			for (size_t i = 0; i < num_states; i++)
			{
				double t = std::min(i * step_size, distview.length());
				TrajectorySamplePoint pt = distview.sample(t);
				points.push_back(pt);
			}

			return timeParameterizeTrajectory(points, constraints, startvel, endvel, maxvel, maxaccel);
		}

		TimedTrajectory TimingUtil::timeParameterizeTrajectory(std::vector<TrajectorySamplePoint> &states, const ConstraintCollection& constraints,
			double startvel, double endvel, double maxvel, double maxaccel)
		{
			std::vector<ConstrainedPoint> constrained_states;
			ConstrainedPoint pred(&states[0], 0.0, startvel, -maxaccel, maxaccel);

			//
			// Forward pass
			//
			for (size_t i = 0; i < states.size(); i++)
			{
				ConstrainedPoint cs(&states[i]);
				double ds = cs.getState()->distance(*pred.getState());
				cs.setDistance(pred.getDistance() + ds);

				while (true)
				{
					double calcvel = std::sqrt(pred.getMaxVelocity() * pred.getMaxVelocity() + 2.0 * pred.getMaxAccel() * ds);
					double mxv = std::min(maxvel, calcvel);
					assert(!std::isnan(mxv));

					cs.setMaxVelocity(mxv);
					cs.setMaxAccel(maxaccel);
					cs.setMinAccel(-maxaccel);
					Pose2dWithTrajectory traj(cs.getState()->getPose(), 0.0, cs.getDistance(), 0.0, 0.0, 0.0);

					//
					// See if any of the constraints cause us to reduce the velocity
					//
					for (auto constraint : constraints)
					{	
						double vel = std::min(constraint->getMaxVelocity(traj), cs.getMaxVelocity());
						cs.setMaxVelocity(vel);
					}
					assert(cs.getMaxVelocity() >= 0.0);

					//
					// See if there are any acceleration based constraints
					//
					for (auto constraint : constraints)
					{
						double vel = cs.getMaxVelocity();
						MinMaxAcceleration acc = constraint->getMinMacAccel(traj, vel);
						assert(acc.isValid());

						double tmp = std::max(cs.getMinAccel(), acc.getMinAccel());
						cs.setMinAccel(tmp);

						tmp = std::min(cs.getMaxAccel(), acc.getMaxAccel());
						cs.setMaxAccel(tmp);
					}
					assert(cs.getMaxAccel() >= cs.getMinAccel());

					if (ds < kEpsilon)
						break;

					double actual_acceleration = (cs.getMaxVelocity() * cs.getMaxVelocity() - pred.getMaxVelocity() * pred.getMaxVelocity()) / (2.0 * ds);
					if (cs.getMaxAccel() < actual_acceleration - kEpsilon) {
						pred.setMaxAccel(cs.getMaxAccel());
						if (i != 0)
							constrained_states[i - 1] = pred;
					}
					else {
						if (actual_acceleration > pred.getMinAccel() + kEpsilon)
						{
							pred.setMaxAccel(actual_acceleration);
							if (i != 0)
								constrained_states[i - 1] = pred;
						}
						break;
					}
				}
				constrained_states.push_back(cs);
				pred = cs;
			}

			// Backward pass
			ConstrainedPoint succ(&states[states.size() - 1], constrained_states[states.size() - 1].getDistance(), startvel, -maxaccel, maxaccel);
			for (size_t i = states.size() - 1; i < states.size(); --i)		// Counting down, when unsigned int wraps, we are done
			{
				ConstrainedPoint& cs = constrained_states[i];
				double ds = cs.getDistance() - succ.getDistance();

				while (true)
				{
					// Enforce reverse max reachable velocity limit.
					// vf = sqrt(vi^2 + 2*a*d), where vi = successor.
					double new_max_velocity = std::sqrt(succ.getMaxVelocity() * succ.getMaxVelocity() + 2.0 * succ.getMinAccel() * ds);
					if (new_max_velocity >= cs.getMaxVelocity())
						break;

					if (std::isnan(new_max_velocity))
					{
						new_max_velocity = 0.0;
					}
					assert(!std::isnan(new_max_velocity));
					cs.setMaxVelocity(new_max_velocity);
					Pose2dWithTrajectory traj(cs.getState()->getPose(), 0.0, cs.getDistance(), 0.0, 0.0, 0.0);

					//
					// See if there are any acceleration based constraints
					//
					for (auto constraint : constraints)
					{
						double vel = cs.getMaxVelocity();
						MinMaxAcceleration acc = constraint->getMinMacAccel(traj, vel);
						assert(acc.isValid());

						double tmp = std::max(cs.getMinAccel(), acc.getMinAccel());
						cs.setMinAccel(tmp);

						tmp = std::min(cs.getMaxAccel(), acc.getMaxAccel());
						cs.setMaxAccel(tmp);
					}
					assert(cs.getMaxAccel() >= cs.getMinAccel());

					if (ds > kEpsilon) {
						break;
					}

					double actual_acceleration = (cs.getMaxVelocity() * cs.getMaxVelocity() - succ.getMaxVelocity() * succ.getMaxVelocity()) / (2.0 * ds);
					if (isnan(actual_acceleration))
						exit(99);

					if (cs.getMinAccel() > actual_acceleration + kEpsilon) {
						succ.setMinAccel(cs.getMinAccel());
						if (i != constrained_states.size() - 1)
							constrained_states[i + 1] = succ;
					}
					else {
						succ.setMinAccel(actual_acceleration);
						if (i != constrained_states.size() - 1)
							constrained_states[i + 1] = succ;
						break;
					}
				}
				succ = cs;
			}

			// Integrate the constrained states forward in time to obtain the TimedStates
			std::vector<TimedTrajectoryPoint> timed;
			double t = 0.0;
			double s = 0.0;
			double v = 0.0;
			double aprev = 0.0;
			double j = 0.0;
			for (size_t i = 0; i < states.size(); i++)
			{
				const ConstrainedPoint& cs = constrained_states[i];
				double ds = cs.getDistance() - s;
				double accel = (cs.getMaxVelocity() * cs.getMaxVelocity() - v * v) / (2.0 * ds);
				double dt = 0.0;
				if (i > 0)
				{
					timed[i - 1].setAcceleration(accel);
					if (std::abs(accel) > kEpsilon)
					{
						dt = (cs.getMaxVelocity() - v) / accel;
					}
					else
					{
						dt = ds / v;
					}
				}
				t += dt;
				v = cs.getMaxVelocity();
				s = cs.getDistance();
				j = (accel - aprev) / dt;

				TimedTrajectoryPoint tpt(*cs.getState(), t, s, v, accel, 0.0);
				timed.push_back(tpt);

				aprev = accel;
			}

			return TimedTrajectory(timed);
		}
	}
}