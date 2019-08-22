#include "XeroGenV1PathGenerator.h"
#include "TrajectoryUtils.h"
#include "PathVelocitySegment.h"
#include "UnitConverter.h"
#include <TrajectoryNames.h>
#include <TrapezoidalProfile.h>
#include <SCurveProfile.h>
#include <RobotPath.h>
#include <DistanceView.h>
#include <DistanceVelocityConstraint.h>
#include <cassert>
#include <iostream>

using namespace xero::paths;

std::shared_ptr<xero::paths::PathTrajectory> 
XeroGenV1PathGenerator::generate(const std::vector<Pose2d>& points, const ConstraintCollection &constraints,
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
	// Step 4: apply the solver
	// 
	std::vector<Pose2dWithTrajectory> trajpts = generateTrajPoints(distview, constraints,  startvel, endvel, maxvel, maxaccel, maxjerk);

	return std::make_shared<PathTrajectory>(TrajectoryName::Main, trajpts);
}

std::vector<std::shared_ptr<xero::paths::SplinePair>> 
XeroGenV1PathGenerator::generateSplines(const std::vector<xero::paths::Pose2d>& points)
{
	RobotPath path(nullptr, "");
	for (const Pose2d& pt : points)
		path.addPoint(pt);
	path.generateSplines();
	return path.getSplines();
}

void XeroGenV1PathGenerator::getMinMaxVel(std::vector<PathVelocitySegment>& segs, size_t index, double& sv, double& ev, double startvel, double endvel)
{
	if (index != 0)
		sv = std::min(segs[index - 1].velocity(), segs[index].velocity());
	else
		sv = startvel;

	if (index != segs.size() - 1)
		ev = std::min(segs[index].velocity(), segs[index + 1].velocity());
	else
		ev = endvel;
}

bool XeroGenV1PathGenerator::adjustPrevious(std::vector<PathVelocitySegment>& segs, size_t index, double startvel, double endvel, double maxjerk, double maxaccel)
{
	if (index == 0)
		return false;

	double sv, ev;
	double newprev = segs[index - 1].velocity() - deltav_;

	while (newprev >= velmin_)
	{
		//
		// Reduce the end velocity of the previous segment
		//
		segs[index - 1].setVelocity(newprev);

		//
		// Run and see if the new end velocity works.  It could fail if the end velocity causes
		// the previous segment to not be achieivable because the gap from start velocity to 
		// end velocity is too great
		//
		getMinMaxVel(segs, index - 1, sv, ev, startvel, endvel);
		if (!segs[index - 1].createProfile(scurve_, maxjerk, maxaccel, sv, ev))
		{
			//
			// Recursively adjust the segment before the previous segment, all the way
			// back to the beginning if necessary.
			//
			if (!adjustPrevious(segs, index - 1, startvel, endvel, maxjerk, maxaccel))
				throw std::runtime_error("cannot find solution for scurve constraints");
		}

		//
		// Now run the current segment with the adjusted prev segment
		//
		getMinMaxVel(segs, index, sv, ev, startvel, endvel);
		if (segs[index].createProfile(scurve_, maxjerk, maxaccel, sv, ev))
			return true;

		//
		// Reduce the end velocity for the previous segment and try again
		//
		newprev -= velmin_;
	}

	return false;
}

std::vector<Pose2dWithTrajectory> 
XeroGenV1PathGenerator::generateTrajPoints(const DistanceView &distview, const ConstraintCollection& constraints, double startvel, double endvel,
											double maxvel, double maxaccel, double maxjerk)
{
	std::vector<Pose2dWithTrajectory> result;
	std::vector<PathVelocitySegment> segments;

	//
	// Create the first segment, which is start to finish with max velocity.  Constraints will
	// be added to break this segment up into smaller pieces
	//
	PathVelocitySegment s(0.0, distview.length(), maxvel);
	segments.push_back(s);

	//
	// Add the constraints, this breaks the segments into smaller pieces to consider the constraints
	//
	for(const auto &con: constraints)
	{ 
		auto distcon = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);
		if (distcon != nullptr)
		{
			if (distcon->getVelocity() == 0.0)
				continue;

			if (distcon->getAfter() >= distcon->getBefore())
				continue;

			PathVelocitySegment newseg(distcon->getAfter(), distcon->getBefore() - distcon->getAfter(), distcon->getVelocity());
			insertSegment(segments, newseg);
		}
	}

	//
	// Create the profile
	//
	double total = 0.0;
	for (size_t i = 0; i < segments.size(); i++)
	{
		double sv, ev;

		if (i == 0)
			sv = startvel;
		else
			sv = std::min(segments[i - 1].velocity(), segments[i].velocity());

		if (i == segments.size() - 1)
			ev = endvel;
		else
			ev = std::min(segments[i].velocity(), segments[i + 1].velocity());

		if (!segments[i].createProfile(scurve_, maxjerk, maxaccel, sv, ev))
		{
#ifdef NOTYET
			//
			// We cannot create this profile, we are over constrained.  This generally means the end 
			// of the last segment velocity is high enough that we cannot get down to the start velocity
			// of this segment.  See if we can lower the end velocity of the last segment.
			//
			if (!adjustPrevious(segments, i, startvel, endvel, maxjerk, maxaccel))
				throw std::runtime_error("cannot find solution for scurve constraints");
#else
			throw std::runtime_error("cannot find solution for scurve constraints");
#endif
		}
		total += segments[i].time();
	}

	for (size_t i = 0; i < segments.size(); i++)
	{
		PathVelocitySegment& seg = segments[i];
		std::cout << "Segment " << i;
		std::cout << ", start " << seg.start();
		std::cout << ", len " << seg.length();
		std::cout << ", velocity " << seg.velocity();
		std::cout << ", prdist " << seg.profile()->getTotalDistance();
		std::cout << ", prtime " << seg.profile()->getTotalTime();
		std::cout << std::endl;
	}

	size_t sindex = 0;
	double tstart = 0.0;
	double dstart = 0.0;
	double prevacc = 0.0;
	size_t count = 0;

	for (double t = 0; t <= total; t += timestep_)
	{
		auto profile = segments[sindex].profile();

		if (t - tstart > profile->getTotalTime())
		{
			//
			// We are transistioning between profiles
			//
			tstart += segments[sindex].profile()->getTotalTime();
			dstart += segments[sindex].profile()->getTotalDistance();
			sindex++;
			profile = segments[sindex].profile();
		}

		double dst = profile->getDistance(t - tstart) + dstart;
		double vel = profile->getVelocity(t - tstart);
		double acc = profile->getAccel(t - tstart);
		double jerk = (acc - prevacc) / timestep_;

		Pose2d pt = distview[dst];
		Pose2dWithTrajectory trajpt(pt, t, dst, vel, acc, jerk);
		result.push_back(trajpt);

		prevacc = acc;

		count++;
	}

	return result;
}

void XeroGenV1PathGenerator::insertSegment(std::vector<PathVelocitySegment>& segs, PathVelocitySegment &newseg)
{
	for (size_t i = 0; i < segs.size(); i++)
	{
		PathVelocitySegment& cur = segs[i];
		if (cur.overlaps(newseg))
		{
			if (cur.contains(newseg))
			{
				if (newseg.velocity() > cur.velocity())
					return;

				if (cur.startsSame(newseg))
				{
					auto it = segs.begin();
					std::advance(it, i);
					cur.setStart(newseg.end());
					segs.insert(it, newseg);
					return;
				}
				else if (cur.endsSame(newseg))
				{
					auto it = segs.begin();
					std::advance(it, i + 1);
					cur.setEnd(newseg.start());
					segs.insert(it, newseg);
					return;
				}
				else
				{
					double end = cur.end();
					double vel = cur.velocity();
					cur.setEnd(newseg.start());
					auto it = segs.begin();
					std::advance(it, i + 1);
					bool isend = (it == segs.end());

					segs.insert(it, newseg);
					PathVelocitySegment addseg(newseg.end(), end - newseg.end(), vel);
					if (isend)
						segs.push_back(addseg);
					else
					{
						it = segs.begin();
						std::advance(it, i + 2);
						segs.insert(it, addseg);
					}
					return;
				}
			}
			else
			{
				//
				// New segment overlaps the end of the cur segment
				//
				PathVelocitySegment aseg(newseg.start(), cur.end(), newseg.velocity());
				PathVelocitySegment bseg(cur.end(), newseg.end(), newseg.velocity());
				insertSegment(segs, aseg);
				insertSegment(segs, bseg);
				return;
			}
		}
	}
	assert(false);
}
