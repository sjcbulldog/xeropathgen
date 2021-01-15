#include "CheesyGenerator.h"
#include <RobotPath.h>
#include <TrajectoryUtils.h>
#include <DistanceView.h>
#include <Pose2dConstrained.h>
#include <PathTrajectory.h>
#include <TrajectoryNames.h>
#include <cmath>

using namespace xero::paths;

CheesyGenerator::CheesyGenerator(double diststep, double timestep, double maxdx, double maxdy, double maxtheta)
{
	diststep_ = diststep;
	timestep_ = timestep;
	maxDx_ = maxdx;
	maxDy_ = maxdy;
	maxDTheta_ = maxtheta;
}

CheesyGenerator::~CheesyGenerator()
{
}

std::vector<std::shared_ptr<xero::paths::SplinePair>>
CheesyGenerator::generateSplines(const std::vector<xero::paths::Pose2d>& points)
{
	RobotPath path(nullptr, "");
	for (const Pose2d& pt : points)
		path.addPoint(pt);
	path.generateSplines();
	return path.getSplines();
}

std::vector<xero::paths::Pose2dWithTrajectory>
CheesyGenerator::timeParameterize(const DistanceView& view, const xero::paths::ConstraintCollection& constraints, 
					double startvel, double endvel, double maxvel, double maxaccel)
{
	std::vector<xero::paths::Pose2dConstrained> points;
	Pose2dConstrained predecessor;
	const static double kEpsilon = 1e-6;

	predecessor.setPosition(0.0);
	predecessor.setPose(view[static_cast<size_t>(0)]);
	predecessor.setVelocity(startvel);
	predecessor.setAccelMin(-maxaccel);
	predecessor.setAccelMax(maxaccel);

	//
	// Forward pass
	//
	for (size_t i = 0; i < view.size(); i++)
	{
		Pose2dConstrained state;
		state.setPose(view[i]);
		state.setCurvature(view[i].curvature());
		state.setPosition(view.getPosition(i));

		double dist = predecessor.pose().distance(state.pose());
		state.setPosition(predecessor.position() + dist);

		while (true)
		{
			double calcvel = std::sqrt(predecessor.velocity() * predecessor.velocity() + 2.0 * predecessor.accelMax() * dist);
			state.setVelocity(std::min(maxvel, calcvel));

			if (std::isnan(state.velocity()))
				throw std::runtime_error("invalid maximum velocity");

			state.setAccelMin(-maxaccel);
			state.setAccelMax(maxaccel);

			for (auto constraint : constraints)
			{
				double convel = constraint->getMaxVelocity(state);
				state.setVelocity(std::min(state.velocity(), convel));
			}

			if (state.velocity() < 0.0)
				throw std::runtime_error("invalid maximum velocity - constraint set to negative");

			if (dist < kEpsilon)
				break;

			double actaccel = (state.velocity() * state.velocity() - predecessor.velocity() * predecessor.velocity()) / (2.0 * dist);
			if (state.accelMax() < actaccel - kEpsilon)
			{
				predecessor.setAccelMax(state.accelMax());
				if (i != 0)
					points[i - 1] = predecessor;
			}
			else
			{
				if (actaccel > predecessor.accelMin() + kEpsilon)
				{
					predecessor.setAccelMax(actaccel);
					if (i != 0)
						points[i - 1] = predecessor;
				}
				break;
			}
		}
		points.push_back(state);
		predecessor = state;
	}

	//
	// Backward pass
	//
	size_t last = view.size() - 1;
	Pose2dConstrained sucessor;
	sucessor.setPose(view[last]);
	sucessor.setPosition(points[last].position());
	sucessor.setVelocity(endvel);
	sucessor.setAccelMin(-maxaccel);
	sucessor.setAccelMax(maxaccel);

	for (size_t i = points.size() - 1; i < points.size(); i--)
	{
		Pose2dConstrained state = points[i];
		double dist = state.position() - sucessor.position();				// Will be negative

		while (true)
		{
			double newmaxvel = std::sqrt(sucessor.velocity() * sucessor.velocity() + 2.0 * sucessor.accelMin() * dist);
			if (newmaxvel >= state.velocity())
				break;

			if (std::isnan(newmaxvel))
				throw std::runtime_error("invalid new maximum velocity");

			state.setVelocity(newmaxvel);
			points[i] = state;

			if (dist > kEpsilon)
				break;

			double actaccel = (state.velocity() * state.velocity() - sucessor.velocity() * sucessor.velocity()) / (2.0 * dist);
			if (state.accelMin() > actaccel + kEpsilon)
			{
				sucessor.setAccelMin(state.accelMin());
				if (i != points.size() - 1)
					points[i + 1] = sucessor;
			}
			else
			{
				sucessor.setAccelMin(actaccel);
				if (i != points.size() - 1)
					points[i + 1] = sucessor;

				break;
			}
		}
		sucessor = state;
	}

	double t = 0.0;
	double s = 0.0;
	double v = 0.0;
	std::vector<xero::paths::Pose2dWithTrajectory> result;

	for (size_t i = 0; i < points.size(); i++)
	{
		const Pose2dConstrained& state = points[i];
		double ds = state.position() - s;
		double accel = (state.velocity() * state.velocity() - v * v) / (2.0 * ds);
		double dt = 0;
		if (i > 0)
		{
			result[i - 1].setAcceleration(accel);
			if (std::fabs(accel) > kEpsilon)
			{
				dt = (state.velocity() - v) / accel;
			}
			else
			{
				dt = ds / v;
			}
		}
		t += dt;
		if (std::isinf(t) || std::isnan(t))
			throw std::runtime_error("invalid time value integrating forward");

		v = state.velocity();
		s = state.position();

		Pose2dWithTrajectory trajpt(view[i], t, s, v, accel, 0.0);
		result.push_back(trajpt);
	}
	return result;
}

size_t CheesyGenerator::findIndex(std::vector<xero::paths::Pose2dWithTrajectory>& traj, double time)
{
	if (time < traj[0].time())
		return 0;

	if (time > traj[traj.size() - 1].time())
		return traj.size() - 1;

	size_t low = 0;
	size_t high = traj.size() - 1;

	while (high - low > 1)
	{
		size_t center = (high + low) / 2;
		if (time > traj[center].time())
		{
			low = center;
		}
		else
		{
			high = center;
		}
	}

	return low;
}

std::vector<xero::paths::Pose2dWithTrajectory> CheesyGenerator::convertToUniformTime(std::vector<xero::paths::Pose2dWithTrajectory>& traj, double step)
{
	std::vector<xero::paths::Pose2dWithTrajectory> result;

	for (double time = 0.0; time < traj[traj.size() - 1].time(); time += step)
	{
		Pose2dWithTrajectory newpt;

		size_t low = findIndex(traj, time);
		if (low == traj.size() - 1)
		{
			newpt = traj[traj.size() - 1];
		}
		else
		{
			double percent = (time - traj[low].time()) / (traj[low + 1].time() - traj[low].time());
			newpt = traj[low].interpolate(traj[low + 1], percent);
		}

		result.push_back(newpt);
	}

	return result;
}

std::shared_ptr<xero::paths::PathTrajectory> 
CheesyGenerator::generate(const std::vector<xero::paths::Pose2d>& waypoints, const xero::paths::ConstraintCollection& constraints,
	double startvel, double endvel, double maxvel, double maxaccel, double maxjerk)
{
	//
	// Step 1: generate a set of splines that represent the path
	//         (taken from the cheesy poofs code)
	//
	std::vector<std::shared_ptr<xero::paths::SplinePair>> splines = generateSplines(waypoints);

	//
	// Step 2: generate a set of points that represent the path where the curvature, x, and y do not 
	//         differ to an amount greater than maxDx_, maxDy_, maxDTheta_
	//         (taken from the cheesy poofs code)
	//
	std::vector<Pose2d> paramtraj = TrajectoryUtils::parameterize(splines, maxDx_, maxDy_, maxDTheta_);

	//
	// Step 3: generate a set of points that are equi-distant apart (diststep_).
	//
	DistanceView distview(paramtraj, diststep_);

	//
	// Step 4: generate a timing view that meets the constraints of the system
	//
	std::vector<xero::paths::Pose2dWithTrajectory> pts = timeParameterize(distview, constraints, startvel, endvel, maxvel, maxaccel);

	//
	// Step 5: convert the timeview view to a uniform timeing view
	//
	std::vector<xero::paths::Pose2dWithTrajectory> uniform = convertToUniformTime(pts, timestep_);

	//
	// Return a trajectory
	//
	return std::make_shared<PathTrajectory>(TrajectoryName::Main, uniform);
}

