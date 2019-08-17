#include "DriveBaseModel.h"
#include "TrajectoryNames.h"

using namespace xero::paths;

DriveBaseModel::DriveBaseModel(std::shared_ptr<RobotPath> path, std::shared_ptr<RobotParams> robot)
{
	path_ = path;
	robot_ = robot;
}

DriveBaseModel::~DriveBaseModel()
{
}

double DriveBaseModel::getVelocity(const char* trajname)
{
	auto traj = path_->getTrajectory(trajname);
	assert(traj != nullptr);
	size_t index = index_;
	if (index >= traj->size())
		index = traj->size() - 1;

	return (*traj)[index].velocity();
}

double DriveBaseModel::getAcceleration(const char* trajname)
{
	auto traj = path_->getTrajectory(trajname);
	assert(traj != nullptr);
	size_t index = index_;
	if (index >= traj->size())
		index = traj->size() - 1;

	return (*traj)[index].acceleration();
}

Pose2d DriveBaseModel::getPose(const char* trajname)
{
	auto traj = path_->getTrajectory(trajname);
	assert(traj != nullptr);
	size_t index = index_;
	if (index >= traj->size())
		index = traj->size() - 1;

	return (*traj)[index].pose();
}

bool DriveBaseModel::isDone()
{
	auto main = getPath()->getTrajectory(TrajectoryName::Main);
	return index_ >= main->size();
}

double DriveBaseModel::getTime()
{
	auto main = getPath()->getTrajectory(TrajectoryName::Main);
	if (!isDone())
		return (*main)[index_].time();

	return 0.0;
}