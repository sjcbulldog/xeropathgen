#include "WaypointReader.h"
#include <fstream>
#include <iostream>

namespace xero {
	namespace paths {

		std::vector<std::string> WaypointReader::split(const std::string& str, char ch)
		{
			std::vector<std::string> result;
			size_t index = 0;
			std::string word;

			while (index < str.length())
			{
				if (str[index] == ch)
				{
					result.push_back(word);
					word.clear();
				}
				else
				{
					word += str[index];
				}
				index++;
			}

			if (word.length() > 0 || (str.length() > 0 && str[index - 1] == ch))
				result.push_back(word);

			return result;
		}

		bool WaypointReader::getNumbers(const std::vector<std::string>& words, std::vector<double>& numbers)
		{
			bool ret = true;

			for (const std::string& word : words)
			{
				double d;
				size_t idx;
				
				try {
					d = std::stod(word, &idx);
					if (idx != word.length())
					{
						std::cerr << "WapointReader: invalid floating point number '" << word << "'" << std::endl;
						ret = false;
						break;
					}
				}
				catch(...)
				{
					std::cerr << "WapointReader: invalid floating point number '" << word << "'" << std::endl;
					ret = false;
					break;
				}

				numbers.push_back(d);
			}

			return ret;
		}

		bool WaypointReader::readWaypoints(const std::string& filename, std::vector<Pose2d> &points)
		{
			std::string line;
			std::ifstream strm(filename);
			std::vector<double> numbers;
			size_t lineno = 0;
			if (!strm.is_open())
				return false;

			while (std::getline(strm, line))
			{
				lineno++;

				if (line[0] == '#')
					continue;

				std::vector<std::string> words = split(line, ',');
				if (words.size() != 3)
				{
					std::cerr << "WaypointReader: line " << lineno << " contained for than three comma seperated numbers" << std::endl;
					return false;
				}

				numbers.clear();
				if (!getNumbers(words, numbers))
					return false;

				Translation2d t(numbers[0], numbers[1]);
				Rotation2d rot = Rotation2d::fromDegrees(numbers[2]);
				Pose2d p(t, rot);
				points.push_back(p);
			}

			return true;
		}
	}
}
