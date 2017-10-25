/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/Interpreter.hpp>
#include <eci/debug.hpp>

eci::Interpreter::Interpreter() {
	
}

eci::Interpreter::~Interpreter() {
	
}

void eci::Interpreter::addFile(const etk::String& _filename) {
	// TODO : Check if file is not previously loaded ...
	m_files.pushBack(eci::File(_filename));
}

void eci::Interpreter::main() {
	ECI_TODO("create the main ... ");
}


