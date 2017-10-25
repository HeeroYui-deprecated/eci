/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <eci/visibility.hpp>
#include <eci/Variable.hpp>
#include <eci/Value.hpp>
#include <ememory/memory.hpp>

namespace eci {
	class Function {
		public:
			Function();
			~Function();
		protected:
			etk::String m_name; //!< Function Name.
			bool m_const; //!< The function is const.
			bool m_static; //!< function is static.
			enum eci::visibility m_visibility; //!< Visibility of the function
			etk::Vector<eci::Variable> m_return; //!< return value.
			etk::Vector<eci::Variable> m_arguments; //!< return value.
			
			etk::Vector<ememory::SharedPtr<eci::Value>> call(const etk::Vector<ememory::SharedPtr<eci::Value>>& _input);
			
			// 3 step:
			//    - first get Tockens (returns , names, const, parameters, codes
			//    - interpreted all of this ... no link on variables
			//    - all is linked
	};
}

