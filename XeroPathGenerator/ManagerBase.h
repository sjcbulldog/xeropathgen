#pragma once

#include <Translation2d.h>
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>

class ManagerBase
{
public:
	ManagerBase();
	virtual ~ManagerBase();

	virtual bool initialize();

	void setDirectory(const std::string& dir) {
		dirs_.clear();
		dirs_.push_back(dir);
	}

	void addDirectory(const std::string& dir) {
		dirs_.push_back(dir);
	}

	void dumpSearchPath(const char* name);

protected:
	virtual bool processPath(const std::string& path);
	virtual bool processJSONFile(QFile& file) = 0;
	virtual bool checkCount() = 0;

	const std::string& getFirstDir() {
		assert(dirs_.size() > 0);
		return dirs_.front();
	}

	bool getJSONStringValue(QFile &file, QJsonDocument& doc, const char* name, std::string& value);
	bool getJSONStringValue(QFile& file, QJsonObject& obj, const char* name, std::string& value);
	bool getJSONDoubleValue(QFile& file, QJsonDocument& doc, const char* name, double& value);
	bool getJSONDoubleValue(QFile& file, QJsonObject& doc, const char* name, double& value);
	bool getJSONIntegerValue(QFile& file, QJsonDocument& doc, const char* name, int& value);
	bool getJSONPointValue(QFile& file, const QJsonValue& value, const char *name, xero::paths::Translation2d& t2d);
	bool getJSONPointValue(QFile& file, QJsonDocument& doc, const char* name, xero::paths::Translation2d& t2d);

private:
	std::list<std::string> dirs_;
};

