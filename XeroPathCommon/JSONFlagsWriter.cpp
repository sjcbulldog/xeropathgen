#include "JSONFlagsWriter.h"
#include <TrajectoryNames.h>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonDocument>
#include <QFile>

using namespace xero::paths;

bool JSONFlagsWriter::writeFlags(std::shared_ptr<xero::paths::RobotPath> path, const std::string& outfile)
{
	QJsonArray a;

	for (auto flag : path->getFlags())
	{
		double btime, atime;

		if (flag->before() < flag->after())
			continue;

		if (flag->before() < 0.0 || flag->after() < 0.0)
			continue;

		auto traj = path->getTrajectory(TrajectoryName::Main);
		if (traj == nullptr)
			continue;

		if (!traj->getTimeForDistance(flag->before(), btime))
			continue;

		if (!traj->getTimeForDistance(flag->after(), atime))
			continue;

		QJsonObject flagobj;
		flagobj[RobotPath::NameTag] = flag->name().c_str();
		flagobj[RobotPath::BeforeTag] = btime;
		flagobj[RobotPath::AfterTag] = atime;
		a.push_back(flagobj);
	}

	QJsonDocument doc(a);
	QFile file(outfile.c_str());
	if (!file.open(QIODevice::OpenModeFlag::Truncate | QIODevice::OpenModeFlag::WriteOnly))
		return false;

	file.write(doc.toJson());
	file.close();

	return true;
}