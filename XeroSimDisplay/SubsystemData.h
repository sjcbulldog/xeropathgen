#pragma once

#include <map>
#include <string>
#include <list>

class SubsystemData
{
public:
	SubsystemData(const std::string& name) {
		name_ = name;
	}

	const std::string& getName() const {
		return name_;
	}

	bool addData(const std::string& dname, const std::string& dvalue) {
		auto it = data_.find(dname);
		if (it != data_.end() && it->second == dvalue)
			return false;

		data_[dname] = dvalue;
		return true;
	}

	const std::string& getValue(const std::string& key) {
		return data_[key];
	}

	std::list<std::string> getKeys() {
		std::list<std::string> keys;

		for (std::map<std::string, std::string>::iterator it = data_.begin(); it != data_.end(); ++it)
			keys.push_back(it->first);

		return keys;
	}
private:
	std::string name_;
	std::map<std::string, std::string> data_;
};

