/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_PARSER_JS_H__
#define __ECI_PARSER_JS_H__

#include <eci/Lexer.h>
#include <eci/Interpreter.h>

namespace eci {
	
	enum jsTokenList {
		tokenJSCommentMultiline = interpreter::typeReserveId,
		tokenJSCommentSingleLine,
		
		tokenJSStringDoubleQuote,
		tokenJSStringSimpleQuote,
		tokenJSBraceIn,
		tokenJSBraceOut,
		tokenJSPtheseIn,
		tokenJSPtheseOut,
		tokenJSHookIn,
		tokenJSHookOut,
		tokenJSSectionBrace,
		tokenJSSectionPthese,
		tokenJSSectionHook,
		tokenJSBranch,
		tokenJSSystem,
		tokenJSType,
		tokenJSContener,
		tokenJSNumericValue,
		tokenJSBoolean,
		tokenJSCondition,
		tokenJSAssignation,
		tokenJSString,
		tokenJSSeparator,
	};
	class ParserJS {
		public:
			eci::Lexer m_lexer;
			eci::LexerResult m_result;
		public:
			ParserJS();
			~ParserJS();
			bool parse(const std::string& _data);
	};
}

#endif
