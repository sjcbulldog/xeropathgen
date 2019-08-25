#pragma once

#include "PathVelocitySegment.h"
#include <SplinePair.h>
#include <Pose2d.h>
#include <PathTrajectory.h>
#include <DistanceView.h>
#include <PathConstraint.h>
#include <TrapezoidalProfile.h>
#include <memory>
#include <vector>

class XeroGenV1PathGenerator
{
public:
	XeroGenV1PathGenerator(double diststep, double timestep, bool scurve, double maxdx, double maxdy, double maxtheta, double deltav, double velmin) {
		diststep_ = diststep;
		timestep_ = timestep;
		maxDx_ = maxdx;
		maxDy_ = maxdy;
		maxDTheta_ = maxtheta;
		scurve_ = scurve;
		deltav_ = deltav;
		velmin_ = velmin;
	}

	std::shared_ptr<xero::paths::PathTrajectory> generate(const std::vector<xero::paths::Pose2d>& points, 
														  const xero::paths::ConstraintCollection &constraints,
														  double startvel, double endvel, double maxvel, 
														  double maxaccel, double maxjerk);

private:
	std::vector<std::shared_ptr<xero::paths::SplinePair>> generateSplines(const std::vector<xero::paths::Pose2d>& points);

	std::vector<xero::paths::Pose2dWithTrajectory> generateTrajPoints(const xero::paths::DistanceView &distview, 
			const xero::paths::ConstraintCollection& constraints, double startvel, double endvel, double maxvel, double maxaccel, double maxjerk);

	void insertSegment(std::vector<PathVelocitySegment>& segs, PathVelocitySegment &newseg);
	bool adjustPrevious(std::vector<PathVelocitySegment> &segs, size_t i, double startvel, double endvel, double maxjerk, double maxaccel);
	void getMinMaxVel(std::vector<PathVelocitySegment>& segs, size_t i, double& sv, double& ev, double startvel, double endvel);

	void dumpSegments(const std::string &title, std::vector<PathVelocitySegment>& segs);

private:
	double diststep_;
	double timestep_;
	double maxDx_;
	double maxDy_;
	double maxDTheta_;
	bool scurve_;
	double deltav_;
	double velmin_;
};

