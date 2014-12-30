/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_FUNCTION_H__
#define __ECI_FUNCTION_H__

#include <etk/types.h>
#include <eci/visibility.h>
#include <eci/Variable.h>
#include <eci/Value.h>
#include <memory>

namespace eci {
	class Function {
		public:
			Function();
			~Function();
		protected:
			std::string m_name; //!< Function Name.
			bool m_const; //!< The function is const.
			bool m_static; //!< function is static.
			enum eci::visibility m_visibility; //!< Visibility of the function
			std::vector<eci::Variable> m_return; //!< return value.
			std::vector<eci::Variable> m_arguments; //!< return value.
			
			std::vector<std::shared_ptr<eci::Value>> call(const std::vector<std::shared_ptr<eci::Value>>& _input);
	};
}

#endif
