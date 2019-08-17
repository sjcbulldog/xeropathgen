#pragma once
#include <QVariant>
#include <string>

class GeneratorParameter
{
public:
	GeneratorParameter(const std::string& name, const std::string& desc, const std::string &type,  const std::string& arg, QVariant def) {
		name_ = name;
		description_ = desc;
		type_ = type;
		arg_ = arg;
		default_ = def;
	}

	const std::string& getName() const {
		return name_;
	}

	const std::string& getDescription() const {
		return description_;
	}

	const std::string& getType() const {
		return type_;
	}

	const std::string& getArg() const {
		return arg_;
	}

	QVariant getDefault() const {
		return default_;
	}

private:
	std::string name_;
	std::string description_;
	std::string type_;
	std::string arg_;
	QVariant default_;
};
