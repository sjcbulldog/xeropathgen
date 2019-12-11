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
#include "GameFieldManager.h"
#include "GameField.h"
#include <QDirIterator>
#include <QJsonDocument>
#include <QJsonObject>
#include <QDebug>

using namespace xero::paths;

GameFieldManager::GameFieldManager()
{
}

GameFieldManager::~GameFieldManager()
{
}

bool GameFieldManager::hasFile(QString filename)
{
	QString path = QString(getDefaultDir().c_str()) + "/" + filename;
	return QFile::exists(path);
}

void GameFieldManager::convert(const std::string& units)
{
	for (auto field : fields_)
		field->convert(units);
}

bool GameFieldManager::exists(const std::string& name)
{
	for (auto field : fields_)
	{
		if (field->getName() == name)
			return true;
	}

	return false;
}

std::shared_ptr<GameField>  GameFieldManager::getDefaultField()
{
	if (fields_.size() == 0)
		return nullptr;

	return fields_.front();
}

std::shared_ptr<GameField> GameFieldManager::getFieldByName(const std::string& name)
{
	for (auto field : fields_)
	{
		if (field->getName() == name)
			return field;
	}

	return nullptr;
}

std::list<std::string> GameFieldManager::getNames()
{
	std::list<std::string> names;

	for (auto field : fields_)
		names.push_back(field->getName());

	return names;
}

std::list<std::shared_ptr<GameField>> GameFieldManager::getFields()
{
	std::list<std::shared_ptr<GameField>> list;
	for (auto field : fields_)
		list.push_back(field);

	return list;
}

bool GameFieldManager::processJSONFile(QFile& file)
{
	std::string title_value;
	std::string image_value;
	std::string units_value;
	Translation2d topleft;
	Translation2d bottomright;
	Translation2d size;
	QString text;


	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Cannot open file '" << file.fileName() << "' for reading";
		return false;
	}

	text = file.readAll();
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8());
	if (doc.isNull())
	{
		qWarning() << "Cannot parse file '" << file.fileName() << "' for reading";
		return false;
	}

	if (!doc.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not hold a JSON object";
		return false;
	}

	if (!getJSONStringValue(file, doc, gameTag, title_value))
		return false;

	if (!getJSONStringValue(file, doc, imageTag, image_value))
		return false;

	if (!getJSONStringValue(file, doc, unitsTag, units_value))
		return false;

	QJsonValue v = doc[cornersTag];
	if (v.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << cornersTag
			<< "' at the top level";
		return false;
	}

	if (!v.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << cornersTag
			<< "' at the top level, but it is not a JSON object";
		return false;
	}

	QJsonObject obj = v.toObject();

	QJsonValue tl = obj[topLeftCornerTag];
	if (tl.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << topLeftCornerTag
			<< "' in the '" << cornersTag << "' object";
		return false;
	}
	if (!getJSONPointValue(file, tl, topLeftCornerTag, topleft))
		return false;


	QJsonValue br = obj[bottomRightCornerTag];
	if (tl.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << bottomRightCornerTag
			<< "' in the '" << cornersTag << "' object";
		return false;
	}
	if (!getJSONPointValue(file, br, bottomRightCornerTag, bottomright))
		return false;

	if (!getJSONPointValue(file, doc, sizeTag, size))
		return false;

	QFileInfo info(file);
	auto field = std::make_shared<GameField>(info.dir(), title_value, image_value, units_value, topleft, bottomright, size);
	fields_.push_back(field);
	return true;
}