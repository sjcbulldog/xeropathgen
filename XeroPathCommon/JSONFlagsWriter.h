#pragma once

#include <RobotPath.h>

class JSONFlagsWriter
{
public:
	static bool writeFlags(std::shared_ptr<xero::paths::RobotPath> path, const std::string& outfile);
};

