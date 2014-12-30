/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_VARIABLE_H__
#define __ECI_VARIABLE_H__

#include <etk/types.h>
#include <eci/visibility.h>
#include <eci/Type.h>


namespace eci {
	class Variable {
		public:
			Variable();
			~Variable();
		private:
			enum eci::visibility m_visibility;
			bool m_const;
			std::string m_name;
			eci::Type m_type;
	};
}

#endif
