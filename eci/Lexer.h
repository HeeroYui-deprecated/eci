/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_LEXER_H__
#define __ECI_LEXER_H__

#include <etk/types.h>
#include <etk/stdTools.h>
#include <regex>
#include <map>
#include <vector>


namespace eci {
	using LexerResult = std::vector<std::pair<int32_t, std::string>>;
	
	class Lexer {
		private:
			std::map<int32_t, std::regex> m_searchList;
		public:
			Lexer();
			~Lexer();
			void append(int32_t _tokenId, const std::string& _regularExpression);
			LexerResult interprete(const std::string& _data);
	};
}

#endif
