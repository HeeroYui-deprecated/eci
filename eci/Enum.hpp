/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <etk/Pair.hpp>
#include <etk/Vector.hpp>

namespace eci {
	class Enum {
		public:
			Enum() {};
			~Enum() {};
		protected:
			etk::Vector<etk::Pair<etk::String, int32_t>> m_values;
		public:
			void addValue(const etk::String& _name);
			void addValue(const etk::String& _name, int32_t _value);
			int32_t getValue(const etk::String& _name) const;
			const etk::String& getName(int32_t _value) const;
	};
}
