#pragma once

#include <TrapezoidalProfile.h>
#include <memory>

class PathVelocitySegment
{
public:
	PathVelocitySegment(double start, double length, double vel, double velmin = 10.0);
	virtual ~PathVelocitySegment();

	double time() const { return profile_->getTotalTime(); }

	double start() const { return start_dist_; }
	void setStart(double v) { start_dist_ = v; }
	double length() const { return length_; }
	void setLength(double v) { length_ = v; }
	double end() const { return start_dist_ + length_; }
	void setEnd(double v) { length_ = v - start_dist_; }
	double velocity() const { return velocity_; }
	void setVelocity(double v) { velocity_ = v; }

	bool startsSame(const PathVelocitySegment& seg);
	bool endsSame(const PathVelocitySegment& seg);

	bool overlaps(const PathVelocitySegment&);
	bool contains(const PathVelocitySegment&);

	bool createProfile(bool scurve, double maxjerk, double maxacc, double startvel, double endvel);

	std::shared_ptr<xero::paths::SpeedProfileGenerator> profile() { return profile_; }
	std::shared_ptr<const xero::paths::SpeedProfileGenerator> profile() const { return profile_; }

private:
	static constexpr double kEpsilon = 1e-5;

private:
	double start_dist_;
	double length_;
	double velocity_;
	double velmin_;
	std::shared_ptr<xero::paths::SpeedProfileGenerator> profile_;
};

