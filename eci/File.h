/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_FILE_H__
#define __ECI_FILE_H__

#include <etk/types.h>
#include <eci/Class.h>
#include <eci/Enum.h>
#include <eci/Variable.h>
#include <eci/Function.h>

namespace eci {
	class File {
		public:
			File(const std::string& _filename);
			~File() {};
		protected:
			std::string m_fileName; //!< Name of the file.
			std::string m_fileData; //!< Data of the file.
			std::vector<std::shared_ptr<eci::Function>> m_listFunction; // all function in the file
			std::vector<std::shared_ptr<eci::Class>> m_listClass; // all class in the file
			std::vector<std::shared_ptr<eci::Variable>> m_listVariable; // all variable in the file
	};
}

#endif
