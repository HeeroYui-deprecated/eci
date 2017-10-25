/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <eci/visibility.hpp>
#include <eci/Type.hpp>
#include <ememory/memory.hpp>


namespace eci {
	class Variable : public ememory::EnableSharedFromThis<Variable> {
		public:
			Variable();
			virtual ~Variable();
		private:
			enum eci::visibility m_visibility;
			bool m_const;
			etk::String m_name;
			ememory::SharedPtr<eci::Type> m_type;
	};
}
