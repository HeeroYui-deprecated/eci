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

void run_interactive() {
	ECI_CRITICAL("TODO ... create interactive interface");
}

bool run_test(const std::string& _filename) {
	eci::Interpreter::Interpreter virtualMachine;
	virtualMachine.addFile(_filename);
	
	virtualMachine.main();
	return false;
}

void run_test(const std::vector<std::string>& _listFileToTest) {
	
	int32_t test_num = 1;
	int32_t count = 0;
	int32_t passed = 0;
	for (auto &it : _listFileToTest) {
		enum etk::typeNode type = etk::FSNode(it).getNodeType();
		if (type == etk::FSN_FOLDER) {
			etk::FSNode node(it);
			std::vector<std::string> list;
			node.folderGetRecursiveFiles(list, false);
			for (auto &it2 : list) {
				if (run_test(it2)) {
					passed++;
				}
				count++;
				test_num++;
			}
		} else if (type == etk::FSN_FILE) {
			if (run_test(it)) {
				passed++;
			}
			count++;
			test_num++;
		}
	}
	ECI_INFO("Done. " << count << " tests, " << passed << " pass, " << count-passed << " fail");
}


int main(int argc, char** argv) {
	// the only one init for etk:
	std::vector<std::string> listFileToTest;
	for (int32_t iii=1; iii<argc ; ++iii) {
		std::string data = argv[iii];
		if (data == "-l0") {
			etk::log::setLevel(etk::log::logLevelNone);
		} else if (data == "-l1") {
			etk::log::setLevel(etk::log::logLevelCritical);
		} else if (data == "-l2") {
			etk::log::setLevel(etk::log::logLevelError);
		} else if (data == "-l3") {
			etk::log::setLevel(etk::log::logLevelWarning);
		} else if (data == "-l4") {
			etk::log::setLevel(etk::log::logLevelInfo);
		} else if (data == "-l5") {
			etk::log::setLevel(etk::log::logLevelDebug);
		} else if (data == "-l6") {
			etk::log::setLevel(etk::log::logLevelVerbose);
		} else if (    data == "-h"
		            || data == "--help") {
			etk::log::setLevel(etk::log::logLevelVerbose);
			ECI_INFO("Help : ");
			ECI_INFO("    ./xxx [options]");
			ECI_INFO("        -l0: debug None");
			ECI_INFO("        -l1: debug Critical");
			ECI_INFO("        -l2: debug Error");
			ECI_INFO("        -l3: debug Warning");
			ECI_INFO("        -l4: debug Info");
			ECI_INFO("        -l5: debug Debug");
			ECI_INFO("        -l6: debug Verbose");
			ECI_INFO("        -h/--help: this help");
			exit(0);
		} else {
			listFileToTest.push_back(data);
		}
	}
	etk::setArgZero(argv[0]);
	
	etk::initDefaultFolder("exml_test");
	ECI_INFO("input elements : " << listFileToTest);
	// Cocal parse :
	if (listFileToTest.size() == 0) {
		run_interactive();
	} else {
		run_test(listFileToTest);
	}
	
	return 0;
}