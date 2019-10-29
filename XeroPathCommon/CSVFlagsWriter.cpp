#include "CSVFlagsWriter.h"
#include <TrajectoryNames.h>
#include <fstream>

using namespace xero::paths;

bool CSVFlagsWriter::writeFlags(std::shared_ptr<xero::paths::RobotPath> path, const std::string& outfile)
{
	std::ofstream outstrm(outfile);

	if (!outstrm.is_open())
		return false;

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

		outstrm << '"' << flag->name() << '"' << ",";
		outstrm << atime << "," << btime << std::endl;
	}

	return true;
}