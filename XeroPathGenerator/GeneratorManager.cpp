#include "GeneratorManager.h"
#include <QStringList>
#include <QDirIterator>
#include <QJsonObject>
#include <QDebug>
#include <algorithm>


GeneratorManager::GeneratorManager()
{
}

GeneratorManager::~GeneratorManager()
{
}

bool GeneratorManager::exists(const std::string& name)
{
	auto it = std::find_if(generators_.begin(), generators_.end(), [name](auto g) { return g->getName() == name; });
	return it != generators_.end();
}

std::shared_ptr<Generator> GeneratorManager::getGeneratorByName(const std::string& name)
{
	auto it = std::find_if(generators_.begin(), generators_.end(), [name](auto g) { return g->getName() == name; });
	if (it == generators_.end())
		return nullptr;

	return *it;
}

bool GeneratorManager::processJSONFile(QFile& file)
{
	QString text;
	std::string name;

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

	if (!processProgram(file, obj, name))
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

bool GeneratorManager::processProgram(QFile &file, QJsonObject& obj, const std::string &name)
{
	std::string exec, out, robot, path, timestep, units;

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

	std::shared_ptr<Generator> gen = std::make_shared<Generator>(name, dir.path().toStdString(), exec, units, out, robot, path, timestep);

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

		QJsonObject obj = av.toObject() ;
		if (!processParameter(file, obj, gen))
			return false;
	}

	generators_.push_back(gen);

	return true;
}

bool GeneratorManager::processParameter(QFile& file, QJsonObject& obj, std::shared_ptr<Generator> gen)
{
	std::string name, desc, type, arg;
	double def;

	if (!getJSONStringValue(file, obj, paramsNameTag, name))
		return false;

	if (!getJSONStringValue(file, obj, paramsDescTag, desc))
		return false;

	if (!getJSONStringValue(file, obj, paramsTypeTag, type))
		return false;

	if (!getJSONStringValue(file, obj, paramsArgTag, arg))
		return false;

	if (!getJSONDoubleValue(file, obj, paramsDefaultTag, def))
		return false;

	gen->addGeneratorParam(name, desc, type, arg, QVariant(def));

	return true;
}
