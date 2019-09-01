#include "SwerveDriveBaseModel.h"
#include "TrajectoryNames.h"
#include <MathUtils.h>
#include <QDebug>
#include <cmath>

using namespace xero::paths;

SwerveDriveBaseModel::SwerveDriveBaseModel(std::shared_ptr<RobotPath> path, std::shared_ptr<RobotParams> robot) : DriveBaseModel(path, robot)
{
	reset();
}

SwerveDriveBaseModel::~SwerveDriveBaseModel()
{
}

double SwerveDriveBaseModel::getAngle()
{
	Rotation2d r = Translation2d::getAngle(flpos_, frpos_);
	return r.toDegrees() + 90.0;
}

void SwerveDriveBaseModel::reset()
{
	resetIndex();
	fllinpos_ = 0.0;
	frlinpos_ = 0.0;
	bllinpos_ = 0.0;
	brlinpos_ = 0.0;

	//
	// Get the starting position of the robot
	//
	auto traj = getPath()->getTrajectory(TrajectoryName::Main);
	if (traj != nullptr && traj->size() > 0)
	{
		location_ = (*traj)[0].pose();
	}

	//
	// Get the positon of the four wheels
	//
	flpos_ = Translation2d(getRobot()->getEffectiveLength() / 2.0, getRobot()->getEffectiveWidth() / 2.0).
									rotateBy(location_.getRotation()).translateBy(location_.getTranslation());
	frpos_ = Translation2d(getRobot()->getEffectiveLength() / 2.0, -getRobot()->getEffectiveWidth() / 2.0).
									rotateBy(location_.getRotation()).translateBy(location_.getTranslation());
	blpos_ = Translation2d(-getRobot()->getEffectiveLength() / 2.0, getRobot()->getEffectiveWidth() / 2.0).
									rotateBy(location_.getRotation()).translateBy(location_.getTranslation());
	brpos_ = Translation2d(-getRobot()->getEffectiveLength() / 2.0, -getRobot()->getEffectiveWidth() / 2.0).
									rotateBy(location_.getRotation()).translateBy(location_.getTranslation());
}

void SwerveDriveBaseModel::setDemoStep(size_t t)
{
	auto fltraj = getPath()->getTrajectory(TrajectoryName::FL);
	auto frtraj = getPath()->getTrajectory(TrajectoryName::FR);
	auto bltraj = getPath()->getTrajectory(TrajectoryName::BL);
	auto brtraj = getPath()->getTrajectory(TrajectoryName::BR);

	if (fltraj == nullptr || frtraj == nullptr || bltraj == nullptr || brtraj == nullptr)
		return;

	while (getIndex() < t)
	{
		double deltafl = (*fltraj)[getIndex()].position() - fllinpos_;
		const Rotation2d &flhead = (*fltraj)[getIndex()].rotation();

		double deltafr = (*frtraj)[getIndex()].position() - frlinpos_;
		const Rotation2d& frhead = (*frtraj)[getIndex()].rotation();

		double deltabl = (*bltraj)[getIndex()].position() - bllinpos_;
		const Rotation2d& blhead = (*bltraj)[getIndex()].rotation();

		double deltabr = (*brtraj)[getIndex()].position() - brlinpos_;
		const Rotation2d& brhead = (*brtraj)[getIndex()].rotation();

		update(deltafl, flhead, deltafr, frhead, deltabl, blhead, deltabr, brhead);

		flpos_ = (*fltraj)[getIndex()].translation();
		frpos_ = (*frtraj)[getIndex()].translation();
		blpos_ = (*bltraj)[getIndex()].translation();
		brpos_ = (*brtraj)[getIndex()].translation();

		fllinpos_ = (*fltraj)[getIndex()].position();
		frlinpos_ = (*frtraj)[getIndex()].position();
		bllinpos_ = (*bltraj)[getIndex()].position();
		brlinpos_ = (*brtraj)[getIndex()].position();

		incrIndex();
	}
}

void SwerveDriveBaseModel::update(double deltafl, const Rotation2d& flhead, double deltafr, const Rotation2d& frhead,
								  double deltabl, const Rotation2d& blhead, double deltabr, const Rotation2d& brhead)
{
	flpos_ = Translation2d(deltafl, 0.0).rotateBy(flhead) + flpos_;
	frpos_ = Translation2d(deltafr, 0.0).rotateBy(frhead) + frpos_;
	blpos_ = Translation2d(deltabl, 0.0).rotateBy(blhead) + blpos_;
	brpos_ = Translation2d(deltabr, 0.0).rotateBy(brhead) + brpos_;
}

void SwerveDriveBaseModel::getLocation(Translation2d& fl, Translation2d& fr, Translation2d& bl, Translation2d& br)
{
	fl = flpos_;
	fr = frpos_;
	bl = blpos_;
	br = brpos_;
}
