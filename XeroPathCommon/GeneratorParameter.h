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
#include <QVariant>
#include <string>
#include <list>

class GeneratorParameter
{
public:
	static constexpr const char* DoublePropType = "double";
	static constexpr const char* IntegerPropType = "integer";
	static constexpr const char* StringListPropType = "strings";
	static constexpr const char* StringPropType = "string";

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

	void addChoice(const std::string& choice) {
		choices_.push_back(choice);
	}

	const std::list<std::string>& getChoices() const {
		return choices_;
	}

private:
	std::string name_;
	std::string description_;
	std::string type_;
	std::string arg_;
	std::list<std::string>choices_;
	QVariant default_;

};
