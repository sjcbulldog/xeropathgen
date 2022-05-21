#include "UnitConverter.h"
#include <stdexcept>
#include <algorithm>

namespace xero
{
	namespace paths
	{
		UnitConverter::conversion UnitConverter::convert_[] =
		{
			{ "ft", "in", 12.0, UnitConverter::UnitType::Length},
			{ "in", "cm", 2.54, UnitConverter::UnitType::Length},
			{ "in", "m", 0.0254, UnitConverter::UnitType::Length},
			{ "m", "ft", 3.28084, UnitConverter::UnitType::Length},
			{ "ft", "cm", 30.48, UnitConverter::UnitType::Length},
			{ "m", "cm", 100.0, UnitConverter::UnitType::Length},

			{ "lbs", "kg", 0.453592, UnitConverter::UnitType::Weight},
		};

		std::string UnitConverter::normalizeUnits(const std::string& units)
		{
			std::string ret = units;

			if (units == "meters")
				ret = "m";
			else if (units == "feet")
				ret = "ft";
			else if (units == "foot")
				ret = "ft";
			else if (units == "inches")
				ret = "in";

			return ret;
		}

		bool UnitConverter::findConversion(const std::string& from, const std::string& to, double& conversion)
		{
			bool ret = false;

			std::string fromnorm = normalizeUnits(from);
			std::string tonorm = normalizeUnits(to);

			if (fromnorm == tonorm)
			{
				conversion = 1.0;
				ret = true;
			}
			else
			{
				for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
				{
					if (fromnorm == convert_[i].from && tonorm == convert_[i].to)
					{
						conversion = convert_[i].conversion;
						ret = true;
						break;
					}

					if (tonorm == convert_[i].from && fromnorm == convert_[i].to)
					{
						conversion = 1.0 / convert_[i].conversion;
						ret = true;
						break;
					}
				}
			}

			return ret;
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

		std::list<std::string> UnitConverter::getAllLengthUnits()
		{
			std::list<std::string> result;

			for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
			{
				if (convert_[i].type_ == UnitType::Length)
				{
					if (std::find(result.begin(), result.end(), convert_[i].to) == result.end())
						result.push_back(convert_[i].to);
					if (std::find(result.begin(), result.end(), convert_[i].from) == result.end())
						result.push_back(convert_[i].from);
				}
			}

			return result;
		}

		std::list<std::string> UnitConverter::getAllWeightUnits()
		{
			std::list<std::string> result;

			for (size_t i = 0; i < sizeof(convert_) / sizeof(convert_[0]); i++)
			{
				if (convert_[i].type_ == UnitType::Weight)
				{
					if (std::find(result.begin(), result.end(), convert_[i].to) == result.end())
						result.push_back(convert_[i].to);
					if (std::find(result.begin(), result.end(), convert_[i].from) == result.end())
						result.push_back(convert_[i].from);
				}
			}

			return result;
		}
	}
}
