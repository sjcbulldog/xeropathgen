// DevApp.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "JSONPathReader.h"
#include "PathCollection.h"
#include "CSVWriter.h"
#include "PathGenerator.h"
#include "DistanceView.h"
#include "TimingUtil.h"
#include <iostream>

using namespace xero::paths;

double maxDx = 2.0;
double maxDy = 0.25;
double maxDTheta = MathUtils::degreesToRadians(0.1);
double step = 2.0;
double timestep = 0.02;
std::string pathfile;
std::string outfile;
std::string robotfile;
std::string units;
xero::paths::PathCollection collection;

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

		if (arg == "--dx")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected floating point nubmer following --dx argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				maxDx = std::stod(arg, &index);
			}
			catch (...)
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --dx argument" << std::endl;
				return 1;
			}

			if (index != arg.length()) 
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --dx argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--dy")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected floating point nubmer following --dy argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				maxDy = std::stod(arg, &index);
			}
			catch (...)
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --dy argument" << std::endl;
				return 1;
			}

			if (index != arg.length())
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --dy argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--dtheta")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected floating point nubmer following --dtheta argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				maxDTheta = std::stod(arg, &index);
				maxDTheta = MathUtils::degreesToRadians(maxDTheta);
			}
			catch (...)
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --dtheta argument" << std::endl;
				return 1;
			}

			if (index != arg.length())
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --dtheta argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--step")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected floating point nubmer following --step argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				step = std::stod(arg, &index);
			}
			catch (...)
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --step argument" << std::endl;
				return 1;
			}

			if (index != arg.length())
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --step argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--timestep")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected floating point nubmer following --timestep argument" << std::endl;
				return 1;
			}

			arg = *av;
			try {
				timestep = std::stod(arg, &index);
			}
			catch (...)
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --timestep argument" << std::endl;
				return 1;
			}

			if (index != arg.length())
			{
				std::cerr << "CheesyGen: expected floating point nubmer following --timestep argument" << std::endl;
				return 1;
			}
			ac--;
			av++;
		}
		else if (arg == "--units")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected units type following --units argument" << std::endl;
				return 1;
			}
			units = *av++;
			ac--;
		}
		else if (arg == "--pathfile")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected directory name following --pathfile argument" << std::endl;
				return 1;
			}
			pathfile = *av++;
			ac--;
		}
		else if (arg == "--outfile")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected directory name following --outfile argument" << std::endl;
				return 1;
			}
			outfile = *av++;
			ac--;
		}
		else if (arg == "--robotfile")
		{
			if (ac == 0) {
				std::cerr << "CheesyGen: expected directory name following --robotfile argument" << std::endl;
				return 1;
			}
			robotfile = *av++;
			ac--;
		}
		else
		{
			std::cerr << "CheesyGen: invalid command line argument '" << arg << "'" << std::endl;
			return 1;
		}
	}

	RobotParams robot("name");
	if (!JSONPathReader::readJSONRobotFile(robotfile, robot))
	{
		std::cerr << "CheesyGen: error reading robot file '" << robotfile << "'" << std::endl;
		return 1;
	}
	robot.convert(units);

	if (!JSONPathReader::readJSONPathFile(pathfile, robot, collection)) {
		std::cerr << "CheesyGen: error reading path file '" << pathfile << "'" << std::endl;
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
		std::cerr << "CheesyGen: warning: path group '" << group << "' does not exist" << std::endl;
		return;
	}

	std::cout << "Generating paths for group " << group << " ... " << std::endl;
	std::vector<std::string> paths = gptr->getPathNames();
	for(const std::string &path: paths)
		generateForPath(*gptr, path);
}

void createJerk(TimedTrajectory& ttj)
{
	auto pts = ttj.getPoints();
	ttj.setJerk(0, 0.0);
	for(size_t i = 1 ; i < pts.size() ; i++)
	{
		double jerk = (pts[i].getAcceleration() - pts[i - 1].getAcceleration()) /
			(pts[i].getTime() - pts[i - 1].getTime());

		ttj.setJerk(i, jerk);
	}
}

void generateForPath(PathGroup& group, const std::string& path)
{
	std::cout << "  Generating paths for path " << path << " ... " << std::endl;
	auto pptr = group.findPathByName(path);
	if (pptr == nullptr)
	{
		std::cerr << "CheesyGen: warning: path '" << path << "' does not exist in group '";
		std::cerr << group.getName() << "'" << std::endl;
		return;
	}

	//
	// Turn the waypoints into a set of irregularly spaced trajectory points.  The spacing will be
	// variable and will be given by the allowed differences between points in the X, Y, and THETA
	// values.
	//
	PathGenerator gen(maxDx, maxDy, maxDTheta);
	Trajectory trajectory = gen.generate(pptr->getPoints());

	//
	// Turn the trajectory points into a set of evenly spaced points along the path.
	//
	DistanceView dist(trajectory);
	TimedTrajectory ttj = TimingUtil::timeParameterizeTrajectory(dist, pptr->getConstraints(), step,
		pptr->getStartVelocity(), pptr->getEndVelocity(), pptr->getMaxVelocity(), pptr->getMaxAccel());

	createJerk(ttj);

	TimedTrajectory timed_ttj = TimingUtil::createConstantTimeTrajectory(ttj, timestep);

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
	CSVWriter::write<std::vector<TimedTrajectoryPoint>::const_iterator>(outfile, headers, timed_ttj.getPoints().begin(), timed_ttj.getPoints().end());
}