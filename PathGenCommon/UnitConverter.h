#pragma once

#include <string>
#include <list>

/// \file

namespace xero
{
	namespace paths
	{
		/// \brief This class contains static methods for converting values from one set of units to another
		class UnitConverter
		{
		public:
			UnitConverter() = delete;
			~UnitConverter() = delete;

			/// \brief convert a double value from one set of units to another
			/// \param value the value to convert
			/// \param from the units to convert from
			/// \param to the units to convert to
			/// \returns value converted to new units
			static double convert(double value, const std::string& from, const std::string& to);

			/// \brief convert a double value from one set of units to another
			/// \param value the value to convert
			/// \param from the units to convert from
			/// \param to the units to convert to
			/// \return value converted to new units
			static float convert(float value, const std::string& from, const std::string& to);

			/// \brief returns information about whether a conversion is possible
			/// \param from the units to convert from
			/// \param to the units to convert to
			/// \returns true if the conversion is possible, false otherwise
			static bool hasConversion(const std::string& from, const std::string& to);

			/// \brief returns all units the converter can process
			/// \returns a list of all units the converter can process
			static std::list<std::string> getAllUnits();

		private:
			struct conversion
			{
				const char* from;
				const char* to;
				double conversion;
			};

			static bool findConversion(const std::string& from, const std::string& to, double& conversion);
			static struct conversion convert_[];
		};
	}
}
