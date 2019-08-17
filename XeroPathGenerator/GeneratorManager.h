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
	static constexpr char* nameTag = "name";
	static constexpr char* programTag = "program";
	static constexpr char* propertiesTag = "properties";
	static constexpr char* execTag = "exec";
	static constexpr char* unitsTag = "units";
	static constexpr char* outputTag = "output";
	static constexpr char* robotTag = "robot";
	static constexpr char* pathsTag = "paths";
	static constexpr char* timestepTag = "timestep";
	static constexpr char* paramsTag = "parameters";
	static constexpr char* paramsNameTag = "name";
	static constexpr char* paramsDescTag = "description";
	static constexpr char* paramsArgTag = "arg";
	static constexpr char* paramsTypeTag = "type";
	static constexpr char* paramsDefaultTag = "default";

private:
	virtual bool checkCount() {
		if (generators_.size() < 0)
			qDebug() << "no generators found, cannot start application - must have at least one generator defined";

		return generators_.size() > 0;
	}
	virtual bool processJSONFile(QFile& file);
	bool processProgram(QFile& file, QJsonObject& obj, const std::string& name);
	bool processParameter(QFile& file, QJsonObject& obj, std::shared_ptr<Generator> gen);

private:
	std::list<std::shared_ptr<Generator>> generators_;
};

