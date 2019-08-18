#include "UnitConverter.h"
#include <stdexcept>
#include <algorithm>

namespace xero
{
	namespace paths
	{
		UnitConverter::conversion UnitConverter::convert_[] =
		{
			{ "ft", "in", 12.0},
			{ "feet", "in", 12.0},
			{ "foot", "in", 12.0},
			{ "ft", "inches", 12.0},
			{ "feet", "inches", 12.0},
			{ "foot", "inches", 12.0},
			{ "in", "cm", 2.54},
			{ "in", "m", 0.0254},
			{ "in", "meters", 0.0254},
		};

		bool UnitConverter::findConversion(const std::string& from, const std::string& to, double& conversion)
		{
			for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
			{
				if (from == convert_[i].from && to == convert_[i].to)
				{
					conversion = convert_[i].conversion;
					return true;
				}

				if (to == convert_[i].from && from == convert_[i].to)
				{
					conversion = 1.0 / convert_[i].conversion;
					return true;
				}
			}

			return false;
		}

		double UnitConverter::convert(double value, const std::string& from, const std::string& to)
		{
			double conv;

			if (from == to)
				return value;

			if (!findConversion(from, to, conv))
			{
				std::string msg = "no conversion from '";
				msg += from;
				msg += "' to '";
				msg += to;
				msg += "'";
				throw std::runtime_error(msg.c_str());
			}

			return value * conv;
		}

		float UnitConverter::convert(float value, const std::string& from, const std::string& to)
		{
			double conv;

			if (!findConversion(from, to, conv))
			{
				std::string msg = "no conversion from '";
				msg += from;
				msg += "' to '";
				msg += to;
				msg += "'";
				throw std::runtime_error(msg.c_str());
			}

			return static_cast<float>(value * conv);
		}

		bool UnitConverter::hasConversion(const std::string& from, const std::string& to)
		{
			double conv;
			return findConversion(from, to, conv);
		}

		std::list<std::string> UnitConverter::getAllUnits()
		{
			std::list<std::string> result;

			for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
			{
				if (std::find(result.begin(), result.end(), convert_[i].to) == result.end())
					result.push_back(convert_[i].to);
				if (std::find(result.begin(), result.end(), convert_[i].from) == result.end())
					result.push_back(convert_[i].from);
			}

			return result;
		}
	}
}
