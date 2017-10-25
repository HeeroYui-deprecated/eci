/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <eci/Lexer.hpp>
#include <eci/Interpreter.hpp>

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
			bool parse(const etk::String& _data);
	};
}
