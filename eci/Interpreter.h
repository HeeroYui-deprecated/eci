/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_INTERPRETER_H__
#define __ECI_INTERPRETER_H__

#include <etk/types.h>
#include <eci/Library.h>
#include <eci/File.h>

namespace eci {
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();
		protected:
			std::vector<eci::Library> m_libraries; //!< list of all loaded libraries.
			std::vector<eci::File> m_files; //!< List of all files in the current program.
		public:
			void addFile(const std::string& _filename);
			void main();
	};
}

#endif
