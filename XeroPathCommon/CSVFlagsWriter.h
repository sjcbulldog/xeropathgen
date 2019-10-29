#pragma once

#include <RobotPath.h>

class CSVFlagsWriter
{
public:
	static bool writeFlags(std::shared_ptr<xero::paths::RobotPath> path, const std::string& outfile);
};

