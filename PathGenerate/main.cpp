#include "RobotManager.h"
#include "GeneratorManager.h"
#include "PathGenerationEngine.h"
#include "GeneratorParameterStore.h"
#include "RobotPath.h"
#include "JSONWriter.h"
#include "CSVWriter.h"
#include <QtCore/QCoreApplication>
#include <QStandardPaths>
#include <iostream>
#include <list>

using namespace xero::paths;

int main(int argc, char *argv[])
{
	bool debug = false;
	bool csv = true;
	QCoreApplication::setOrganizationName("ErrorCodeXero");
	QCoreApplication::setOrganizationDomain("www.wilsonvillerobotics.com");
	QCoreApplication::setApplicationName("XeroPathGenerator");
	QCoreApplication::setApplicationVersion("1.0.0");

	QCoreApplication a(argc, argv);

	GeneratorManager generators;
	GeneratorParameterStore store;
	RobotManager robots;
	std::list<std::shared_ptr<RobotPath>> pathlist;
	std::string outdir;

	QStringList dirs = QStandardPaths::standardLocations(QStandardPaths::AppLocalDataLocation);
	QString appdir = dirs.front();

	PathGenerationEngine engine;

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

	if (debug)
	{
		generators.dumpSearchPath("Generators");
		robots.dumpSearchPath("Robots");
	}

	//
	// TODO - load the paths file
	//

	//
	// TODO - path engine, set generator and load generator specific parameters from application settings
	//

	//
	// TODO - path engine, set robot
	//
	engine.setGeneratorStore(store);

	//
	// TODO - add paths to the generator
	//

	int pending;
	std::chrono::milliseconds delay(250);
	do {
		pending = static_cast<int>(engine.waitingPaths());
		std::this_thread::sleep_for(delay);
	} while (pending > 0);

	std::string outfile;
	for (auto path : pathlist)
	{
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
			if (output_type_ == OutputType::OutputCSV)
			{
				CSVWriter::write<std::vector<Pose2dWithTrajectory>::const_iterator>(outfile, headers, t->begin(), t->end());
			}
			else
			{
				JSONWriter::write<std::vector<Pose2dWithTrajectory>::const_iterator>(outfile, headers, t->begin(), t->end());
			}
		}
	}

	return a.exec();
}
