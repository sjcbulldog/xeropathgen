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
#include <PathCollection.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QFile>

class PathCollectionIO
{
public:
	PathCollectionIO() = delete;
	~PathCollectionIO() = delete;

	static bool writePathCollection(QFile& file, const xero::paths::PathCollection& paths, QString& outdir);
	static bool readPathCollection(const std::string& filename, xero::paths::PathCollection& paths, QString& outdir);

private:
	static bool createGroupArray(QJsonArray& a, const xero::paths::PathCollection& paths);
	static bool createGroup(QJsonObject& o, const std::shared_ptr<xero::paths::PathGroup> group);
	static bool createPath(QJsonObject& o, const std::shared_ptr<xero::paths::RobotPath> path);

	static bool readGroup(QFile &file, xero::paths::PathCollection& paths, QJsonObject& group);
	static bool readPath(QFile& file, xero::paths::PathCollection& pahts, std::shared_ptr<xero::paths::PathGroup> group, QJsonObject& obj);

	static bool readDouble(QFile& file, QJsonObject& obj, QString obname, QString name, double& value, bool opt = false);
	static bool readString(QFile& file, QJsonObject& obj, QString obname, QString name, QString &value, bool opt = false);
};
