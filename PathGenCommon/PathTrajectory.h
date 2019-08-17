#pragma once

#include "Pose2dWithTrajectory.h"
#include <vector>
#include <string>

namespace xero
{
	namespace paths
	{
		class PathTrajectory
		{
		public:
			PathTrajectory(const std::string& name, const std::vector<Pose2dWithTrajectory>& pts) {
				name_ = name;
				points_ = pts;
			}
			typedef std::vector<Pose2dWithTrajectory>::iterator iterator;
			typedef std::vector<Pose2dWithTrajectory>::const_iterator const_iterator;

			iterator begin() {
				return points_.begin();
			}

			iterator end() {
				return points_.end();
			}

			const_iterator begin() const {
				return points_.begin();
			}

			const_iterator end() const {
				return points_.end();
			}

			size_t size() const {
				return points_.size();
			}

			const Pose2dWithTrajectory& operator[](size_t index) {
				return points_[index];
			}

			const std::string& name() const {
				return name_;
			}

		private:
			std::string name_;
			std::vector<Pose2dWithTrajectory> points_;
		};
	}
}

