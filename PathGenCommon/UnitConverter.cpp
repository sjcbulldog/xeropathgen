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
			{ "feet", "in", 12.0, UnitConverter::UnitType::Length},
			{ "foot", "in", 12.0, UnitConverter::UnitType::Length},
			{ "ft", "inches", 12.0, UnitConverter::UnitType::Length},
			{ "feet", "inches", 12.0, UnitConverter::UnitType::Length},
			{ "foot", "inches", 12.0, UnitConverter::UnitType::Length},
			{ "in", "cm", 2.54, UnitConverter::UnitType::Length},
			{ "in", "m", 0.0254, UnitConverter::UnitType::Length},
			{ "in", "meters", 0.0254, UnitConverter::UnitType::Length},
			{ "lbs", "kg", 0.453592, UnitConverter::UnitType::Weight},
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
