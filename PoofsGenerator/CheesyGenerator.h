#pragma once

#include <PathConstraint.h>
#include <PathTrajectory.h>
#include <Pose2d.h>
#include <vector>

class CheesyGenerator
{
public:
	CheesyGenerator();
	virtual ~CheesyGenerator();

	std::shared_ptr<xero::paths::PathTrajectory> generate(const std::vector<xero::paths::Pose2d>& waypoints, const xero::paths::ConstraintCollection& constraints,
		double startvel, double endvel, double maxvel, double maxaccel, double maxjerk);

private:
};

