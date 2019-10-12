#include "PathVelocitySegment.h"
#include <SCurveProfile.h>
#include <TrapezoidalProfile.h>
#include <cmath>
#include <cassert>

using namespace xero::paths;

PathVelocitySegment::PathVelocitySegment(double start, double length, double vel, double velmin)
{
	start_dist_ = start;
	length_ = length;
	velocity_ = vel;
	velmin_ = velmin;
}

PathVelocitySegment::~PathVelocitySegment()
{
}

bool PathVelocitySegment::createProfile(bool scurve, double maxjerk, double maxacc, double startvel, double endvel)
{
	bool ret = true;

	if (scurve)
	{
		profile_ = std::make_shared<SCurveProfile>(maxjerk, -maxjerk, maxacc, -maxacc, velocity_, velmin_);
	}
	else
		profile_ = std::make_shared<TrapezoidalProfile>(maxacc, -maxacc, velocity_);

	try {
		profile_->update(length_, startvel, endvel);
	}
	catch (...)
	{
		ret = false;
	}

	assert(ret == false || std::fabs(profile_->getDistance(profile_->getTotalTime()) - length_) < 0.1);
	return ret;
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
	if (seg.start() >= start() && seg.start() <= end())
		return true;

	if (seg.end() >= start() && seg.end() <= end())
		return true;

	return false;
}
