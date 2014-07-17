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
	m_searchList.insert(std::make_pair(_tokenId, std::regex(_regularExpression)));
}

eci::LexerResult eci::Lexer::interprete(const std::string& _data) {
	eci::LexerResult result;
	for (auto &it : m_searchList) {
		ECI_INFO("Parse RegEx : " << it.first);// << " : '" << it.second.str() << "'");
		std::sregex_iterator it_search(_data.begin(), _data.end(), it.second);
		std::sregex_iterator it_end;
		for (std::sregex_iterator i = it_search; i != it_end; ++i) {
			std::smatch match = *i;
			std::string match_str = match.str();
			ECI_INFO("    " << match_str);
		}
	}
	return result;
}

