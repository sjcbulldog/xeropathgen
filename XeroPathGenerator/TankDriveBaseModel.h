#pragma once
#include "DriveBaseModel.h"
class TankDriveBaseModel : public DriveBaseModel
{
public:
	TankDriveBaseModel(std::shared_ptr<xero::paths::RobotPath> path, std::shared_ptr<xero::paths::RobotParams> robot);
	virtual ~TankDriveBaseModel();

	virtual void setDemoStep(size_t t);
	virtual void getLocation(xero::paths::Translation2d& fl, xero::paths::Translation2d& fr, 
		xero::paths::Translation2d& bl, xero::paths::Translation2d& br) ;
	virtual double getAngle();
	virtual void reset();

	virtual bool isTankDrive() { return true; }
	virtual bool isSwerveDrive() { return false; }

private:
	void update(double dl, double dr, double heading);

private:
	double leftpos_;
	double rightpos_;
	xero::paths::Pose2d location_;
};

