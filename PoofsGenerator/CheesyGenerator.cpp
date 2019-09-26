#include "CheesyGenerator.h"

CheesyGenerator::CheesyGenerator()
{
}

CheesyGenerator::~CheesyGenerator()
{
}

std::shared_ptr<xero::paths::PathTrajectory> CheesyGenerator::generate(const std::vector<xero::paths::Pose2d>& waypoints, const xero::paths::ConstraintCollection& constraints,
	double startvel, double endvel, double maxvel, double maxaccel, double maxjerk)
{
	return nullptr;
}
