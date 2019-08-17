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

