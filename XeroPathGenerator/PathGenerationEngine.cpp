#include "PathGenerationEngine.h"
#include "PathCollectionIO.h"
#include "RobotManager.h"
#include "CSVParser.h"
#include <DistanceVelocityConstraint.h>
#include <TankDriveModifier.h>
#include <SwerveDriveModifier.h>
#include <TrajectoryNames.h>
#include <QDebug>
#include <QProcess>
#include <QTemporaryFile>
#include <cassert>
#include <algorithm>

using namespace xero::paths;

PathGenerationEngine::PathGenerationEngine()
{
	parallel_ = 2;
	init();
}

PathGenerationEngine::~PathGenerationEngine()
{
}

void PathGenerationEngine::markPathDirty(std::shared_ptr<RobotPath> path)
{
	waiting_paths_lock_.lock();

	auto it = std::find(waiting_.begin(), waiting_.end(), path);
	if (it != waiting_.end())
	{
		//
		// Its alredy in the dirty list, leave it there
		//
		waiting_paths_lock_.unlock();
		return;
	}

	//
	// Add it to the dirty list
	//
	waiting_.push_back(path);

	qDebug() << "'" << path->getName().c_str() << "' added to path generation dirty list";
	waiting_paths_lock_.unlock();
	cleanup();
}

void PathGenerationEngine::init()
{
	if (robot_ == nullptr || generator_ == nullptr)
		return;

	for (size_t i = 0; i < parallel_; i++)
	{
		thread_data* data = new thread_data();
		per_thread_data_.push_back(data);
		data->thread_ = new std::thread([this, data] { this->threadFunction(data); });
	}
}

void PathGenerationEngine::stopAll()
{
	std::chrono::milliseconds delay(100);

	for (size_t i = 0; i < per_thread_data_.size(); i++)
		per_thread_data_[i]->running_ = false;

	while (true)
	{
		per_thread_data_lock_.lock();
		if (per_thread_data_.size() == 0)
		{
			per_thread_data_lock_.unlock();
			break;
		}

		per_thread_data_lock_.unlock();
		std::this_thread::sleep_for(delay);
	}
}

void PathGenerationEngine::cleanup()
{
	per_thread_data_lock_.lock();

	while (old_thread_data_.size())
	{
		thread_data* data = old_thread_data_.front();
		if (data->thread_->joinable())
			data->thread_->join();

		old_thread_data_.pop_front();
		delete data->thread_;
		delete data;
	}

	per_thread_data_lock_.unlock();
}

std::shared_ptr<RobotPath> PathGenerationEngine::getComplete()
{
	std::shared_ptr<RobotPath> path;

	complete_paths_locks.lock();
	if (complete_.size() > 0)
	{
		path = complete_.front();
		complete_.pop_front();
	}
	complete_paths_locks.unlock();
	return path;
}

void PathGenerationEngine::waitForAllIdle()
{
	auto delay = std::chrono::milliseconds(100);
	bool busy = true;

	while (busy)
	{
		busy = false;
		for (auto data : per_thread_data_)
		{
			if (data->idle_ == false)
			{
				busy = true;
				break;
			}
		}
		std::this_thread::sleep_for(delay);
	}
}

std::shared_ptr<RobotPath> PathGenerationEngine::waitForWork(thread_data *data)
{
	std::shared_ptr<RobotPath> path;
	std::chrono::milliseconds delay(100);

	data->idle_ = true;

	while (data->running_)
	{
		if (robot_ == nullptr || generator_ == nullptr)
		{
			std::this_thread::sleep_for(delay);
			continue;
		}

		waiting_paths_lock_.lock();
		if (waiting_.size() == 0)
		{
			waiting_paths_lock_.unlock();
			std::this_thread::sleep_for(delay);
			continue;
		}

		path = waiting_.front();
		waiting_.pop_front();
		waiting_paths_lock_.unlock();
		break;
	}

	data->idle_ = false;
	return path;
}

bool PathGenerationEngine::runGenerator(std::shared_ptr<xero::paths::RobotPath> path, double maxvel, double maxaccel, thread_data *data, QTemporaryFile &outfile)
{
	//
	// Now process the single path we have been assigned
	//
	QTemporaryFile pathfile;
	QTemporaryFile robotfile;

	path->setImpossible(false);

	//
	// Write the paths to the pathfile temporary file
	//
	PathCollection coll;
	std::shared_ptr<PathGroup> group = coll.addGroup(path->getParent()->getName());
	group->addPath(path);
	double savevel = path->getMaxVelocity();
	double saveacc = path->getMaxAccel();
	path->setMaxVelocity(maxvel);
	path->setMaxAccel(maxaccel);
	if (!PathCollectionIO::writePathCollection(pathfile, coll))
	{
		path->setMaxVelocity(savevel);
		waiting_paths_lock_.lock();
		waiting_.push_back(path);
		waiting_paths_lock_.unlock();
		return false;
	}
	path->setMaxVelocity(savevel);
	path->setMaxAccel(saveacc);

	//
	// Write the robot to the robotfile temporary file
	//
	if (!RobotManager::save(robot_, robotfile))
	{
		waiting_paths_lock_.lock();
		waiting_.push_back(path);
		waiting_paths_lock_.unlock();
		return false;
	}

	QStringList args;
	QString str = generator_->getRobotArg().c_str();
	str.replace("$$", robotfile.fileName());
	QStringList onearg = str.split(' ');
	args.append(onearg);

	str = generator_->getPathArg().c_str();
	str.replace("$$", pathfile.fileName());
	onearg = str.split(' ');
	args.append(onearg);

	str = generator_->getOutputArg().c_str();
	str.replace("$$", outfile.fileName());
	onearg = str.split(' ');
	args.append(onearg);

	str = generator_->getTimestepArg().c_str();
	str.replace("$$", std::to_string(robot_->getTimestep()).c_str());
	onearg = str.split(' ');
	args.append(onearg);

	str = generator_->getUnitsArg().c_str();
	str.replace("$$", units_.c_str());
	onearg = str.split(' ');
	args.append(onearg);

	store_lock_.lock();
	for (const GeneratorParameter& p : generator_->getGeneratorParams())
	{
		if (store_.hasParameterValue(p.getName().c_str()))
		{
			str = p.getArg().c_str();
			QVariant v = store_[p.getName().c_str()];
			if (p.getType() == "double")
			{
				str.replace("$$", std::to_string(v.toDouble()).c_str());
				onearg = str.split(' ');
				args.append(onearg);
			}
			else
			{
				qWarning() << "unhandled generator parameter type '" << p.getType().c_str() << "' in generator JSON file";
			}
		}
	}
	store_lock_.unlock();

	qDebug() << "==================================================";
	qDebug() << "Running path'" << path->getName().c_str() << "'";
	QProcess* p = new QProcess();
	std::string genpath = generator_->fullPath();
	p->start(genpath.c_str(), args);
	if (!p->waitForStarted())
	{
		qDebug() << "Failed: generator '" << genpath.c_str() << "' - failed to launch";
		return false;
	}

	QByteArray ret;
	QString readdata;
	std::string error, output;
	QProcess::ProcessState st;
	int count = 120;			// Two minutes

	qDebug() << "Waiting for generator to finish";
	while (!p->waitForFinished(1000) && count > 0)
	{
		count--;
		if ((count % 10) == 0)
			qDebug() << "    still waiting, count = " << count;
	}


	ret = p->readAllStandardError();
	readdata = QString(ret);
	error += readdata.toStdString();

	ret = p->readAllStandardOutput();
	readdata = QString(ret);
	output += readdata.toStdString();

	if (!data->running_ || count == 0)
	{
		qDebug() << "killed generator, path '" << path->getName().c_str() << "'";
		qDebug() << "#########################################################";
		p->kill();
		qDebug() << "stdout: " << output.c_str();
		qDebug() << "stderr: " << error.c_str();
		return false;
	}

	int code = p->exitCode();
	QProcess::ExitStatus status = p->exitStatus();
	if (p->exitCode() != 0)
	{
		qDebug() << "Generated exited with error, path '" << path->getName().c_str() << "'";
		qDebug() << "------------------------------------------------------";

		//
		// Send the stdout and stderror to the log
		//
		qDebug() << "pathgen program failed";
		qDebug() << "exit code " << p->exitCode();
		qDebug() << "stdout: " << output.c_str();
		qDebug() << "stderr: " << error.c_str();

		if (p->exitCode() == 99)
			path->setImpossible(true);

		return false;
	}

	qDebug() << "Generator finished sucessfully, path '" << path->getName().c_str() << "'";
	qDebug() << "$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$";

	return true;
}

bool PathGenerationEngine::readResults(QFile& outfile, std::vector<Pose2dWithTrajectory>& pts)
{
	CSVParser parser;

	if (!parser.open(outfile.fileName().toStdString()))
		return false;

	while (parser.hasData())
	{
		double time = parser.getData(RobotPath::TimeTag);
		double x = parser.getData(RobotPath::XTag);
		double y = parser.getData(RobotPath::YTag);
		double heading = parser.getData(RobotPath::HeadingTag);
		double pos = parser.getData(RobotPath::PositionTag);
		double velocity = parser.getData(RobotPath::VelocityTag);
		double accel = parser.getData(RobotPath::AccelerationTag);
		double jerk = parser.getData(RobotPath::JerkTag);

		Translation2d trans(x, y);
		Rotation2d rot = Rotation2d::fromDegrees(heading);
		Pose2d pose(trans, rot);
		Pose2dWithTrajectory tpt(pose, time, pos, velocity, accel, jerk);
		pts.push_back(tpt);

		if (!parser.next())
			return false;
	}

	return true;
}

bool PathGenerationEngine::runOnePath(std::shared_ptr<xero::paths::RobotPath> path, thread_data* data)
{
	QTemporaryFile* outfile = nullptr;
	double percent = 0.0;
	int pass = 1;
	std::vector<Pose2dWithTrajectory> pts;
	DriveModifier* mod = nullptr;
	bool swerve = false;

	if (robot_->getDriveType() == RobotParams::DriveType::TankDrive)
	{
		mod = new TankDriveModifier();
		percent = 0.0;
	}
	else if (robot_->getDriveType() == RobotParams::DriveType::SwerveDrive)
	{
		mod = new SwerveDriveModifier();
		percent = 0.05;
		swerve = true;
	}

	while (true)
	{
		pts.clear();

		outfile = new QTemporaryFile();
		outfile->open();
		outfile->close();

		qDebug() << "processing path '" << path->getName().c_str() << "', pass " << pass;

		double vel = path->getMaxVelocity();
		double acc = path->getMaxAccel();
		if (!runGenerator(path, vel * (1 - percent), acc * (1 - percent), data, *outfile))
		{
			//
			// Generation failed, no results to process
			//
			return false;
		}

		//
		// Now parse the data the results
		//
		if (!readResults(*outfile, pts))
			return false;

		auto traj = std::make_shared<PathTrajectory>(TrajectoryName::Main, pts);
		path->addTrajectory(traj);

		if (swerve)
		{
			SwerveDriveModifier* sw = dynamic_cast<SwerveDriveModifier*>(mod);
			assert(sw != nullptr);
			sw->setRotationalPercent(percent);
		}

		if (mod->modify(*robot_, path, units_))
			break;

		percent += 0.05;
		delete outfile;
	}

	delete mod;
	return true;
}

void PathGenerationEngine::threadFunction(thread_data* data)
{
	data->running_ = true;
	data->stopped_ = false;
	std::chrono::milliseconds delay(100);

	while (data->running_)
	{
		std::shared_ptr<RobotPath> path = waitForWork(data);
		if (path == nullptr)
			continue;

		if (runOnePath(path, data))
		{
			complete_paths_locks.lock();
			complete_.push_back(path);
			complete_paths_locks.unlock();
		}
	}

	per_thread_data_lock_.lock();
	auto it = std::find(per_thread_data_.begin(), per_thread_data_.end(), data);
	assert(it != per_thread_data_.end());
	per_thread_data_.erase(it);
	old_thread_data_.push_back(data);
	per_thread_data_lock_.unlock();
}
