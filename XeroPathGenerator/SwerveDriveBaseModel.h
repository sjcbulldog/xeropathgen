#pragma once
#include "DriveBaseModel.h"

class SwerveDriveBaseModel : public DriveBaseModel
{
public:
	SwerveDriveBaseModel(std::shared_ptr<xero::paths::RobotPath> path, std::shared_ptr<xero::paths::RobotParams> robot);
	virtual ~SwerveDriveBaseModel();

	virtual void setDemoStep(size_t t);
	virtual void getLocation(xero::paths::Translation2d& fl, xero::paths::Translation2d& fr, 
		xero::paths::Translation2d& bl, xero::paths::Translation2d& br) ;
	virtual double getAngle();
	virtual void reset();

	virtual bool isTankDrive() { return false; }
	virtual bool isSwerveDrive() { return true; }

private:
	void update(double fldel, const xero::paths::Rotation2d &flhead, double frdel, const xero::paths::Rotation2d& frhead,
				double bldel, const xero::paths::Rotation2d& blhead, double brdel, const xero::paths::Rotation2d& brhead);

private:
	xero::paths::Pose2d location_;

	xero::paths::Translation2d flpos_;
	xero::paths::Translation2d frpos_;
	xero::paths::Translation2d blpos_;
	xero::paths::Translation2d brpos_;

	double fllinpos_;
	double frlinpos_;
	double bllinpos_;
	double brlinpos_;
};

