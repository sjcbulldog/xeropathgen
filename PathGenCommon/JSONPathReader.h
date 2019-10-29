#pragma once

#include "PathCollection.h"
#include "JSON.h"
#include <string>

namespace xero {
	namespace paths
	{
		class JSONPathReader
		{
		public:
			JSONPathReader() = delete;
			virtual ~JSONPathReader() = delete;

			static bool readJSONPathFile(const std::string& filename, const RobotParams& robot, PathCollection& paths);
			static bool readJSONRobotFile(const std::string& filename, RobotParams& robot);

		private:
			static std::wstring toWide(const std::string& str);
			static std::string toNarrow(const std::wstring& str);
			static bool readFile(const std::string& filename, std::wstring& data);
			static bool parseGroups(const std::string& filename, const RobotParams& robot, PathCollection& paths, JSONValue* groups);
			static bool parseGroup(const std::string& filename, const RobotParams& robot, PathCollection& paths, JSONValue *group);
			static bool parsePath(const std::string& filename, const RobotParams& robot, PathCollection& paths, const std::string &path, JSONValue* group);

			static bool parseConstraints(const std::string& filename, JSONObject& path, std::shared_ptr<RobotPath> onepath);
			static bool parseFlags(const std::string& filename, JSONObject& path, std::shared_ptr<RobotPath> onepath);
		};
	}
}

