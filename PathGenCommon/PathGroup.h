#pragma once

#include "RobotPath.h"
#include "PathBase.h"
#include <list>
#include <string>
#include <memory>
#include <algorithm>

namespace xero
{
	namespace paths
	{
		class PathCollection;

		class PathGroup : public PathBase
		{
			friend class PathCollection;

		public:
			PathGroup(PathCollection *parent, const std::string& name) {
				parent_ = parent;
				name_ = name;

			}

			virtual ~PathGroup() {
			}

			PathCollection* getParent() {
				return parent_;
			}

			size_t size() const {
				return paths_.size();
			}

			std::shared_ptr<RobotPath> getPathByIndex(size_t index) const {
				std::shared_ptr<RobotPath> ret;

				if (index < paths_.size())
				{
					auto it = paths_.begin();
					std::advance(it, index);
					ret = *it;
				}

				return ret;
			}

			size_t getIndexFromPath(std::shared_ptr<RobotPath> path) const {
				size_t index = 0;
				auto it = paths_.begin();

				while (it != paths_.end())
				{
					if (*it == path)
						return index;

					index++;
					it++;
				}

				return std::numeric_limits<size_t>::max();
			}

			virtual const std::string& getName() const {
				return name_;
			}

			virtual void setName(const std::string& name) {
				name_ = name;
			}

			std::shared_ptr<RobotPath> findPathByName(const std::string& name) const {
				std::list<std::shared_ptr<RobotPath>>::const_iterator it =
					std::find_if(paths_.begin(), paths_.end(), [name](std::shared_ptr<RobotPath> p) { return p->getName() == name; });

				if (it == paths_.end())
					return nullptr;

				return *it;
			}

			bool hasPath(const std::string& name) const {
				std::list<std::shared_ptr<RobotPath>>::const_iterator it =
					std::find_if(paths_.begin(), paths_.end(), [name](std::shared_ptr<RobotPath> p) { return p->getName() == name; });

				return it != paths_.end();
			}


			std::vector<std::string> getPathNames() {
				std::vector<std::string> names;

				for (auto item : paths_)
					names.push_back(item->getName());

				return names;
			}

			std::list<std::shared_ptr<RobotPath>>& getPaths() {
				return paths_;
			}

			size_t getPathCount() const {
				return paths_.size();
			}

			void clear() {
				for (auto pair : paths_)
					pair->clear();

				paths_.clear();
			}

			void addPath(std::shared_ptr<RobotPath> path) {
				paths_.push_back(path);
			}

			bool delPath(const std::string& path) {
				auto it = std::find_if(paths_.begin(), paths_.end(), [path](auto p) {return p->getName() == path; });
				if (it == paths_.end())
					return false;

				paths_.erase(it);
				return true;
			}

		private:
			std::string name_;
			std::list<std::shared_ptr<RobotPath>> paths_;
			PathCollection* parent_;
		};
	}
}

