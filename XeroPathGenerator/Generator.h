#pragma once
#include "GeneratorParameter.h"
#include <string>
#include <list>


class Generator
{
public:
	Generator(const std::string& name, const std::string &dir, std::string &exec, const std::string &units, std::string &out, 
					const std::string &robot, const std::string &path, const std::string &timestep) {
		name_ = name;
		exec_ = exec;
		dir_ = dir;
		units_arg_ = units;
		output_arg_ = out;
		robot_arg_ = robot;
		path_arg_ = path;
		timestep_arg_ = timestep;
	}

	void addProperty(const std::string& str) {
		properties_.push_back(str);
	}

	bool hasProperty(const std::string& str) const {
		auto it = std::find(properties_.begin(), properties_.end(), str);
		return it != properties_.end();
	}

	const std::string &getName() const {
		return name_;
	}

	const std::string& getExec() const {
		return exec_;
	}

	const std::string& getDir() const {
		return dir_;
	}

	const std::string& getUnitsArg() const {
		return units_arg_;
	}

	const std::string& getOutputArg() const {
		return output_arg_;
	}

	const std::string& getRobotArg() const {
		return robot_arg_;
	}

	const std::string& getPathArg() const {
		return path_arg_;
	}

	const std::string& getTimestepArg() const {
		return timestep_arg_;
	}

	GeneratorParameter &addGeneratorParam(const std::string &name, const std::string &desc, const std::string &type, const std::string &arg, QVariant def) {
		params_.push_back(GeneratorParameter(name, desc, type, arg, def));
		return params_.back();
	}

	const std::list<GeneratorParameter>& getGeneratorParams() const {
		return params_;
	}

	std::string fullPath() const {
		return dir_ + "/" + exec_;
	}

	static constexpr const char* DistanceContraintPropertyName = "distance_constraint";
	static constexpr const char* NonZeroStartEndVelocities = "nonzero_start_end_velocity";

private:
	std::string name_;
	std::string exec_;
	std::string dir_;
	std::string units_arg_;
	std::string output_arg_;
	std::string robot_arg_;
	std::string path_arg_;
	std::string timestep_arg_;
	std::list<GeneratorParameter> params_;
	std::list<std::string> properties_;
};
