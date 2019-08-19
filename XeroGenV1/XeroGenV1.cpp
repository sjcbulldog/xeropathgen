#include "JSONPathReader.h"
#include "PathCollection.h"
#include "CSVWriter.h"
#include "PathTrajectory.h"
#include "XeroGenV1PathGenerator.h"
#include <iostream>

using namespace xero::paths;

std::string pathfile;
std::string robotfile;
std::string outfile;
std::string units = "in";
double timestep = 0.02;
double diststep = 1.0;
xero::paths::PathCollection collection;
bool scurve = true;

extern void generateForGroup(const std::string& group);
extern void generateForPath(PathGroup& group, const std::string& path);

int main(int ac, char** av)
{
	ac--;
	av++;

	//
	// Process the command line arguments
	//
	while (ac > 0 && **av == '-')
	{
		size_t index;

		std::string arg = *av++;
		ac--;

		if (arg == "--diststep")
		{
			if (ac == 0) {
				std::cerr << "pathfinderV1: expected floating point nubmer following --step argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				diststep = std::stoi(arg, &index);
			}
			catch (...)
			{
				std::cerr << "pathfinderV1: expected floating point nubmer following --step argument" << std::endl;
				return 1;
			}

			if (index != arg.length())
			{
				std::cerr << "pathfinderV1: expected floating point nubmer following --step argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--timestep")
		{
			if (ac == 0) {
				std::cerr << "pathfinderV1: expected floating point nubmer following --timestep argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				timestep = std::stod(arg, &index);
			}
			catch (...)
			{
				std::cerr << "pathfinderV1: expected floating point nubmer following --timestep argument" << std::endl;
				return 1;
			}

			if (index != arg.length())
			{
				std::cerr << "pathfinderV1: expected floating point nubmer following --timestep argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--scurve")
		{
			scurve = true;
		}
		else if (arg == "--trapezoid")
		{
			scurve = false;
		}
		else if (arg == "--units")
		{
			if (ac == 0) {
				std::cerr << "pathfinderV1: expected units type following --units argument" << std::endl;
				return 1;
			}
			units = *av++;
			ac--;
		}
		else if (arg == "--pathfile")
		{
			if (ac == 0) {
				std::cerr << "pathfinderV1: expected directory name following --pathfile argument" << std::endl;
				return 1;
			}
			pathfile = *av++;
			ac--;
		}
		else if (arg == "--robotfile")
		{
			if (ac == 0) {
				std::cerr << "pathfinderV1: expected directory name following --robotfile argument" << std::endl;
				return 1;
			}
			robotfile = *av++;
			ac--;
		}
		else if (arg == "--outfile")
		{
			if (ac == 0) {
				std::cerr << "pathfinderV1: expected directory name following --output argument" << std::endl;
				return 1;
			}

			outfile = *av++;
			ac--;
		}
		else
		{
			std::cerr << "pathfinderV1: invalid command line argument '" << arg << "'" << std::endl;
			return 1;
		}
	}

	if (robotfile.length() == 0)
	{
		std::cerr << "pathfinderV1: no robot file specified" << std::endl;
		return 1;
	}

	if (pathfile.length() == 0)
	{
		std::cerr << "pathfinderV1: no path file specified" << std::endl;
		return 1;
	}

	RobotParams robot("");
	if (!JSONPathReader::readJSONRobotFile(robotfile, robot))
	{
		std::cerr << "pathfinderV1: error reading robot file '" << robotfile << "'" << std::endl;
		return 1;
	}
	robot.convert(units);

	if (!JSONPathReader::readJSONPathFile(pathfile, robot, collection)) {
		std::cerr << "pathfinderV1: error reading path file '" << pathfile << "'" << std::endl;
		return 1;
	}

	if (collection.getPathCount() != 1) {
		std::cerr << "CheesyGen: can only process a single path in the paths file" << std::endl;
		return 1;
	}

	std::vector<std::string> groups = collection.getGroupNames();
	for (const std::string& group : groups)
		generateForGroup(group);
}

void generateForGroup(const std::string& group)
{
	auto gptr = collection.getGroupByName(group);
	if (gptr == nullptr)
	{
		std::cerr << "pathfinderV1: warning: path group '" << group << "' does not exist" << std::endl;
		return;
	}

	std::cout << "Generating paths for group " << group << " ... " << std::endl;
	std::vector<std::string> paths = gptr->getPathNames();
	for (const std::string& path : paths)
		generateForPath(*gptr, path);
}

void generateForPath(PathGroup& group, const std::string& path)
{
	std::cout << "  Generating paths for path " << path << " ... " << std::endl;
	auto pptr = group.findPathByName(path);
	if (pptr == nullptr)
	{
		std::cerr << "pathfinderV1: warning: path '" << path << "' does not exist in group '";
		std::cerr << group.getName() << "'" << std::endl;
		return;
	}

	//
	// Turn the waypoints into a set of irregularly spaced trajectory points.  The spacing will be
	// variable and will be given by the allowed differences between points in the X, Y, and THETA
	// values.
	//
	std::shared_ptr<PathTrajectory> trajectory;

	XeroGenV1PathGenerator gen(diststep, timestep, scurve);
	trajectory = gen.generate(pptr->getPoints(), pptr->getConstraints(), pptr->getStartVelocity(),
		pptr->getEndVelocity(), pptr->getMaxVelocity(), pptr->getMaxAccel(), pptr->getMaxJerk());

	std::vector<std::string> headers =
	{
		RobotPath::TimeTag,
		RobotPath::XTag,
		RobotPath::YTag,
		RobotPath::PositionTag,
		RobotPath::VelocityTag,
		RobotPath::AccelerationTag,
		RobotPath::JerkTag,
		RobotPath::HeadingTag
	};
	CSVWriter::write<PathTrajectory::const_iterator>(outfile, headers, trajectory->begin(), trajectory->end());
}
