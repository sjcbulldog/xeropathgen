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
private:
	static constexpr double kMaxDX = 2.0; //inches
	static constexpr double kMaxDY = 0.05; //inches
	static constexpr double kMaxDTheta = 0.1; //radians!

public:
	XeroGenV1PathGenerator(double diststep, double timestep, bool scurve) {
		diststep_ = diststep;
		timestep_ = timestep;
		maxDx_ = kMaxDX;
		maxDy_ = kMaxDY;
		maxDTheta_ = kMaxDTheta;
		scurve_ = scurve;
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

private:
	double diststep_;
	double timestep_;
	double maxDx_;
	double maxDy_;
	double maxDTheta_;
	bool scurve_;
};

