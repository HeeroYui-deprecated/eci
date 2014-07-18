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

int main(int argc, char** argv) {
	ECI_INFO("Start Application interpeter languages");
	if (argc<=1) {
		ECI_CRITICAL("need the file to parse");
		return -1;
	}
	eci::ParserCpp tmpParser;
	std::string data = "/* plop */ \n int main(void) {\n return 0;\n}\n";//etk::FSNodeReadAllData(argv[1]);
	tmpParser.parse(data);
	
	return 0;
}