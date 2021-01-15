#pragma once

#include "Translation2d.h"

namespace xero
{
	namespace paths
	{
		class FieldMarker
		{
		public:
			FieldMarker(Translation2d pos, int which) {
				pos_ = pos;
				which_ = which;
			}

			Translation2d pos() const {
				return pos_;
			}

			int which() const {
				return which_;
			}

		private:
			int which_;
			Translation2d pos_;
		};

		static inline bool operator==(const FieldMarker& a, const FieldMarker& b) {
			return a.pos().getX() == b.pos().getX() && a.pos().getY() == b.pos().getY() && a.which() == b.which();
		}
	}
}

