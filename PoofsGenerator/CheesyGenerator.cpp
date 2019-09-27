#include "CheesyGenerator.h"
#include <RobotPath.h>
#include <TrajectoryUtils.h>
#include <DistanceView.h>

using namespace xero::paths;

CheesyGenerator::CheesyGenerator(double diststep, double maxdx, double maxdy, double maxtheta)
{
	diststep_ = diststep;
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
CheesyGenerator::timeParameterize(const DistanceView& view)
{

}

std::shared_ptr<xero::paths::PathTrajectory> 
CheesyGenerator::generate(const std::vector<xero::paths::Pose2d>& waypoints, const xero::paths::ConstraintCollection& constraints,
	double startvel, double endvel, double maxvel, double maxaccel, double maxjerk)
{
	//
	// Step 1: generate a set of splines that represent the path
	//         (taken from the cheesy poofs code)
	//
	std::vector<std::shared_ptr<xero::paths::SplinePair>> splines = generateSplines(points);

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


	return nullptr;
}
