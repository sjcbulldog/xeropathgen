#pragma once

#include <RobotPath.h>
#include <RobotParams.h>
#include <Pose2d.h>

class DriveBaseModel
{
public:
	DriveBaseModel(std::shared_ptr<xero::paths::RobotPath> path, std::shared_ptr<xero::paths::RobotParams> robot);
	virtual ~DriveBaseModel();
	
	virtual void getLocation(xero::paths::Translation2d& fl, xero::paths::Translation2d& fr, 
		xero::paths::Translation2d& bl, xero::paths::Translation2d& br) = 0 ;
	virtual double getAngle() = 0;
	virtual void reset() = 0;
	virtual void setDemoStep(size_t step) = 0;
	virtual bool isDone() ;
	virtual double getTime() ;
	virtual double getVelocity(const char* trajname);
	virtual double getAcceleration(const char* trajname);
	virtual xero::paths::Pose2d getPose(const char* trajname);

	virtual bool isTankDrive() = 0;
	virtual bool isSwerveDrive() = 0;

	void setPath(std::shared_ptr<xero::paths::RobotPath> path) {
		path_ = path;
	}

	void resetIndex() {
		index_ = 0;
	}

	size_t getIndex() {
		return index_;
	}

	void incrIndex() {
		index_++;
	}


protected:
	std::shared_ptr<xero::paths::RobotPath> getPath() {
		return path_;
	}

	std::shared_ptr<xero::paths::RobotParams> getRobot() {
		return robot_;
	}

private:
	std::shared_ptr<xero::paths::RobotPath> path_;
	std::shared_ptr<xero::paths::RobotParams> robot_;
	size_t index_;
};

