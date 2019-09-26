#pragma once

#include "PathGroup.h"
#include "RobotParams.h"
#include <list>
#include <string>
#include <memory>
#include <algorithm>

namespace xero
{
	namespace paths
	{
		class PathCollection
		{
		public:
			PathCollection() {
			}

			virtual ~PathCollection() {
			}

			std::shared_ptr<PathGroup> getGroupByIndex(size_t index) const {
				std::shared_ptr<PathGroup> ret;

				if (index < groups_.size())
				{
					auto it = groups_.begin();
					std::advance(it, index);
					ret = *it;
				}

				return ret;
			}

			size_t getIndexForGroup(std::shared_ptr<PathGroup> group) const {
				size_t index = 0;

				for (auto gr : groups_)
				{
					if (gr == group)
						return index;

					index++;
				}

				return std::numeric_limits<size_t>::max();
			}

			size_t size() const {
				return groups_.size();
			}

			size_t getPathCount() const {
				size_t ret = 0;

				for (auto gr : groups_)
					ret += gr->getPathCount();

				return ret;
			}

			std::shared_ptr<PathGroup> getGroupByName(const std::string& name) const {
				std::list<std::shared_ptr<PathGroup>>::const_iterator it =
					std::find_if(groups_.begin(), groups_.end(), [name](std::shared_ptr<PathGroup> g) { return g->getName() == name; });

				if (it == groups_.cend())
					return nullptr;

				return *it;
			}

			std::shared_ptr<RobotPath> findPathByName(const std::string &group, const std::string& name) {
				auto gptr = getGroupByName(group);
				if (gptr == nullptr)
					return nullptr;

				return gptr->findPathByName(name);
			}

			bool hasGroup(const std::string & name) const {
				std::list<std::shared_ptr<PathGroup>>::const_iterator it =
					std::find_if(groups_.begin(), groups_.end(), [name](std::shared_ptr<PathGroup> g) { return g->getName() == name ; });
				return it != groups_.end();
			}

			std::shared_ptr<PathGroup> addGroup(const std::string& name) {
				std::shared_ptr<PathGroup> gptr = getGroupByName(name);
				if (gptr != nullptr)
					return gptr;

				gptr = std::make_shared<PathGroup>(this, name);
				groups_.push_back(gptr);
				return gptr;
			}

			void addGroup(int row, std::shared_ptr<PathGroup> gr)
			{
				auto it = groups_.begin();
				std::advance(it, row);
				groups_.insert(it, gr);
			}

			std::shared_ptr<RobotPath> addPath(const std::string &group, const std::string &name) {
				auto gptr = addGroup(group);
				auto path = std::make_shared<RobotPath>(gptr, name);
				gptr->addPath(path);
				return path;
			}

			bool delGroup(const std::string& group) {
			  auto it = std::find_if(groups_.begin(), groups_.end(), [group](std::shared_ptr<PathGroup> g) { return g->getName() == group; });
				if (it == groups_.end())
					return false;

				groups_.erase(it);
				return true;
			}

			bool delPath(const std::string& group, const std::string& path) {
				auto gr = getGroupByName(group);
				if (gr == nullptr)
					return false;
				return gr->delPath(path);
			}

			std::vector<std::string> getGroupNames() {
				std::vector<std::string> names;

				for (auto item : groups_)
					names.push_back(item->getName());

				return names;
			}

			void clear() {
				for (auto group : groups_)
					group->clear();

				groups_.clear();
			}

			const std::list<std::shared_ptr<PathGroup>>& getGroups() const {
				return groups_;
			}

			void getAllPaths(std::list<std::shared_ptr<RobotPath>> &result) const {
				for (auto group : groups_)
				{
					auto plist = group->getPaths();
					result.insert(result.end(), plist.begin(), plist.end());
				}
			}

		private:
			std::list<std::shared_ptr<PathGroup>> groups_;
		};
	}
}


