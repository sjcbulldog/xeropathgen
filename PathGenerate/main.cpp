#include "RobotManager.h"
#include "GeneratorManager.h"
#include "PathGenerationEngine.h"
#include "GeneratorParameterStore.h"
#include "RobotPath.h"
#include "PathGroup.h"
#include "PathCollection.h"
#include "PathCollectionIO.h"
#include "JSONWriter.h"
#include "CSVWriter.h"
#include "build.h"
#include <QtCore/QCoreApplication>
#include <QStandardPaths>
#include <QSettings>
#include <iostream>
#include <list>

using namespace xero::paths;

static void usage()
{
	std::cout << "usage: PathGenerator options" << std::endl;
	std::cout << "options: --debug                  - prints debug information)" << std::endl;
	std::cout << "         --csv                    - path trajectories are in CSV format" << std::endl;
	std::cout << "         --json                   - path trajectories are in JSON format" << std::endl;
	std::cout << "         --outdir                 - output directory for all path trajectories" << std::endl;
	std::cout << "         --generators directory   - adds the given directory to the list to search for generators" << std::endl;
	std::cout << "         --robots directory       - adds the given directory to the list to search for robots" << std::endl;
	std::cout << "         --generator name         - the name of the generator to use" << std::endl;
	std::cout << "         --robot name             - the name of the robot to use" << std::endl;
	std::cout << "         --pathfile name          - the name of the pathfile to process" << std::endl;
	std::cout << "         --units units            - the units to use, inches, feet, meters, cm, etc." << std::endl;
	std::cout << "         --help                   - print this help information" << std::endl;
}

int main(int argc, char *argv[])
{
	bool debug = false;
	bool csv = true;
	bool help = false;
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroPathGenerator");
	QCoreApplication::setApplicationVersion("1.0.0");

	QCoreApplication a(argc, argv);

	QSettings settings;

	GeneratorManager generators;
	GeneratorParameterStore store;
	RobotManager robots;
	std::string outdir;

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString appdir = dirs.front();

	PathGenerationEngine engine;

	std::shared_ptr<RobotParams> robot;
	std::string robotname;
	std::shared_ptr<Generator> generator;
	std::string generatorname;
	std::string pathfile;
	std::string units = "in";

	std::cout << "PathGenerator Version ";
	std::cout << XERO_MAJOR_VERSION << "." << XERO_MINOR_VERSION << ".";
	std::cout << XERO_MICRO_VERSION << " build " << XERO_BUILD_VERSION << std::endl;

	argc--;
	argv++;

	while (argc-- > 0)
	{
		std::string arg = *argv++;
		if (arg == "--debug")
		{
			debug = true;
		}
		else if (arg == "--csv")
		{
			csv = true;
		}
		else if (arg == "--json")
		{
			csv = false;
		}
		else if (arg == "--units")
		{
			if (argc == 0)
			{
				std::cerr << "error: --units flag requires units type argument" << std::endl;
				return 1;
			}
			units = *argv++;
			argc--;
		}
		else if (arg == "--outdir")
		{
			if (argc == 0)
			{
				std::cerr << "error: --outdir flag requires path argument" << std::endl;
				return 1;
			}
			outdir = *argv++;
			argc--;
		}
		else if (arg == "--generators")
		{
			if (argc == 0)
			{
				std::cerr << "error: --generators flag requires path argument" << std::endl;
				return 1;
			}

			generators.setDirectory(*argv++);
			argc--;
		}
		else if (arg == "--robots")
		{
			if (argc == 0)
			{
				std::cerr << "--robots flag requires path argument" << std::endl;
				return 1;
			}

			robots.setDirectory(*argv++);
			argc--;
		}
		else if (arg == "--generator")
		{
			if (argc == 0)
			{
				std::cerr << "--generator flag requires generator name argument" << std::endl;
				return 1;
			}

			generatorname = *argv++;
			argc--;
		}
		else if (arg == "--robot")
		{
			if (argc == 0)
			{
				std::cerr << "--robot flag requires robot name argument" << std::endl;
				return 1;
			}

			robotname = *argv++;
			argc--;
		}
		else if (arg == "--pathfile")
		{
			if (argc == 0)
			{
				std::cerr << "--pathfile flag requires path file argument" << std::endl;
				return 1;
			}

			pathfile = *argv++;
			argc--;
		}
		else if (arg == "--help")
		{
			help = true;
		}
		else
		{
			std::cerr << "error: argument '" << arg << "' is not a valid argument." << std::endl;
			usage();
			return 1;
		}
	}

	generators.addDefaultDirectory((appdir + "/generators").toStdString());
	robots.addDefaultDirectory((appdir + "/robots").toStdString());

	generators.copyDefaults("generators");

	if (!generators.initialize())
	{
		std::cerr << "error: could not initialize the generator manager" << std::endl;
		return 1;
	}

	if (!robots.initialize())
	{
		std::cerr << "error: could not initialize the robots manager" << std::endl;
		return 1;
	}

	if (help)
	{
		usage();
		std::cout << "Generators" << std::endl;
		for (auto gen : generators.getGenerators())
			std::cout << "    " << gen->getName() << std::endl;
		std::cout << std::endl;
		std::cout << "Robots" << std::endl;
		for (auto robot : robots.getRobots())
			std::cout << "    " << robot->getName() << std::endl;

		return 1;
	}

	if (debug)
	{
		generators.dumpSearchPath("Generators");
		robots.dumpSearchPath("Robots");
	}

	generator = generators.getGeneratorByName(generatorname);
	if (generator == nullptr)
	{
		std::cerr << "error: generator '" << generatorname << "' is not a valid generator.";
		return 1;
	}

	robot = robots.getRobotByName(robotname);
	if (robot == nullptr)
	{
		std::cerr << "error: robo '" << robotname << "' is not a valid robot.";
		return 1;
	}

	engine.setUnits(units);
	engine.setGenerator(generator);
	engine.setRobot(robot);
	QString keyname = QString("generator/") + generator->getName().c_str();
	QVariant value = settings.value(keyname);
	store.assignValues(value.toHash());
	engine.setGeneratorStore(store);

	//
	// TODO - add paths to the generator
	//
	PathCollection paths;
	PathCollectionIO::readPathCollection(pathfile, paths);

	std::list<std::shared_ptr<RobotPath>> pathlist;
	paths.getAllPaths(pathlist);

	for(auto path : pathlist)
		engine.markPathDirty(path);

	int pending;
	std::chrono::milliseconds delay(250);
	do {
		pending = static_cast<int>(engine.waitingPaths());
		std::this_thread::sleep_for(delay);
	} while (pending > 0);

	engine.stopAll();

	std::string outfile;
	std::cout << "Processing paths ..." << std::endl;
	for (auto path : pathlist)
	{
		std::cout << "  path '" << path->getName() << "' " << std::flush;
		std::vector<std::string> names = path->getTrajectoryNames();
		for (const std::string& trajname : names)
		{
			//
			// Writre the paths to the last_path_dir directory as
			// CSV files
			//
			if (csv)
			{
				outfile = outdir + "/" + path->getParent()->getName() + "_" + path->getName() + "_" + trajname + ".csv";
			}
			else
			{
				outfile = outdir + "/" + path->getParent()->getName() + "_" + path->getName() + "_" + trajname + ".json";
			}

			std::ofstream outstrm(outfile);

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

			auto t = path->getTrajectory(trajname);
			if (csv)
			{
				CSVWriter::write<std::vector<Pose2dWithTrajectory>::const_iterator>(outstrm, headers, t->begin(), t->end());
			}
			else
			{
				JSONWriter::write<std::vector<Pose2dWithTrajectory>::const_iterator>(outstrm, headers, t->begin(), t->end());
			}
		}
		std::cout << ", length " << path->getDistance() << ", time " << path->getTime() << std::endl;
	}

	return 0;
}
