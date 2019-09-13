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

