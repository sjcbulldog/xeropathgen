#include "XeroGenV2PathGenerator.h"
#include <SplinePair.h>
#include <TrajectoryUtils.h>
#include <DistanceView.h>
#include <RobotPath.h>

using namespace xero::paths;


XeroGenV2PathGenerator::XeroGenV2PathGenerator(double diststep, double timestep)
{
	diststep_ = diststep;
	timestep_ = timestep;
	maxDx_ = kMaxDX;
	maxDy_ = kMaxDY;
	maxDTheta_ = kMaxDTheta;
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
	// Step 4: now perform the walk down the path to find the velocities
	//

	return nullptr;
}

std::vector<std::shared_ptr<xero::paths::SplinePair>>
XeroGenV2PathGenerator::generateSplines(const std::vector<xero::paths::Pose2d>& points)
{
	RobotPath path(nullptr, "");
	for (const Pose2d& pt : points)
		path.addPoint(pt);
	path.generateSplines();
	return path.getSplines();
}
