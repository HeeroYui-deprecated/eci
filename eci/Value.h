/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_VALUE_H__
#define __ECI_VALUE_H__

#include <etk/types.h>
#include <eci/Type.h>
#include <memory>

namespace eci {
	class Value : public std::enable_shared_from_this<Value>{
		public:
			Value() {};
			~Value() {};
		protected:
			
	};
}

#endif
