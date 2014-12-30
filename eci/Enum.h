/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_ENUM_H__
#define __ECI_ENUM_H__

#include <etk/types.h>

namespace eci {
	class Enum {
		public:
			Enum() {};
			~Enum() {};
		protected:
			std::vector<std::pair<std::string, int32_t>> m_values;
		public:
			void addValue(const std::string& _name);
			void addValue(const std::string& _name, int32_t _value);
			int32_t getValue(const std::string& _name) const;
			const std::string& getName(int32_t _value) const;
	};
}

#endif
