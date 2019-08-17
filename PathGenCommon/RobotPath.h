#pragma once

#include "Pose2d.h"
#include "PathConstraint.h"
#include "PathBase.h"
#include "UnitConverter.h"
#include "PathTrajectory.h"
#include "SplinePair.h"
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <algorithm>

namespace xero
{
	namespace paths
	{
		class PathGroup;

		class RobotPath : public PathBase
		{
		public:
			static constexpr const char* TimeTag = "time";
			static constexpr const char* GroupsTag = "groups";
			static constexpr const char* PositionTag = "position";
			static constexpr const char* AccelerationTag = "acceleration";
			static constexpr const char* JerkTag = "jerk";
			static constexpr const wchar_t* GroupsTagW = L"groups";
			static constexpr const char* NameTag = "name";
			static constexpr const wchar_t* NameTagW = L"name";
			static constexpr const char* PathsTag = "paths";
			static constexpr const wchar_t* PathsTagW = L"paths";
			static constexpr const char* StartVelocityTag = "startvelocity";
			static constexpr const wchar_t* StartVelocityTagW = L"startvelocity";
			static constexpr const char* EndVelocityTag = "endvelocity";
			static constexpr const wchar_t* EndVelocityTagW = L"endvelocity";
			static constexpr const char* StartAngleTag = "startangle";
			static constexpr const char* StartAngleDelayTag = "startangledelay";
			static constexpr const char* EndAngleTag = "endangle";
			static constexpr const char* EndAngleDelayTag = "endangledelay";
			static constexpr const char* MaxVelocityTag = "maxvelocity";
			static constexpr const wchar_t* MaxVelocityTagW = L"maxvelocity";
			static constexpr const char* MaxAccelerationTag = "maxacceleration";
			static constexpr const wchar_t* MaxAccelerationTagW = L"maxacceleration";
			static constexpr const char* MaxJerkTag = "maxjerk";
			static constexpr const wchar_t* MaxJerkTagW = L"maxjerk";
			static constexpr const char* ConstraintsTag = "constraints";
			static constexpr const wchar_t* ConstraintsTagW = L"constraints";
			static constexpr const char* TypeTag = "type";
			static constexpr const wchar_t* TypeTagW = L"type";
			static constexpr const char* DistanceVelocityTag = "distance_velocity";
			static constexpr const char* BeforeTag = "before";
			static constexpr const wchar_t* BeforeTagW = L"before";
			static constexpr const char* AfterTag = "after";
			static constexpr const wchar_t* AfterTagW = L"after";
			static constexpr const char* VelocityTag = "velocity";
			static constexpr const wchar_t* VelocityTagW = L"velocity";
			static constexpr const char* PointsTag = "points";
			static constexpr const wchar_t* PointsTagW = L"points";
			static constexpr const char* XTag = "x";
			static constexpr const wchar_t* XTagW = L"x";
			static constexpr const char* YTag = "y";
			static constexpr const wchar_t* YTagW = L"y";
			static constexpr const char* HeadingTag = "heading";
			static constexpr const wchar_t* HeadingTagW = L"heading";

		public:
			RobotPath(std::shared_ptr<PathGroup> parent, const std::string& name) {
				name_ = name;
				start_vel_ = 0.0;
				end_vel_ = 0.0;
				max_vel_ = 0.0;
				max_accel_ = 0.0;
				max_jerk_ = 0.0;
				start_angle_ = 0.0;
				start_angle_delay_ = 0.0;
				end_angle_ = 0.0;
				end_angle_delay_ = 0.0;
				parent_ = parent;
				impossible_ = false;
			}

			virtual ~RobotPath() {
			}

			bool getImpossible() const {
				return impossible_;
			}

			void setImpossible(bool b) {
				impossible_ = b;
			}

			double getTime();

			void addTrajectory(std::shared_ptr<PathTrajectory> newtraj) {
				trajectory_lock_.lock();
				auto it = std::find_if(trajectories_.begin(), trajectories_.end(), [newtraj](auto traj) { return traj->name() == newtraj->name(); });
				if (it != trajectories_.end())
					trajectories_.erase(it);
				trajectories_.push_back(newtraj);
				trajectory_lock_.unlock();
			}

			bool hasTrajectory(const std::string& name) {
				bool ret = false;

				trajectory_lock_.lock();
				auto it = std::find_if(trajectories_.begin(), trajectories_.end(), [name](auto traj) { return traj->name() == name; });
				ret = (it != trajectories_.end());
				trajectory_lock_.unlock();

				return ret;
			}

			std::shared_ptr<PathTrajectory> getTrajectory(const std::string& name) {
				std::shared_ptr<PathTrajectory> traj;

				trajectory_lock_.lock();
				auto it = std::find_if(trajectories_.begin(), trajectories_.end(), [name](auto traj) { return traj->name() == name; });
				if (it != trajectories_.end())
					traj = *it;
				trajectory_lock_.unlock();

				return traj;
			}

			void clearTrajectories() {
				trajectory_lock_.lock();
				trajectories_.clear();
				trajectory_lock_.unlock();
			}

			std::vector<std::string> getTrajectoryNames() {
				std::vector<std::string> names;

				trajectory_lock_.lock();
				for (auto traj : trajectories_)
					names.push_back(traj->name());
				trajectory_lock_.unlock();

				return names;
			}

			std::shared_ptr<PathGroup> getParent() {
				return parent_;
			}

			const std::string& getName() const {
				return name_;
			}

			virtual void setName(const std::string& name) {
				name_ = name;
			}

			void addPoint(const Pose2d& pt) {
				points_.push_back(pt);
				distances_.clear();
			}

			void removePoint(size_t index) {
				if (index < size() - 1 && index != 0)
				{
					points_.erase(points_.begin() + index);
					clearSplines();
					generateSplines();
					distances_.clear();
				}
			}

			void insertPoint(size_t after, const Pose2d& pt)
			{
				if (after < size() - 1)
				{
					points_.insert(points_.begin() + after + 1, pt);
					clearSplines();
					generateSplines();
					distances_.clear();
				}
			}

			void replacePoint(size_t index, const Pose2d& pd)
			{
				if (index < size())
				{
					points_[index] = pd;
					clearSplines();
					generateSplines();
					distances_.clear();
				}
			}

			size_t size() const {
				return points_.size();
			}

			const Pose2d& operator[](size_t index) const {
				return points_[index];
			}

			const std::vector<Pose2d>& getPoints() const {
				return points_;
			}

			void addTimingConstraint(std::shared_ptr<PathConstraint> cstr) {
				constraints_.push_back(cstr);
			}

			void removeTimingConstraint(size_t index) {
				if (index < constraints_.size())
				{
					auto it = constraints_.begin();
					std::advance(it, index);
					constraints_.erase(it);
				}
			}

			const ConstraintCollection& getConstraints() const {
				return constraints_;
			}

			double getStartVelocity() const {
				return start_vel_;
			}

			void setStartVelocity(double v) {
				start_vel_ = v;
			}

			double getEndVelocity() const {
				return end_vel_;
			}

			void setEndVelocity(double v) {
				end_vel_ = v;
			}

			double getStartAngle() const {
				return start_angle_;
			}

			void setStartAngle(double v) {
				start_angle_ = v;
			}

			double getStartAngleDelay() const {
				return start_angle_delay_;
			}

			void setStartAngleDelay(double v) {
				start_angle_delay_ = v;
			}

			double getEndAngle() const {
				return end_angle_;
			}

			void setEndAngle(double v) {
				end_angle_ = v;
			}

			double getEndAngleDelay() const {
				return end_angle_delay_;
			}

			void setEndAngleDelay(double v) {
				end_angle_delay_ = v;
			}

			double getMaxVelocity() const {
				return max_vel_;
			}

			void setMaxVelocity(double v) {
				max_vel_ = v;
			}

			double getMaxAccel() const {
				return max_accel_;
			}

			void setMaxAccel(double a) {
				max_accel_ = a;
			}

			double getMaxJerk() const {
				return max_jerk_;
			}

			void setMaxJerk(double j) {
				max_jerk_ = j;
			}

			void clear() {
				parent_ = nullptr;
				points_.clear();
			}

			void generateSplines();

			void clearSplines() {
				splines_.clear();
			}

			bool hasSplines() {
				return splines_.size() > 0;
			}

			const std::vector<std::shared_ptr<xero::paths::SplinePair>> &getSplines() {
				return splines_;
			}

			void convert(const std::string& old, const std::string& newunits) {
				clearTrajectories();
				clearSplines();

				start_vel_ = UnitConverter::convert(start_vel_, old, newunits);
				end_vel_ = UnitConverter::convert(end_vel_, old, newunits);
				max_vel_ = UnitConverter::convert(max_vel_, old, newunits);
				max_accel_ = UnitConverter::convert(max_accel_, old, newunits);
				max_jerk_ = UnitConverter::convert(max_jerk_, old, newunits);

				std::vector<Pose2d> newpts;
				for (size_t i = 0; i < points_.size(); i++) {
					
					double x = UnitConverter::convert(points_[i].getTranslation().getX(), old, newunits); 
					double y = UnitConverter::convert(points_[i].getTranslation().getY(), old, newunits);

					Translation2d t2d(x, y);
					Pose2d p2d(t2d, points_[i].getRotation());

					newpts.push_back(p2d);
				}
				points_ = newpts;

				for (auto c : constraints_)
					c->convert(old, newunits);

				generateSplines();
				distances_.clear();
			}

			double getDistance();
			double getDistance(size_t waypoint) {
				if (distances_.size() == 0)
					getDistance();

				if (waypoint == 0)
					return 0.0;
				
				return distances_[waypoint - 1];
			}

			bool getPoseAtTime(double time, Pose2dWithTrajectory& p2d);
			bool getDistance(double time, double& value);

			double sumDCurvature2();

		private:
			double optimize();
			void runOptimizationIteration();
			static double fitParabola(Translation2d p1, Translation2d p2, Translation2d p3);

			struct ControlPoint {
				ControlPoint() {
					ddx = 0;
					ddy = 0;
				}
				double ddx, ddy;
			};

			static constexpr int kMaxIterations = 100;
			static constexpr double kMinDelta = 0.001;
			static constexpr double kEpsilon = 1e-5;
			static constexpr double kStepSize = 1.0;

		private:
			//
			// The name of the robot path, must be unique in the path group
			//
			std::string name_;

			//
			// The x, y points and robot angle for the waypoints that make up the path
			//
			std::vector<Pose2d> points_;

			// 
			// Constraints for the path.  Note, not all path generators honor all
			// constraints.  If a path generator sees a constraint it cannot honor it should
			// tell the user.
			//
			ConstraintCollection constraints_;

			//
			// Characteristics of the path.
			//
			double start_vel_;
			double end_vel_;
			double max_vel_;
			double max_accel_;
			double max_jerk_;
			double start_angle_;
			double start_angle_delay_;
			double end_angle_;
			double end_angle_delay_;

			//
			// The path group this path belong to
			//
			std::shared_ptr<PathGroup> parent_;

			//
			// These are physical trajector paths returned by the path generator.  All path
			// generators are required to return at least one trajector path.
			//
			std::list<std::shared_ptr<PathTrajectory>> trajectories_;

			//
			// This is a set of simple spline equestions that are used to represent the
			// robot path when displaying it on the screen.  These are not actually used for
			// path generation, because each path generator may have its own method for 
			// computing the physical path.
			//
			std::vector<std::shared_ptr<xero::paths::SplinePair>> splines_;

			//
			// Used to lock access to the trajectory map
			//
			std::mutex trajectory_lock_;

			//
			// Path distance between each waypoint in the path
			//
			std::vector<double> distances_;

			//
			// If true, path generation cannot find a solution for this path, maybe
			// too many constraints and call cannot be met
			//
			bool impossible_;
		};
	}
}

