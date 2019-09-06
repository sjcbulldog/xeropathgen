#include "RobotManager.h"
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

using namespace xero::paths;

RobotManager::RobotManager()
{
}

RobotManager::~RobotManager()
{
}

bool RobotManager::deleteRobot(const std::string& robot)
{
	bool ret = false;

	auto it = std::find_if(robots_.begin(), robots_.end(), [robot](std::shared_ptr<RobotParams> one) { return one->getName() == robot; });
	if (it != robots_.end())
	{
		auto robotobj = *it;
		QFile file(robotobj->getFilename().c_str());
		file.remove();
		robots_.erase(it);
		ret = true;
	}

	return ret;
}

std::shared_ptr<xero::paths::RobotParams> RobotManager::importRobot(QFile &file)
{
	processJSONFile(file) ;

	auto robot = robots_.back();
	robot->setFilename("");
	if (!add(robot))
	{
		deleteRobot(robot->getName());
		throw std::runtime_error("cannot save robot file");
	}
	return robot;
}

void RobotManager::convert(const std::string& units)
{
	for (auto robot : robots_)
		robot->convert(units);
}

bool RobotManager::exists(const std::string& name)
{
	for (auto robot : robots_)
	{
		if (robot->getName() == name)
			return true;
	}

	return false;
}

std::shared_ptr<RobotParams> RobotManager::getDefaultRobot()
{
	if (robots_.size() == 0)
		return nullptr;

	return robots_.front();
}

std::shared_ptr<RobotParams> RobotManager::getRobotByName(const std::string& name)
{
	for (auto field : robots_)
	{
		if (field->getName() == name)
			return field;
	}

	return nullptr;
}

std::list<std::string> RobotManager::getNames()
{
	std::list<std::string> names;

	for (auto field : robots_)
		names.push_back(field->getName());

	return names;
}

std::list<std::shared_ptr<RobotParams>> RobotManager::getRobots()
{
	std::list<std::shared_ptr<RobotParams>> list;
	for (auto field : robots_)
		list.push_back(field);

	return list;
}

bool RobotManager::processJSONFile(QFile& file)
{
	std::string name_value, verstr;
	double ewidth_value;
	double elength_value;
	double rwidth_value;
	double rlength_value;
	double velocity_value;
	double accel_value;
	double jerk_value;
	double timestep_value;
	int drivetype_value;
	QString text;
	int version;
	size_t len;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
		throw std::runtime_error("cannot open file for reading");

	text = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
	if (doc.isNull())
		throw std::runtime_error("invalid JSON text in file");

	if (!doc.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not hold a JSON object";
		throw std::runtime_error("valid JSON, but not correct format for a robot file");
	}

	if (!getJSONStringValue(file, doc, RobotParams::VersionTag, verstr))
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not have a '_version' value, assuming version 1";
	}
	else
	{
		try {
			version = std::stoi(verstr, &len);
		}
		catch (...)
		{
			qWarning() << "JSON file '" << file.fileName() << "' has a '_version' string field, that is not a valid integer";
			throw std::runtime_error("valid JSON, but not correct format for a robot file");
		}

		if (len != verstr.length())
		{
			qWarning() << "JSON file '" << file.fileName() << "' has a '_version' string field, that is not a valid integer";
			throw std::runtime_error("valid JSON, but not correct format for a robot file");
		}

		if (version != 1)
		{
			qWarning() << "JSON file '" << file.fileName() << "' has a '_version' field '" << version << "' that is not valid";
			throw std::runtime_error("valid JSON, but not correct format for a robot file");
		}
	}

	if (!getJSONStringValue(file, doc, RobotParams::NameTag, name_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (exists(name_value))
	{
		std::string msg = "robot with the name '" + name_value + "' already exists";
		throw std::runtime_error(msg.c_str());
	}

	if (!getJSONDoubleValue(file, doc, RobotParams::EffectiveWidthTag, ewidth_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::EffectiveLengthTag, elength_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::RobotWidthTag, rwidth_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::RobotLengthTag, rlength_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::MaxVelocityTag, velocity_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::MaxAccelerationTag, accel_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::MaxJerkTag, jerk_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONDoubleValue(file, doc, RobotParams::TimeStepTag, timestep_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	if (!getJSONIntegerValue(file, doc, RobotParams::DriveTypeTag, drivetype_value))
		throw std::runtime_error("valid JSON, but not correct format for a robot file");

	std::shared_ptr<RobotParams> robot = std::make_shared<RobotParams>(name_value);
	robot->setFilename(file.fileName().toStdString());
	robot->setTimestep(timestep_value);
	robot->setEffectiveWidth(ewidth_value);
	robot->setEffectiveLength(elength_value);
	robot->setRobotWidth(rwidth_value);
	robot->setRobotLength(rlength_value);
	robot->setMaxVelocity(velocity_value);
	robot->setMaxAcceleration(accel_value);
	robot->setMaxJerk(jerk_value);
	robot->setDriveType(static_cast<RobotParams::DriveType>(drivetype_value));

	robots_.push_back(robot);
	return true;
}

bool RobotManager::getNextFile(QFileInfo &file)
{
	size_t i = 1;
	while (i < 1024)
	{
		std::string path = getFirstDir();
		path += "//";
		path += std::to_string(i) + ".json";

		QFileInfo info(path.c_str());
		if (!info.exists())
		{
			file = info;
			return true;
		}

		i++;
	}
	return false;
}

bool RobotManager::add(std::shared_ptr<xero::paths::RobotParams> robot)
{
	robots_.push_back(robot);
	return save(robot);
}

bool RobotManager::save(std::shared_ptr<xero::paths::RobotParams> robot)
{
	QFile* file;
	bool ret;

	if (robot->getFilename().length() > 0)
	{
		file = new QFile(robot->getFilename().c_str());
	}
	else
	{
		QFileInfo fileinfo;

		if (!getNextFile(fileinfo))
			return false;

		file = new QFile(fileinfo.absoluteFilePath());
		robot->setFilename(fileinfo.absoluteFilePath().toStdString());
	}

	ret = save(robot, *file);
	delete file;

	return ret;
}

bool RobotManager::save(std::shared_ptr<xero::paths::RobotParams> robot, QFile &file)
{
	QJsonObject obj;
	QFileInfo info(file);
	QDir dir = info.absoluteDir();

	if (!dir.exists())
		dir.mkpath(dir.path());

	obj[RobotParams::VersionTag] = "1";
	obj[RobotParams::NameTag] = robot->getName().c_str();
	obj[RobotParams::DriveTypeTag] = static_cast<int>(robot->getDriveType());
	obj[RobotParams::TimeStepTag] = robot->getTimestep();
	obj[RobotParams::EffectiveWidthTag] = robot->getEffectiveWidth();
	obj[RobotParams::EffectiveLengthTag] = robot->getEffectiveLength();
	obj[RobotParams::RobotWidthTag] = robot->getRobotWidth();
	obj[RobotParams::RobotLengthTag] = robot->getRobotLength();
	obj[RobotParams::MaxVelocityTag] = robot->getMaxVelocity();
	obj[RobotParams::MaxAccelerationTag] = robot->getMaxAccel();
	obj[RobotParams::MaxJerkTag] = robot->getMaxJerk();

	QJsonDocument doc(obj);
	file.open(QFile::WriteOnly);
	file.write(doc.toJson());
	file.close();
	return true;
}
