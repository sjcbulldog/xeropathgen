#pragma once

#include <PathConstraint.h>
#include <PathTrajectory.h>
#include <SplinePair.h>
#include <memory>

class XeroGenV2PathGenerator
{
private:
	static constexpr double kMaxDX = 2.0; //inches
	static constexpr double kMaxDY = 0.05; //inches
	static constexpr double kMaxDTheta = 0.1; //radians!
	
public:
	XeroGenV2PathGenerator(double timestep, double diststep);
	virtual ~XeroGenV2PathGenerator();

	std::shared_ptr<xero::paths::PathTrajectory> generate(const std::vector<xero::paths::Pose2d>& points,
		const xero::paths::ConstraintCollection& constraints,
		double startvel, double endvel, double maxvel,
		double maxaccel, double maxjerk);

private:
	std::vector<std::shared_ptr<xero::paths::SplinePair>> generateSplines(const std::vector<xero::paths::Pose2d>& points);

private:
	double timestep_;
	double diststep_;
	double maxDx_;
	double maxDy_;
	double maxDTheta_;
};

