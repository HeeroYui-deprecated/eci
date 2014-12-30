/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/File.h>
#include <eci/debug.h>
#include <etk/os/FSNode.h>
#include <eci/lang/ParserCpp.h>


static std::string getValue(const std::string& _file, const std::shared_ptr<eci::LexerNode>& _it) {
	return std::string(_file, _it->getStartPos(), _it->getStopPos()-_it->getStartPos());
}

eci::File::File(const std::string& _filename) {
	m_fileName = _filename;
	m_fileData = etk::FSNodeReadAllData(_filename);
	eci::ParserCpp tmpParser;
	tmpParser.parse(m_fileData);
	std::string value;
	for (auto &it : tmpParser.m_result.m_list) {
		switch (it->getTockenId()) {
			case tokenCppVisibility:
				ECI_INFO("get visibility : " << getValue(m_fileData, it) << "'" );
				break;
			case tokenCppType:
				ECI_INFO("get type : " << getValue(m_fileData, it) << "'" );
				break;
			case tokenCppString:
				ECI_INFO("get string : " << getValue(m_fileData, it) << "'" );
				break;
		}
	}
}

