#pragma once

#include "UnitConverter.h"
#include <vector>
#include <cassert>

namespace xero {
	namespace paths
	{
		class RobotParams
		{
		public:
			enum class DriveType : int
			{
				TankDrive,
				SwerveDrive,
				Invalid
			};

			static constexpr const char* VersionTag = "_version";
			static constexpr const wchar_t* VersionTagW = L"_version";
			static constexpr const char* NameTag = "name";
			static constexpr const wchar_t* NameTagW = L"name";
			static constexpr const char* LengthTag = "length";
			static constexpr const wchar_t* LengthTagW = L"length";
			static constexpr const char* WidthTag = "width";
			static constexpr const wchar_t* WidthTagW = L"width";
			static constexpr const char* MaxVelocityTag = "maxvelocity";
			static constexpr const wchar_t* MaxVelocityTagW = L"maxvelocity";
			static constexpr const char* MaxAccelerationTag = "maxacceleration";
			static constexpr const wchar_t* MaxAccelerationTagW = L"maxacceleration";
			static constexpr const char* MaxJerkTag = "maxjerk";
			static constexpr const wchar_t* MaxJerkTagW = L"maxjerk";
			static constexpr const char* TimeStepTag = "timestep";
			static constexpr const wchar_t* TimeStepTagW = L"timestep";
			static constexpr const char* DriveTypeTag = "drivetype";
			static constexpr const wchar_t* DriveTypeTagW = L"drivetype";

		public:
			static constexpr double DefaultLength = 30.0;
			static constexpr double DefaultWidth = 24.0;
			static constexpr double DefaultMaxVelocity = 180.0;
			static constexpr double DefaultMaxAcceleration = 180.0;
			static constexpr double DefaultMaxJerk = 1800.0;
			static constexpr double DefaultTimestep = 0.02;
			static constexpr DriveType DefaultDriveType = DriveType::TankDrive;
			static constexpr const char * DefaultUnits = "in";

		public:
			RobotParams(const std::string &name) {
				name_ = name;
				drivetype_ = DefaultDriveType;
				length_ = DefaultLength;
				width_ = DefaultWidth;
				max_velocity_ = DefaultMaxVelocity;
				max_acceleration_ = DefaultMaxAcceleration;
				max_jerk_ = DefaultMaxJerk;
				units_ = "in";
				timestep_ = 0.02;
			}

			virtual ~RobotParams() {
			}

			static std::vector<DriveType> getDriveTypes() {
				return std::vector<DriveType> { DriveType::TankDrive, DriveType::SwerveDrive };
			}


			const std::string& getName() const {
				return name_;
			}

			void setName(const std::string& name) {
				name_ = name;
			}

			const std::string& getFilename() const {
				return filename_;
			}

			void setFilename(const std::string& file) {
				filename_ = file;
			}

			DriveType getDriveType() const {
				return drivetype_;
			}

			void setDriveType(DriveType t) {
				drivetype_ = t;
			}

			void setUnits(const std::string& units) {
				units_ = units;
			}

			const std::string& getUnits() const {
				return units_;
			}

			void convert(const std::string& units) {
				length_ = UnitConverter::convert(length_, units_, units);
				width_ = UnitConverter::convert(width_, units_, units);
				max_velocity_ = UnitConverter::convert(max_velocity_, units_, units);
				max_acceleration_ = UnitConverter::convert(max_acceleration_, units_, units);
				max_jerk_ = UnitConverter::convert(max_jerk_, units_, units);
				units_ = units;
			}

			double getWidth() const {
				return width_;
			}

			void setWidth(double v) {
				width_ = v;
			}

			double getLength() const {
				return length_;
			}

			void setLength(double v) {
				length_ = v;
			}

			double getMaxVelocity() const {
				return max_velocity_;
			}

			void setMaxVelocity(double v) {
				max_velocity_ = v;
			}

			double getMaxAccel() const {
				return max_acceleration_;
			}

			void setMaxAcceleration(double v) {
				max_acceleration_ = v;
			}

			double getMaxJerk() const {
				return max_jerk_;
			}

			void setMaxJerk(double v) {
				max_jerk_ = v;
			}

			double getTimestep() const {
				return timestep_;
			}

			void setTimestep(double v) {
				timestep_ = v;
			}

		private:
			double width_;
			double length_;
			double max_velocity_;
			double max_acceleration_;
			double max_jerk_;
			std::string units_;
			std::string name_;
			DriveType drivetype_;
			std::string filename_;
			double timestep_;
		};
	}
}

