/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_PARSER_CPP_H__
#define __ECI_PARSER_CPP_H__

#include <eci/Lexer.h>

namespace eci {
	class ParserCpp {
		private:
			eci::Lexer m_lexer;
			eci::LexerResult m_result;
		public:
			ParserCpp();
			~ParserCpp();
			bool parse(const std::string& _data);
	};
}

#endif
