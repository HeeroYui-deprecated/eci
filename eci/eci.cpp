/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/eci.hpp>
#include <eci/debug.hpp>
#include <eci/lang/ParserCpp.hpp>
#include <etk/os/FSNode.hpp>
#include <eci/Interpreter.hpp>
#include <etk/etk.hpp>

void run_interactive() {
	ECI_CRITICAL("TODO ... create interactive interface");
}

bool run_test(const etk::String& _filename) {
	eci::Interpreter virtualMachine;
	virtualMachine.addFile(_filename);
	
	virtualMachine.main();
	return false;
}

void run_test(const etk::Vector<etk::String>& _listFileToTest) {
	
	int32_t test_num = 1;
	int32_t count = 0;
	int32_t passed = 0;
	for (auto &it : _listFileToTest) {
		enum etk::typeNode type = etk::FSNode(it).getNodeType();
		if (type == etk::typeNode_folder) {
			etk::FSNode node(it);
			etk::Vector<etk::String> list;
			node.folderGetRecursiveFiles(list, false);
			for (auto &it2 : list) {
				if (run_test(it2)) {
					passed++;
				}
				count++;
				test_num++;
			}
		} else if (type == etk::typeNode_file) {
			if (run_test(it)) {
				passed++;
			}
			count++;
			test_num++;
		}
	}
	ECI_INFO("Done. " << count << " tests, " << passed << " pass, " << count-passed << " fail");
}


int main(int _argc, const char** _argv) {
	etk::init(_argc, _argv);
	// the only one init for etk:
	etk::Vector<etk::String> listFileToTest;
	for (int32_t iii=1; iii<_argc ; ++iii) {
		etk::String data = _argv[iii];
		if (    data == "-h"
		     || data == "--help") {
			ECI_PRINT("Help : ");
			ECI_PRINT("    ./xxx [options]");
			ECI_PRINT("        No option for now");
			exit(0);
		} else if (    data.startWith("--elog-") == false
		            && data.startWith("--etk-") == false) {
			listFileToTest.pushBack(data);
		}
	}
	ECI_INFO("input elements: " << listFileToTest);
	// Cocal parse :
	if (listFileToTest.size() == 0) {
		run_interactive();
	} else {
		run_test(listFileToTest);
	}
	
	return 0;
}