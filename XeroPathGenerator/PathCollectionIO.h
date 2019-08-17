#pragma once
#include <PathCollection.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

class PathCollectionIO
{
public:
	PathCollectionIO() = delete;
	~PathCollectionIO() = delete;

	static bool writePathCollection(QFile &file, const xero::paths::PathCollection& paths);
	static bool readPathCollection(const std::string& filename, xero::paths::PathCollection& paths);

private:
	static bool createGroupArray(QJsonArray& a, const xero::paths::PathCollection& paths);
	static bool createGroup(QJsonObject& o, const std::shared_ptr<xero::paths::PathGroup> group);
	static bool createPath(QJsonObject& o, const std::shared_ptr<xero::paths::RobotPath> path);

	static bool readGroup(QFile &file, xero::paths::PathCollection& paths, QJsonObject& group);
	static bool readPath(QFile& file, xero::paths::PathCollection& pahts, std::shared_ptr<xero::paths::PathGroup> group, QJsonObject& obj);

	static bool readDouble(QFile& file, QJsonObject& obj, QString obname, QString name, double& value, bool opt = false);
	static bool readString(QFile& file, QJsonObject& obj, QString obname, QString name, QString &value, bool opt = false);
};
