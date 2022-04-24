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
#include "PathCollectionIO.h"
#include "DistanceVelocityConstraint.h"
#include <QJsonDocument>
#include <QJsonValue>
#include <QFile>
#include <QDebug>

using namespace xero::paths;

bool PathCollectionIO::writePathCollection(QFile &file, const PathCollection& paths, QString &outdir)
{
	QJsonObject obj;
	QJsonArray a;

	if (!createGroupArray(a, paths))
		return false;

	obj[RobotPath::VersionTag] = "2";
	obj[RobotPath::OutputTag] = outdir;
	obj[RobotPath::GroupsTag] = a;

	QJsonDocument doc(obj);
	if (!file.open(QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::WriteOnly))
		return false;

	file.write(doc.toJson());
	file.close();

	return true;
}

bool PathCollectionIO::createMarkersArray(QJsonArray& ma, const std::list<FieldMarker>& markers)
{
	for (auto marker : markers)
	{
		QJsonObject obj;

		obj[RobotPath::XTag] = marker.pos().getX();
		obj[RobotPath::YTag] = marker.pos().getY();
		obj[RobotPath::TypeTag] = marker.which();

		ma.append(obj);
	}

	return true;
}

bool PathCollectionIO::createGroupArray(QJsonArray& a, const PathCollection& paths)
{
	for (auto group : paths.getGroups())
	{
		QJsonObject obj;
		if (!createGroup(obj, group))
			return false;

		a.append(obj);
	}

	return true;
}

bool PathCollectionIO::createGroup(QJsonObject& obj, const std::shared_ptr<PathGroup> group)
{
	obj[RobotPath::NameTag] = group->getName().c_str();

	QJsonArray a;
	for (size_t i = 0; i < group->size(); i++)
	{
		QJsonObject pobj;

		auto path = group->getPathByIndex(i);
		if (!createPath(pobj, path))
			return false;

		a.append(pobj);
	}

	obj[RobotPath::PathsTag] = a;
	return true;
}

bool PathCollectionIO::createPath(QJsonObject& obj, const std::shared_ptr<RobotPath> path)
{
	QJsonArray ma;

	if (!createMarkersArray(ma, path->markers()))
		return false;

	obj[RobotPath::NameTag] = path->getName().c_str();
	obj[RobotPath::StartVelocityTag] = path->getStartVelocity();
	obj[RobotPath::EndVelocityTag] = path->getEndVelocity();
	obj[RobotPath::StartAngleTag] = path->getStartAngle();
	obj[RobotPath::StartAngleDelayTag] = path->getStartAngleDelay();
	obj[RobotPath::EndAngleTag] = path->getEndAngle();
	obj[RobotPath::EndAngleDelayTag] = path->getEndAngleDelay();
	obj[RobotPath::MaxVelocityTag] = path->getMaxVelocity();
	obj[RobotPath::MaxAccelerationTag] = path->getMaxAccel();
	obj[RobotPath::MaxJerkTag] = path->getMaxJerk();
	obj[RobotPath::MaxCentripetalTag] = path->getMaxCentripetal();
	obj[RobotPath::MarkersTag] = ma;

	QJsonArray constraints;
	for (auto con : path->getConstraints())
	{
		std::shared_ptr<DistanceVelocityConstraint> dist = std::dynamic_pointer_cast<DistanceVelocityConstraint>(con);
		if (dist != nullptr)
		{
			QJsonObject conobj;

			conobj[RobotPath::TypeTag] = RobotPath::DistanceVelocityTag;
			conobj[RobotPath::BeforeTag] = dist->getBefore();
			conobj[RobotPath::AfterTag] = dist->getAfter();
			conobj[RobotPath::VelocityTag] = dist->getVelocity();
			constraints.append(conobj);
		}
	}
	obj[RobotPath::ConstraintsTag] = constraints;

	QJsonArray flags;
	for (auto flag : path->getFlags())
	{
		QJsonObject flagobj;

		flagobj[RobotPath::BeforeTag] = flag->before();
		flagobj[RobotPath::AfterTag] = flag->after();
		flagobj[RobotPath::NameTag] = QString(flag->name().c_str());
		flags.append(flagobj);
	}
	obj[RobotPath::FlagsTag] = flags;

	QJsonArray points;
	auto parray = path->getPoints();
	for (size_t i = 0; i < parray.size(); i++)
	{
		const Pose2d& onept = parray[i];
		QJsonObject pt;

		pt[RobotPath::XTag] = onept.getTranslation().getX();
		pt[RobotPath::YTag] = onept.getTranslation().getY();
		pt[RobotPath::HeadingTag] = onept.getRotation().toDegrees();
		points.append(pt);
	}

	obj[RobotPath::PointsTag] = points;

	return true;
}

bool PathCollectionIO::readPathCollection(const std::string& filename, PathCollection& paths, QString &outdir)
{
	QFile file(filename.c_str());
	QString text;

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		qWarning() << "Cannot open file '" << file.fileName() << "' for reading";
		return false;
	}

	text = file.readAll();
	QJsonParseError err;
	QJsonDocument doc = QJsonDocument::fromJson(text.toUtf8(), &err);
	if (doc.isNull())
	{
		qWarning() << "Cannot parse file '" << file.fileName() << "' for reading - " << err.errorString();
		return false;
	}

	if (!doc.isObject())
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not hold a JSON object";
		return false;
	}

	int version = 1;
	QJsonObject obj = doc.object();
	if (!obj.contains(RobotPath::VersionTag))
		qWarning() << "JSON file '" << file.fileName() << "' does not contains '_version' value - assuming version 1";
	else
	{
		QJsonValue vobj = obj[RobotPath::VersionTag];
		if (!vobj.isString())
		{
			qWarning() << "JSON file '" << file.fileName() << "' has '_version' value that is not a string, invalid file";
			return false;
		}

		size_t len;
		std::string verstr = vobj.toString().toStdString();
		try
		{
			version = std::stoi(verstr, &len);
		}
		catch (...)
		{
			qWarning() << "JSON file '" << file.fileName() << "' has '_version' value that is a string, but is not a legal version number";
			return false;
		}

		if (len != verstr.length())
		{
			qWarning() << "JSON file '" << file.fileName() << "' has '_version' value that is a string, but is not a legal version number";
			return false;
		}
	}

	if (version > 2)
	{
		//
		// When we create a version 2, here is where the logic will go to read both version 1 and version 2 files.
		//
		qWarning() << "JSON file '" << file.fileName() << "' has '_version' value that is not a known version";
		return false;
	}

	if (obj.contains(RobotPath::OutputTag))
	{
		QJsonValue vobj = obj[RobotPath::OutputTag];
		if (vobj.isString())
		{
			outdir = vobj.toString();
		}
	}

	if (!obj.contains(RobotPath::GroupsTag))
	{
		qWarning() << "JSON file '" << file.fileName() << "' does not contains 'group' array";
		return false;
	}

	QJsonValue grval = obj[RobotPath::GroupsTag];
	if (!grval.isArray())
	{
		qWarning() << "JSON file '" << file.fileName() << "contains 'group' value, but it is not an array";
		return false;
	}

	QJsonArray groups = grval.toArray();
	for (auto it = groups.begin() ; it != groups.end() ; it++)
	{
		QJsonValue groupval = *it;
		if (!groupval.isObject())
		{
			qWarning() << "JSON file '" << file.fileName() << "', 'group' value contains non-object";
			return false;
		}

		QJsonObject obj2 = groupval.toObject() ;
		if (!readGroup(file, paths, obj2))
			return false;
	}

	return true;
}

bool PathCollectionIO::readMarkers(QFile& file, std::shared_ptr<RobotPath> path, const QJsonArray& a)
{
	for (int i = 0; i < a.size(); i++)
	{
		const QJsonValue &av = a.at(i);
		if (!av.isObject())
		{
			qWarning() << "JSON file '" << file.fileName() << "', entry in markers array is not an object";
			return false;
		}

		double x, y;
		int which;

		if (!readDouble(file, av.toObject(), "marker", RobotPath::XTag, x))
			return false;

		if (!readDouble(file, av.toObject(), "marker", RobotPath::YTag, y))
			return false;

		if (!readInteger(file, av.toObject(), "marker", RobotPath::TypeTag, which))
			return false;

		FieldMarker m(Translation2d(x, y), which);
		path->addMarker(m);
	}

	return true;
}

bool PathCollectionIO::readGroup(QFile &file, PathCollection& paths, QJsonObject& obj)
{
	QString name;

	if (!readString(file, obj, "group", RobotPath::NameTag, name))
		return false;

	std::shared_ptr<PathGroup> groupobj = paths.addGroup(name.toStdString());

	if (!obj.contains(RobotPath::PathsTag))
	{
		qWarning() << "JSON file '" << file.fileName() << "', object in 'group' array missing 'paths' value";
		return false;
	}

	QJsonValue val = obj[RobotPath::PathsTag];
	if (!val.isArray())
	{
		qWarning() << "JSON file '" << file.fileName() << "', object in 'group' array has 'paths' value, but its not an array";
		return false;
	}

	QJsonArray pathsarray = val.toArray();
	for (auto it = pathsarray.begin(); it != pathsarray.end(); it++)
	{
		val = *it;
		if (!val.isObject())
		{
			qWarning() << "JSON file '" << file.fileName() << "', object in 'paths' array is not an object";
			return false;
		}
		QJsonObject obj2 = val.toObject() ;
		if (!readPath(file, paths, groupobj, obj2))
			return false;
	}

	return true;
}

bool PathCollectionIO::readPath(QFile& file, PathCollection & paths, std::shared_ptr<PathGroup> group, QJsonObject& obj)
{
	double endvelocity;
	double startvelocity;
	double startangle;
	double startangledelay;
	double endangle;
	double endangledelay;
	double maxvelocity;
	double maxaccel;
	double maxjerk;
	double maxcent;
	QString name;
	Pose2d pt;
	QJsonValue v;

	if (!readString(file, obj, "path", RobotPath::NameTag, name))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::StartVelocityTag, startvelocity))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::EndVelocityTag, endvelocity))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::MaxVelocityTag, maxvelocity))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::MaxAccelerationTag, maxaccel))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::MaxJerkTag, maxjerk))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::MaxCentripetalTag, maxcent))
		maxcent = 10000;

	if (!readDouble(file, obj, "path", RobotPath::StartAngleTag, startangle, true))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::StartAngleDelayTag, startangledelay, true))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::EndAngleTag, endangle, true))
		return false;

	if (!readDouble(file, obj, "path", RobotPath::EndAngleDelayTag, endangledelay, true))
		return false;

	std::shared_ptr<RobotPath> path = paths.addPath(group->getName(), name.toStdString());
	path->setStartVelocity(startvelocity);
	path->setEndVelocity(endvelocity);
	path->setMaxVelocity(maxvelocity);
	path->setMaxAccel(maxaccel);
	path->setMaxJerk(maxjerk);
	path->setMaxCentripetal(maxcent);
	path->setStartAngle(startangle);
	path->setStartAngleDelay(startangledelay);
	path->setEndAngle(endangle);
	path->setEndAngleDelay(endangledelay);

	if (obj.contains(RobotPath::MarkersTag))
	{
		QJsonValue va = obj.value(RobotPath::MarkersTag);
		if (!va.isArray())
		{
			qWarning() << "JSON file '" << file.fileName() << "', value for 'markers' entry is not an array";
			return false;
		}
		if (!readMarkers(file, path, va.toArray()))
			return false;
	}

	if (obj.contains(RobotPath::ConstraintsTag))
	{
		QString str;

		QJsonValue v2 = obj[RobotPath::ConstraintsTag];
		if (v2.isArray())
		{
			QJsonArray arr = v2.toArray();
			for (const QJsonValue& v3 : arr)
			{
				if (!v3.isObject())
					continue;

				QJsonObject vo = v3.toObject();

				if (!readString(file, vo, "constraint", RobotPath::TypeTag, str))
					continue;

				if (str != RobotPath::DistanceVelocityTag)
					continue;

				double before, after, velocity;

				if (!readDouble(file, vo, "constraint", RobotPath::BeforeTag, before))
					continue;

				if (!readDouble(file, vo, "constraint", RobotPath::AfterTag, after))
					continue;

				if (!readDouble(file, vo, "constraint", RobotPath::VelocityTag, velocity))
					continue;

				std::shared_ptr<DistanceVelocityConstraint> dist = std::make_shared<DistanceVelocityConstraint>(after, before, velocity);
				path->addTimingConstraint(dist);
			}
		}
	}

	if (obj.contains(RobotPath::FlagsTag))
	{
		QString str;

		QJsonValue v2 = obj[RobotPath::FlagsTag];
		if (v2.isArray())
		{
			QJsonArray arr = v2.toArray();
			for (const QJsonValue& v3 : arr)
			{
				if (!v3.isObject())
					continue;

				QJsonObject vo = v3.toObject();

				double before, after;
				QString name;

				if (!readDouble(file, vo, "flag", RobotPath::BeforeTag, before))
					continue;

				if (!readDouble(file, vo, "flag", RobotPath::AfterTag, after))
					continue;

				if (!readString(file, vo, "flag", RobotPath::NameTag, name))
					continue;

				std::shared_ptr<PathFlag> flag = std::make_shared<PathFlag>(name.toStdString(), after, before);
				path->addFlag(flag);
			}
		}
	}

	if (!obj.contains(RobotPath::PointsTag))
	{
		qWarning() << "JSON file '" << file.fileName() << "', path object missing 'points' value";
		return false;
	}
	v = obj[RobotPath::PointsTag];
	if (!v.isArray())
	{
		qWarning() << "JSON file '" << file.fileName() << "', path object has 'points' value but it is not an array";
		return false;
	}
	QJsonArray ptsarray = v.toArray();

	for (auto it = ptsarray.begin(); it != ptsarray.end(); it++)
	{
		double x, y, heading;

		QJsonValue ptval = *it;
		if (!ptval.isObject())
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object 'points' array contains non-object";
			return false;
		}

		QJsonObject ptobj = ptval.toObject();

		if (!ptobj.contains(RobotPath::XTag))
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object missing 'x' value";
			return false;
		}
		v = ptobj[RobotPath::XTag];
		if (!v.isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object has 'x' value but it is not a number";
			return false;
		}
		x = v.toDouble();

		if (!ptobj.contains(RobotPath::YTag))
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object missing 'y' value";
			return false;
		}
		v = ptobj[RobotPath::YTag];
		if (!v.isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object has 'y' value but it is not a number";
			return false;
		}
		y = v.toDouble();

		if (!ptobj.contains(RobotPath::HeadingTag))
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object missing 'heading' value";
			return false;
		}
		v = ptobj[RobotPath::HeadingTag];
		if (!v.isDouble())
		{
			qWarning() << "JSON file '" << file.fileName() << "', path object has 'heading' value but it is not a number";
			return false;
		}
		heading = v.toDouble();

		pt = Pose2d(x, y, Rotation2d::fromDegrees(heading));
		path->addPoint(pt);
	}
	path->generateSplines();

	return true;
}

bool PathCollectionIO::readInteger(QFile& file, const QJsonObject& obj, QString obname, QString name, int& value, bool opt)
{
	if (!obj.contains(name))
	{
		if (opt)
		{
			value = 0;
			return true;
		}
		qWarning() << "JSON file '" << file.fileName() << "', object '" << obname << "' missing field '" << name << "'";
		return false;
	}

	QJsonValue v = obj[name];
	if (!v.isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "', object '" << obname << "' has field '" << name << "' but it is not a number";
		return false;
	}
	value = v.toInt();
	return true;
}

bool PathCollectionIO::readDouble(QFile& file, const QJsonObject& obj, QString obname, QString name, double& value, bool opt)
{
	if (!obj.contains(name))
	{
		if (opt)
		{
			value = 0.0;
			return true;
		}
		qWarning() << "JSON file '" << file.fileName() << "', object '" << obname << "' missing field '" << name << "'";
		return false;
	}

	QJsonValue v = obj[name];
	if (!v.isDouble())
	{
		qWarning() << "JSON file '" << file.fileName() << "', object '" << obname << "' has field '" << name << "' but it is not a number";
		return false;
	}
	value = v.toDouble();
	return true;
}

bool PathCollectionIO::readString(QFile& file, const QJsonObject& obj, QString obname, QString name, QString& value, bool opt)
{
	if (!obj.contains(name))
	{
		if (opt)
		{
			value = "";
			return true;
		}

		qWarning() << "JSON file '" << file.fileName() << "', object '" << obname << "' missing field '" << name << "'";
		return false;
	}

	QJsonValue v = obj[name];
	if (!v.isString())
	{
		qWarning() << "JSON file '" << file.fileName() << "', object '" << obname << "' has field '" << name << "' but it is not a string";
		return false;
	}
	value = v.toString();
	return true;
}
