/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/Interpreter.h>
#include <eci/debug.h>

eci::Interpreter::Interpreter() {
	
}

eci::Interpreter::~Interpreter() {
	
}

void eci::Interpreter::addFile(const std::string& _filename) {
	// TODO : Check if file is not previously loaded ...
	m_files.push_back(eci::File(_filename));
}

void eci::Interpreter::main() {
	ECI_TODO("create the main ... ");
}


