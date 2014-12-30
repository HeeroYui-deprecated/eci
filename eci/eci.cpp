/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/eci.h>
#include <eci/debug.h>
#include <eci/lang/ParserCpp.h>
#include <etk/os/FSNode.h>
#include <eci/Interpreter.h>

int main(int argc, char** argv) {
	etk::log::setLevel(etk::log::logLevelDebug);
	//etk::log::setLevel(etk::log::logLevelInfo);
	ECI_INFO("Start Application interpeter languages");
	if (argc<=1) {
		ECI_CRITICAL("need the file to parse");
		return -1;
	}
	//eci::ParserCpp tmpParser;
	//std::string data = "/* plop */ \n int eee = 22; // error value \nint main(void) {\n return 0;\n}\n";//etk::FSNodeReadAllData(argv[1]);
	//std::string data = "alpha /* plop */ test";
	//std::string data = "pp \n // qdfqdfsdf \nde";
	//tmpParser.parse(data);
	//tmpParser.parse(etk::FSNodeReadAllData(argv[1]));
	
	eci::Interpreter::Interpreter virtualMachine;
	virtualMachine.addFile(argv[1]);
	
	virtualMachine.main();
	
	return 0;
}