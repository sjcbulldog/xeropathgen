#pragma once

#include <PathConstraint.h>
#include <PathTrajectory.h>
#include <memory>

class XeroGenV2PathGenerator
{
public:
	XeroGenV2PathGenerator(double timestep);
	virtual ~XeroGenV2PathGenerator();

	std::shared_ptr<xero::paths::PathTrajectory> generate(const std::vector<xero::paths::Pose2d>& points,
		const xero::paths::ConstraintCollection& constraints,
		double startvel, double endvel, double maxvel,
		double maxaccel, double maxjerk);

private:
	double sumDistance(const std::vector<xero::paths::Pose2d>& points) {
		return 0.0;
	}

private:
	double timestep_;
};

