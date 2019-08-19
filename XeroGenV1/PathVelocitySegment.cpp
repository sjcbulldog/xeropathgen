#include "PathVelocitySegment.h"
#include <SCurveProfile.h>
#include <TrapezoidalProfile.h>
#include <cmath>
#include <cassert>

using namespace xero::paths;

PathVelocitySegment::PathVelocitySegment(double start, double length, double vel)
{
	start_dist_ = start;
	length_ = length;
	velocity_ = vel;
}

PathVelocitySegment::~PathVelocitySegment()
{
}

void PathVelocitySegment::createProfile(bool scurve, double maxacc, double startvel, double endvel)
{
	if (scurve)
		profile_ = std::make_shared<SCurveProfile>();
	else
		profile_ = std::make_shared<TrapezoidalProfile>(maxacc, -maxacc, velocity_);

	profile_->update(length_, startvel, endvel);

	assert(std::fabs(profile_->getDistance(profile_->getTotalTime()) - length_) < 0.1);
}

bool PathVelocitySegment::startsSame(const PathVelocitySegment& seg)
{
	return std::fabs(start() - seg.start()) < kEpsilon;
}

bool PathVelocitySegment::endsSame(const PathVelocitySegment& seg)
{
	return std::fabs(end() - seg.end()) < kEpsilon;
}

bool PathVelocitySegment::contains(const PathVelocitySegment& seg)
{
	return seg.start() >= start() && seg.end() <= seg.end();
}

bool PathVelocitySegment::overlaps(const PathVelocitySegment& seg)
{
	if (seg.start() > start() && seg.start() < end())
		return true;

	if (seg.end() > start() && seg.end() < end())
		return true;

	return false;
}
