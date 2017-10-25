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
	
	enum cppTokenList {
		tokenCppCommentMultiline = interpreter::typeReserveId,
		tokenCppCommentSingleLine,
		tokenCppPreProcessor,
		tokenCppPreProcessorIf,
		tokenCppPreProcessorElse,
		tokenCppPreProcessorEndif,
		tokenCppPreProcessorIfdef,
		tokenCppPreProcessorIfndef,
		tokenCppPreProcessorDefine,
		tokenCppPreProcessorWarning,
		tokenCppPreProcessorError,
		tokenCppPreProcessorInclude,
		tokenCppPreProcessorImport,
		tokenCppPreProcessorSectionPthese,
		
		tokenCppStringDoubleQuote,
		tokenCppStringSimpleQuote,
		tokenCppBraceIn,
		tokenCppBraceOut,
		tokenCppPtheseIn,
		tokenCppPtheseOut,
		tokenCppHookIn,
		tokenCppHookOut,
		tokenCppSectionBrace,
		tokenCppSectionPthese,
		tokenCppSectionHook,
		tokenCppBranch,
		tokenCppSystem,
		tokenCppType,
		tokenCppVisibility,
		tokenCppContener,
		tokenCppTypeDef,
		tokenCppAuto,
		tokenCppNullptr,
		tokenCppSystemDefine,
		tokenCppNumericValue,
		tokenCppBoolean,
		tokenCppCondition,
		tokenCppAssignation,
		tokenCppString,
		tokenCppSeparator,
	};
	class ParserCpp {
		public:
			eci::Lexer m_lexer;
			eci::LexerResult m_result;
		public:
			ParserCpp();
			~ParserCpp();
			bool parse(const etk::String& _data);
	};
}
