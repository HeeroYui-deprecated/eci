/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_TYPE_H__
#define __ECI_TYPE_H__

#include <etk/types.h>

namespace eci {
	class Type {
		protected:
			std::string m_signature;
		public:
			Type() {};
			~Type() {};
	};
	template<typename T> class TypeBase {
		
	};
	
}

#endif