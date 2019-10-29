#pragma once

#include <string>

namespace xero
{
	namespace paths
	{
		class PathFlag
		{
		public:
			PathFlag(const std::string& name, double after, double before) {
				name_ = name;
				before_ = before;
				after_ = after;
			}

			virtual ~PathFlag() {
			}

			const std::string& name() const {
				return name_;
			}

			void setName(const std::string & name) {
				name_ = name;
			}

			double before() const {
				return before_;
			}

			void setBefore(double v) {
				before_ = v;
			}

			double after() const {
				return after_;
			}

			void setAfter(double v) {
				after_ = v;
			}

		private:
			double after_;
			double before_;
			std::string name_;
		};
	}
}

