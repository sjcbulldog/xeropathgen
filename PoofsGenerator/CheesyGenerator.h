#pragma once

#include <PathConstraint.h>
#include <PathTrajectory.h>
#include <Pose2d.h>
#include <SplinePair.h>
#include <DistanceView.h>
#include <vector>

class CheesyGenerator
{
public:
	CheesyGenerator(double diststep, double timestep, double maxdx, double maxdy, double maxtheta);
	virtual ~CheesyGenerator();

	std::shared_ptr<xero::paths::PathTrajectory> generate(const std::vector<xero::paths::Pose2d>& waypoints, const xero::paths::ConstraintCollection& constraints,
		double startvel, double endvel, double maxvel, double maxaccel, double maxjerk);

private:
	std::vector<std::shared_ptr<xero::paths::SplinePair>> generateSplines(const std::vector<xero::paths::Pose2d>& points);
	std::vector<xero::paths::Pose2dWithTrajectory> timeParameterize(const xero::paths::DistanceView& view, const xero::paths::ConstraintCollection& constraints, 
					double startvel, double endvel, double maxvel, double maxaccel);

	std::vector<xero::paths::Pose2dWithTrajectory> convertToUniformTime(std::vector<xero::paths::Pose2dWithTrajectory>& traj, double step);
	size_t findIndex(std::vector<xero::paths::Pose2dWithTrajectory>& traj, double time);

private:
	double maxDx_;
	double maxDy_;
	double maxDTheta_;
	double diststep_;
	double timestep_;
};

