#include "TankDriveBaseModel.h"
#include <TrajectoryNames.h>
#include <MathUtils.h>
#include <cmath>

using namespace xero::paths;

TankDriveBaseModel::TankDriveBaseModel(std::shared_ptr<RobotPath> path, std::shared_ptr<RobotParams> robot) : DriveBaseModel(path, robot)
{
	reset();
}

TankDriveBaseModel::~TankDriveBaseModel()
{
}

double TankDriveBaseModel::getAngle()
{
	return location_.getRotation().toDegrees();
}

void TankDriveBaseModel::setDemoStep(size_t t)
{
	auto lefttraj = getPath()->getTrajectory(TrajectoryName::Left);
	auto righttraj = getPath()->getTrajectory(TrajectoryName::Right);

	if (lefttraj == nullptr || righttraj == nullptr)
		return;

	while (getIndex() < t)
	{
		double curleft = (*lefttraj)[getIndex()].position();
		double curright = (*righttraj)[getIndex()].position();
		double deltaleft = curleft - leftpos_;
		double deltaright = curright - rightpos_;
		update(deltaleft, deltaright, (*lefttraj)[getIndex()].rotation().toDegrees());
		incrIndex();

		leftpos_ = curleft;
		rightpos_ = curright;
	}
}

void TankDriveBaseModel::update(double left, double right, double heading)
{
	double x, y;

;	Rotation2d rot = Rotation2d::fromDegrees(heading);

	if (std::fabs(left - right) <  MathUtils::kEpsilon) {
		x = location_.getTranslation().getX() + right * rot.getCos();
		y = location_.getTranslation().getY() + right * rot.getSin();
	}
	else {
		double r = getRobot()->getWidth() * (left + right) / (2 * (right - left));
		double wd = (right - left) / getRobot()->getWidth();
		x = location_.getTranslation().getX() + r * std::sin(wd + rot.toRadians()) - r * std::sin(rot.toRadians());
		y = location_.getTranslation().getY() - r * std::cos(wd + rot.toRadians()) + r * std::cos(rot.toRadians());
	}

	location_ = Pose2d(x, y, rot);
}

void TankDriveBaseModel::getLocation(Translation2d &fl, Translation2d& fr, Translation2d& bl, Translation2d& br)
{
	fl = location_.getTranslation() + Translation2d(getRobot()->getLength() / 2.0, getRobot()->getWidth() / 2.0).rotateBy(location_.getRotation());
	fr = location_.getTranslation() + Translation2d(getRobot()->getLength() / 2.0, -getRobot()->getWidth() / 2.0).rotateBy(location_.getRotation());
	bl = location_.getTranslation() + Translation2d(-getRobot()->getLength() / 2.0, getRobot()->getWidth() / 2.0).rotateBy(location_.getRotation());
	br = location_.getTranslation() + Translation2d(-getRobot()->getLength() / 2.0, -getRobot()->getWidth() / 2.0).rotateBy(location_.getRotation());
}

void TankDriveBaseModel::reset()
{
	resetIndex();
	leftpos_ = 0.0;
	rightpos_ = 0.0;

	if (getPath() == nullptr)
		return;

	auto traj = getPath()->getTrajectory(TrajectoryName::Main);
	if (traj != nullptr && traj->size() > 0)
	{
		location_ = (*traj)[0].pose();
	}
}
