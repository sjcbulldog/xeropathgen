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
#include "GeneratorManager.h"
#include <QStringList>
#include <QDirIterator>
#include <QJsonObject>
#include <QDebug>
#include <QCoreApplication>
#include <QVersionNumber>
#include <algorithm>


GeneratorManager::GeneratorManager()
{
}

GeneratorManager::~GeneratorManager()
{
}

bool GeneratorManager::exists(const std::string& name)
{
  auto it = std::find_if(generators_.begin(), generators_.end(), [name](std::shared_ptr<Generator> g) { return g->getName() == name; });
	return it != generators_.end();
}

std::shared_ptr<Generator> GeneratorManager::getGeneratorByName(const std::string& name)
{
  auto it = std::find_if(generators_.begin(), generators_.end(), [name](std::shared_ptr<Generator> g) { return g->getName() == name; });
	if (it == generators_.end())
		return nullptr;

	return *it;
}

void GeneratorManager::readGenerators(QDir& dir, std::list<std::shared_ptr<Generator>>& generators)
{
	generators_.clear();

	QStringList filter;
	filter << "*.json";
	QStringList list = dir.entryList(filter, QDir::Filter::Files);
	for (QString item : list)
	{
	  	QFile file(dir.absoluteFilePath(item)) ;
		processJSONFile(file);
	}

	generators.insert(generators.end(), generators_.begin(), generators_.end());
	generators_.clear();
}

void GeneratorManager::copyDefaults(const std::string& subdir)
{
	QString defdir = getDefaultDir().c_str();
	if (defdir.length() == 0)
		return;

	QString exedir = QCoreApplication::applicationDirPath();
	QString src = exedir + "/" + QString(subdir.c_str());

	QDir srcdir(src);
	QDir destdir(defdir);

	if (!destdir.exists())
	{
		if (!destdir.mkpath(defdir))
			return;
	}

	QStringList list = srcdir.entryList(QDir::Filter::Files);
	for (QString item : list)
	{
		QString srcfile = srcdir.absoluteFilePath(item);
		QString destfile = destdir.absoluteFilePath(item);

		if (QFile::exists(destfile))
			QFile::remove(destfile);

		QFile::copy(srcfile, destfile);
	}
}

bool GeneratorManager::processJSONFile(QFile& file)
{
	QString text;
	std::string name, version;
	int vnum;
	size_t len;
	QVersionNumber genver;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Cannot open file '" << file.fileName() << "' for reading";
		return false;
	}

	text = file.readAll();

	QJsonParseError error;
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &error);
	if (doc.isNull())
	{
		qWarning() << "Cannot parse file '" << file.fileName() << "' for reading";
		qWarning() << error.errorString();
		return false;
	}

	if (!doc.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not hold a JSON object";
		return false;
	}

	if (!getJSONStringValue(file, doc, versionTag, version))
		return false;

	try {
		vnum = std::stoi(version, &len);
	}
	catch (...)
	{
		qWarning() << "JSON file '" << file.fileName() << "' has a '_version' string field, that is not a valid integer";
		return false;
	}

	if (!getJsonVersionValue(file, doc, genVersionTag, genver))
	{
		qWarning() << "JSON file '" << file.fileName() << "' has a 'generator_version' string field, that is not a valid version structure";
		return false;
	}

	if (len != version.length())
	{
		qWarning() << "JSON file '" << file.fileName() << "' has a '_version' string field, that is not a valid integer";
		return false;
	}

	if (vnum != 1)
	{
		qWarning() << "JSON file '" << file.fileName() << "' has a '_version' field '" << vnum << "' that is not valid";
		return false;
	}

	if (!getJSONStringValue(file, doc, nameTag, name))
		return false;

	if (exists(name))
	{
		qWarning() << "JSON file '" << file.fileName() << "' generator with name '" << name.c_str()
			<< "' already exists.";
		return false;
	}

	QJsonValue v = doc[programTag];
	if (v.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << programTag
			<< "' at the top level";
		return false;
	}

	if (!v.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << programTag
			<< "' at the top level, but it is not a JSON object";
		return false;
	}

	QJsonObject obj = v.toObject();

	if (!processProgram(file, obj, name, genver))
		return false;

	v = doc[propertiesTag];
	if (v.isArray())
	{
		std::shared_ptr<Generator> gen = getGeneratorByName(name);
		QJsonArray arr = v.toArray();
		for (const QJsonValue& var : arr)
		{
			if (!var.isString())
				continue;

			QString str = var.toString();
			if (str.length() > 0)
				gen->addProperty(str.toStdString());
		}
	}

	return true;
}

bool GeneratorManager::processProgram(QFile &file, QJsonObject& obj, const std::string &name, const QVersionNumber &genver)
{
	std::string exec, out, robot, path, timestep, units, otherargs;

	QFileInfo info(file);
	QDir dir = info.dir();

	if (!getJSONStringValue(file, obj, execTag, exec))
		return false;

	if (!getJSONStringValue(file, obj, outputTag, out))
		return false;

	if (!getJSONStringValue(file, obj, robotTag, robot))
		return false;

	if (!getJSONStringValue(file, obj, pathsTag, path))
		return false;

	if (!getJSONStringValue(file, obj, timestepTag, timestep))
		return false;

	if (!getJSONStringValue(file, obj, unitsTag, units))
		return false;

	if (obj.contains(otherArgsTag))
	{
		if (!getJSONStringValue(file, obj, otherArgsTag, otherargs))
			return false;
	}

	std::shared_ptr<Generator> gen = std::make_shared<Generator>(name, genver, dir.path().toStdString(), exec, units, out, robot, path, timestep, otherargs);

	//
	// Now process parameters
	//
	QJsonValue v = obj[paramsTag];
	if (v.isUndefined())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contain a field named '" << paramsTag
			<< "' at the top level";
		return false;
	}

	if (!v.isArray())
	{
		qWarning() << "JSON file '" << file.fileName() << "' contains a field named '" << paramsTag
			<< "' at the top level, but it is not a JSON array";
		return false;
	}

	QJsonArray parray = v.toArray();
	for (auto it = parray.begin(); it != parray.end(); it++)
	{
		QJsonValue av = *it;
		if (!av.isObject())
		{
			qWarning() << "JSON file '" << file.fileName() << "' field '" << paramsTag
				<< "' contains array element that is not an object";
			return false;
		}

		QJsonObject obj2 = av.toObject() ;
		if (!processParameter(file, obj2, gen))
			return false;
	}

	generators_.push_back(gen);

	return true;
}

bool GeneratorManager::processParameter(QFile& file, QJsonObject& obj, std::shared_ptr<Generator> gen)
{
	std::string name, desc, type, arg;

	if (!getJSONStringValue(file, obj, paramsNameTag, name))
		return false;

	if (!getJSONStringValue(file, obj, paramsDescTag, desc))
		return false;

	if (!getJSONStringValue(file, obj, paramsTypeTag, type))
		return false;

	if (!getJSONStringValue(file, obj, paramsArgTag, arg))
		return false;

	QVariant defval;
	if (obj.contains(paramsDefaultTag))
	{
		QJsonValue v = obj[paramsDefaultTag];
		if (v.isDouble())
		{
			if (type == GeneratorParameter::DoublePropType)
			{
				defval = QVariant(v.toDouble());
			}
			else if (type == GeneratorParameter::IntegerPropType)
			{
				defval = QVariant(static_cast<int>(v.toDouble()));
			}
		}
		else if (v.isString())
		{
			defval = QVariant(v.toString());
		}
	}
	GeneratorParameter &param = gen->addGeneratorParam(name, desc, type, arg, defval);

	if (obj.contains(paramsChoicesTag))
	{
		QJsonValue varr = obj[paramsChoicesTag];
		if (!varr.isArray())
		{
			qDebug() << "File '" << file << "': reading parameter '" << name.c_str() << "' got 'choices' property but it was not an array of values";
		}
		else
		{
			QJsonArray arr = varr.toArray();
			for (auto value : arr)
			{
				if (!value.isString())
				{
					qDebug() << "File '" << file << ": reading param" << name.c_str() << "' got 'choices' property but value in array was not string";
				}
				else
				{
					param.addChoice(value.toString().toStdString());
				}
			}
		}
	}

	return true;
}
