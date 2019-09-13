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
#include "ManagerBase.h"
#include "Generator.h"
#include <QDebug>
#include <string>
#include <list>
#include <memory>

class Generator;

class GeneratorManager : public ManagerBase
{
public:
	GeneratorManager();
	virtual ~GeneratorManager();

	bool exists(const std::string& name);

	std::shared_ptr<Generator> getDefaultGenerator() {
		return generators_.front();
	}

	std::list<std::shared_ptr<Generator>> getGenerators() {
		return generators_;
	}

	std::list<std::string> getNames() const {
		std::list<std::string> result;

		for (auto gen : generators_)
			result.push_back(gen->getName());

		return result;
	}

	std::shared_ptr<Generator> getGeneratorByName(const std::string& name);


private:
	static constexpr const char* versionTag = "_version";
	static constexpr const char* nameTag = "name";
	static constexpr const char* programTag = "program";
	static constexpr const char* propertiesTag = "properties";
	static constexpr const char* execTag = "exec";
	static constexpr const char* unitsTag = "units";
	static constexpr const char* otherArgsTag = "others";
	static constexpr const char* outputTag = "output";
	static constexpr const char* robotTag = "robot";
	static constexpr const char* pathsTag = "paths";
	static constexpr const char* timestepTag = "timestep";
	static constexpr const char* paramsTag = "parameters";
	static constexpr const char* paramsNameTag = "name";
	static constexpr const char* paramsDescTag = "description";
	static constexpr const char* paramsArgTag = "arg";
	static constexpr const char* paramsTypeTag = "type";
	static constexpr const char* paramsDefaultTag = "default";
	static constexpr const char* paramsChoicesTag = "choices";

private:
	virtual bool checkCount() {
		if (generators_.size() == 0)
			qDebug() << "no generators found, cannot start application - must have at least one generator defined";

		return generators_.size() > 0;
	}
	virtual bool processJSONFile(QFile& file);
	bool processProgram(QFile& file, QJsonObject& obj, const std::string& name);
	bool processParameter(QFile& file, QJsonObject& obj, std::shared_ptr<Generator> gen);

private:
	std::list<std::shared_ptr<Generator>> generators_;
};

