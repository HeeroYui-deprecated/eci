/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <etk/String.hpp>

namespace eci {
	class Library {
		public:
			Library() {};
			~Library() {};
		protected:
			etk::String m_name; //!< library name (just for debug)
	};
}

