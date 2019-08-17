#pragma once

#include "PathFileTreeModel.h"
#include "GeneratorParameterStore.h"
#include "Generator.h"
#include <RobotParams.h>
#include <RobotPath.h>
#include <QTemporaryFile>
#include <list>
#include <memory>
#include <mutex>
#include <thread>

class PathGenerationEngine
{
public:
	PathGenerationEngine();
	virtual ~PathGenerationEngine();

	size_t waitingPaths() {
		size_t ret;

		waiting_paths_lock_.lock();
		ret = waiting_.size();
		waiting_paths_lock_.unlock();

		return ret;
	}

	GeneratorParameterStore& getParameterStore() {
		return store_;
	}

	void setGeneratorStore(const GeneratorParameterStore& store) {
		store_lock_.lock();
		store_ = store;
		store_lock_.unlock();
	}

	void setRobot(std::shared_ptr<xero::paths::RobotParams> robot) {
		stopAll();
		cleanup();
		waiting_paths_lock_.lock();
		waiting_.clear();
		waitForAllIdle();
		robot_ = robot;
		init();
		waiting_paths_lock_.unlock();
	}

	void setGenerator(std::shared_ptr<Generator> gen) {
		stopAll();
		cleanup();
		waiting_paths_lock_.lock();
		waiting_.clear();
		waitForAllIdle();
		generator_ = gen;
		init();
		waiting_paths_lock_.unlock();
	}

	void setParallel(size_t count) {
		parallel_ = count;
	}

	size_t getParallel() const {
		return parallel_;
	}

	void setUnits(const std::string& v) {
		units_ = v;
	}

	void markPathDirty(std::shared_ptr<xero::paths::RobotPath> path);
	void stopAll();
	std::shared_ptr<xero::paths::RobotPath> getComplete();

private:
	struct thread_data
	{
		std::thread* thread_;
		bool running_;
		bool stopped_;
		bool idle_;
	};

private:
	void init();
	void cleanup();
	void threadFunction(thread_data *arg);
	void waitForAllIdle();

	bool runGenerator(std::shared_ptr<xero::paths::RobotPath> path, double maxvel, double maxaccel, thread_data* data, QTemporaryFile &outfile);
	bool runOnePath(std::shared_ptr<xero::paths::RobotPath> path, thread_data* data);
	bool readResults(QFile& outfile, std::vector<xero::paths::Pose2dWithTrajectory>& pts);
	std::shared_ptr<xero::paths::RobotPath> waitForWork(thread_data *data);

private:
	std::list<std::shared_ptr<xero::paths::RobotPath>> waiting_;
	std::list<std::shared_ptr<xero::paths::RobotPath>> complete_;
	std::shared_ptr<xero::paths::RobotParams> robot_;
	std::shared_ptr<Generator> generator_;
	std::mutex waiting_paths_lock_;
	std::mutex per_thread_data_lock_;
	std::mutex complete_paths_locks;
	std::mutex store_lock_;
	size_t parallel_;
	std::vector<thread_data*> per_thread_data_;
	std::list<thread_data*> old_thread_data_;
	GeneratorParameterStore store_;
	std::string units_;
};

