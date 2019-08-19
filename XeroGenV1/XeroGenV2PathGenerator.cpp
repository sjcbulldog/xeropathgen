#include "XeroGenV2PathGenerator.h"

XeroGenV2PathGenerator::XeroGenV2PathGenerator(double timestep)
{
	timestep_ = timestep;
}

XeroGenV2PathGenerator::~XeroGenV2PathGenerator()
{
}

std::shared_ptr<xero::paths::PathTrajectory> XeroGenV2PathGenerator::generate(const std::vector<xero::paths::Pose2d>& points,
	                    const xero::paths::ConstraintCollection& constraints, double startvel, double endvel, double maxvel,
	                    double maxaccel, double maxjerk)
{
	double t = 0;
	double dist = 0;
	double totaldist = sumDistance(points);

	return nullptr;
}