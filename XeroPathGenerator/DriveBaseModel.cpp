//
// Copyright 2019 Jack W. Griffin
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
// http ://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissionsand
// limitations under the License.
//
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