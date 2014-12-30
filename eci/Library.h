/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_LIBRARY_H__
#define __ECI_LIBRARY_H__

#include <etk/types.h>

namespace eci {
	class Library {
		public:
			Library() {};
			~Library() {};
		protected:
			std::string m_name; //!< library name (just for debug)
	};
}

#endif
