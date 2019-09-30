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
#pragma once
#include "GeneratorParameter.h"
#include <QVersionNumber>
#include <string>
#include <list>


class Generator
{
public:
	Generator(const std::string& name, const QVersionNumber &genver, const std::string &dir, std::string &exec, const std::string &units, std::string &out, 
					const std::string &robot, const std::string &path, const std::string &timestep, const std::string &otherargs) {
		name_ = name;
		genver_ = genver;
		exec_ = exec;
		dir_ = dir;
		units_arg_ = units;
		output_arg_ = out;
		robot_arg_ = robot;
		path_arg_ = path;
		timestep_arg_ = timestep;
		other_args_ = otherargs;
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

	const QVersionNumber& getVersion() const {
		return genver_;
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

	const std::string& getOtherArgs() const {
		return other_args_;
	}

	bool hasOtherArgs() const {
		return other_args_.length() > 0;
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
	QVersionNumber genver_;
	std::string exec_;
	std::string dir_;
	std::string units_arg_;
	std::string output_arg_;
	std::string robot_arg_;
	std::string path_arg_;
	std::string timestep_arg_;
	std::string other_args_;
	std::list<GeneratorParameter> params_;
	std::list<std::string> properties_;
};
