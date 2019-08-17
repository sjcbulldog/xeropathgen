#pragma once
#include "Pose2d.h"
#include <vector>
#include <string>

/// \file

namespace xero
{
	namespace paths
	{
		/// \brief This class reads a set of waypoints from a file.
		/// The waypoints are one per line, with three number representing a waypoint.
		/// The first two numbers are the X and Y coordinates.  The final number is the
		/// heading in degrees.
		class WaypointReader
		{
		public:
			WaypointReader() = delete;;
			virtual ~WaypointReader() = delete;

			/// \brief Read waypoints from a file
			/// \param filename the name of the file to read
			/// \param points the vector to receive the waypoints
			/// \returns true if the points were read sucessfully, false if a problem was encountered
			static bool readWaypoints(const std::string& filename, std::vector<Pose2d>& points);

		private:
			static std::vector<std::string> split(const std::string& str, char ch);
			static bool getNumbers(const std::vector<std::string>& strings, std::vector<double>& numbers);
		};
	}
}
