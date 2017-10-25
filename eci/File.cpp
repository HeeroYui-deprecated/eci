/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/File.hpp>
#include <eci/debug.hpp>
#include <etk/os/FSNode.hpp>
#include <eci/lang/ParserCpp.hpp>
#include <eci/lang/ParserJS.hpp>


static etk::String getValue(const etk::String& _file, const ememory::SharedPtr<eci::LexerNode>& _it) {
	return etk::String(_file, _it->getStartPos(), _it->getStopPos()-_it->getStartPos());
}

eci::Variable getVariableWithType(const etk::String& _value) {
	eci::Variable ret;
	if (_value == "void") {
		
	} else if (    _value == "int"
	            || _value == "int32_t"
	            || _value == "signed int") {
		
	} else if (    _value == "unsigned int"
	            || _value == "uint32_t") {
		
	} else if (    _value == "short"
	            || _value == "int16_t"
	            || _value == "signed short") {
		
	} else if (    _value == "unsigned short"
	            || _value == "uint16_t") {
		
	} else if (    _value == "char"
	            || _value == "int8_t"
	            || _value == "signed char") {
		
	} else if (    _value == "unsigned char"
	            || _value == "uint8_t") {
		
	} else if (    _value == "long"
	            || _value == "int64_t"
	            || _value == "signed long") {
		
	} else if (    _value == "unsigned long"
	            || _value == "uint64_t") {
		
	} else if (_value == "bool") {
		
	} else if (_value == "size_t") {
		
	} else {
		ECI_ERROR("get variable with type : " << _value << "' << NOT parsed !!!!" );
	}
	return ret;
}

eci::File::File(const etk::String& _filename) {
	m_fileName = _filename;
	m_fileData = etk::FSNodeReadAllData(m_fileName);
	if (    etk::end_with(m_fileName, "cpp", false) == true
	     || etk::end_with(m_fileName, "cxx", false) == true
	     || etk::end_with(m_fileName, "c", false) == true
	     || etk::end_with(m_fileName, "hpp", false) == true
	     || etk::end_with(m_fileName, "hxx", false) == true
	     || etk::end_with(m_fileName, "h", false) == true) {
		eci::ParserCpp tmpParser;
		tmpParser.parse(m_fileData);
		
		// all we need all the time:
		etk::Vector<eci::Variable> returnList;
		etk::Vector<eci::Variable> argumentList;
		etk::String name;
		etk::String value;
		ememory::SharedPtr<eci::Class> lastClass;
		ememory::SharedPtr<eci::Function> lastFunction;
		ememory::SharedPtr<eci::Variable> lastVariable;
		enum eci::visibility lastVisibility = eci::visibilityPublic;
		
		for (auto &it : tmpParser.m_result.m_list) {
			value = getValue(m_fileData, it);
			switch (it->getTockenId()) {
				case tokenCppVisibility:
					ECI_INFO("get visibility : " << value << "'" );
					if (value == "private") {
						lastVisibility = eci::visibilityPrivate;
					} else if (value == "public") {
						lastVisibility = eci::visibilityPublic;
					} else if (value == "protected") {
						lastVisibility = eci::visibilityProtected;
					//} else if (value == "inline") {
						
					//} else if (value == "const") {
						
					//} else if (value == "virtual") {
						
					//} else if (value == "friend") {
						
					//} else if (value == "extern") {
						
					//} else if (value == "register") {
						
					//} else if (value == "static") {
						
					//} else if (value == "volatile") {
						
					} else {
						ECI_ERROR("get visibility : " << value << "' << NOT parsed !!!!" );
					}
					break;
				case tokenCppType:
					ECI_INFO("get type : " << value << "'" );
					if (name == "") {
						returnList.pushBack(getVariableWithType(value));
					} else {
						ECI_ERROR("      get type : " << value << "' after name !!!" );
					}
					break;
				case tokenCppString:
					ECI_INFO("get string : " << value << "'" );
					name = value;
					break;
			}
		}
	} else if (etk::end_with(m_fileName, "js", false) == true) {
		eci::ParserJS tmpParser;
		tmpParser.parse(m_fileData);
		
	} else {
		ECI_CRITICAL("Unknow file type ... '" << m_fileName << "'");
	}
}

