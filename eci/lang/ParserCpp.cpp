/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/lang/ParserCpp.h>

enum cppTokenList {
	tokenCppMultilineComment,
	tokenCppSingleLineComment,
	
};

eci::ParserCpp::ParserCpp() {
	m_lexer.append(tokenCppMultilineComment, "/\\*.*\\*/");
	m_lexer.append(tokenCppSingleLineComment, "//$");
}

eci::ParserCpp::~ParserCpp() {
	
}

bool eci::ParserCpp::parse(const std::string& _data) {
	m_result = m_lexer.interprete(_data);
	return false;
}

