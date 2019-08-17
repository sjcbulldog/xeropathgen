#pragma once

#include <TrapezoidalProfile.h>
#include <memory>

class PathVelocitySegment
{
public:
	PathVelocitySegment(double start, double length, double vel);
	virtual ~PathVelocitySegment();

	double time() const { return profile_->getTotalTime(); }

	double start() const { return start_dist_; }
	void setStart(double v) { start_dist_ = v; }
	double length() const { return length_; }
	void setLength(double v) { length_ = v; }
	double end() const { return start_dist_ + length_; }
	void setEnd(double v) { length_ = v - start_dist_; }
	double velocity() const { return velocity_; }

	bool startsSame(const PathVelocitySegment& seg);
	bool endsSame(const PathVelocitySegment& seg);

	bool overlaps(const PathVelocitySegment&);
	bool contains(const PathVelocitySegment&);

	void createProfile(double maxacc, double startvel, double endvel);

	std::shared_ptr<xero::paths::TrapezoidalProfile> profile() { return profile_; }

private:
	static constexpr double kEpsilon = 1e-5;

private:
	double start_dist_;
	double length_;
	double velocity_;
	std::shared_ptr<xero::paths::TrapezoidalProfile> profile_;
};

