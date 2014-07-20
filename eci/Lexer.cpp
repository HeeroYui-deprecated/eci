/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/Lexer.h>
#include <eci/debug.h>

eci::Lexer::Lexer() {
	
}

eci::Lexer::~Lexer() {
	
}

void eci::Lexer::append(int32_t _tokenId, const std::string& _regularExpression) {
	m_searchList.insert(std::make_pair(_tokenId, etk::RegExp<std::string>(_regularExpression)));
	etk::RegExp<std::string>(_regularExpression).display();
}

eci::LexerResult eci::Lexer::interprete(const std::string& _data) {
	eci::LexerResult result;
	ECI_INFO("Parse : \n" << _data);
	for (auto &it : m_searchList) {
		ECI_INFO("Parse RegEx : " << it.first << " : " << it.second.getRegExDecorated());
		if (it.second.parse(_data, 0, _data.size()) == true) {
			ECI_INFO("    match [" << it.second.start() << ".." << it.second.stop() << "] ");
			ECI_INFO("        ==> '" << std::string(_data, it.second.start(), it.second.stop()-it.second.start()) << "'");
		}
	}
	return result;
}

