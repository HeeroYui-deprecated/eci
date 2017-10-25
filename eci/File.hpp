/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <eci/Class.hpp>
#include <eci/Enum.hpp>
#include <eci/Variable.hpp>
#include <eci/Function.hpp>

namespace eci {
	class File {
		public:
			File(const etk::String& _filename);
			~File() {};
		protected:
			etk::String m_fileName; //!< Name of the file.
			etk::String m_fileData; //!< Data of the file.
			etk::Vector<ememory::SharedPtr<eci::Function>> m_listFunction; // all function in the file
			etk::Vector<ememory::SharedPtr<eci::Class>> m_listClass; // all class in the file
			etk::Vector<ememory::SharedPtr<eci::Variable>> m_listVariable; // all variable in the file
	};
}

