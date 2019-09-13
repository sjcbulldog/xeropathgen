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
#include "ManagerBase.h"
#include <RobotParams.h>
#include <QFile>
#include <QFileInfo>
#include <string>
#include <list>
#include <memory>

class RobotManager : public ManagerBase
{
public:
	RobotManager();
	virtual ~RobotManager();

	std::list<std::string> getNames();
	std::list<std::shared_ptr<xero::paths::RobotParams>> getRobots();

	bool exists(const std::string& name);

	std::shared_ptr<xero::paths::RobotParams> getRobotByName(const std::string& name);
	std::shared_ptr<xero::paths::RobotParams> getDefaultRobot();
	void convert(const std::string& units);

	bool add(std::shared_ptr<xero::paths::RobotParams> robot);
	bool save(std::shared_ptr<xero::paths::RobotParams> robot);
	std::shared_ptr<xero::paths::RobotParams>  importRobot(QFile &file);

	static bool save(std::shared_ptr<xero::paths::RobotParams> robot, QFile& file);

	bool deleteRobot(const std::string& name);

protected:
	virtual bool checkCount() {
		return true;
	}

private:
	virtual bool processJSONFile(QFile& file);
	bool getNextFile(QFileInfo &file);

private:
	std::list<std::shared_ptr<xero::paths::RobotParams>> robots_;
};

