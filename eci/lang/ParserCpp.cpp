/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/lang/ParserCpp.h>
#include <eci/debug.h>

enum cppTokenList {
	tokenCppCommentMultiline,
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

eci::ParserCpp::ParserCpp() {
	m_lexer.append(tokenCppCommentMultiline, "/\\*(.|\\r|\\n)*?(\\*/|\\0)");
	m_lexer.append(tokenCppCommentSingleLine, "//.*");
	m_lexer.append(tokenCppPreProcessor, "#(.|\\\\[\\\\\\n])*");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorIf, "\\bif\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorElse, "\\belse\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorEndif, "\\bendif\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorIfdef, "\\bifdef\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorIfndef, "\\bifndef\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorDefine, "\\bdefine\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorWarning, "\\bwarning\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorError, "\\berror\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorInclude, "\\binclude\\b");
	m_lexer.appendSub(tokenCppPreProcessor, tokenCppPreProcessorImport, "\\bimport\\b"); // specific to c++ interpreted
	m_lexer.appendSubSection(tokenCppPreProcessor, tokenCppPreProcessorSectionPthese, "\\(", "\\)");
	m_lexer.append(tokenCppStringDoubleQuote, "\"(.|\\\\[\\\\\"])*?\"");
	m_lexer.append(tokenCppStringSimpleQuote, "'\\?.'");
	m_lexer.appendSection(tokenCppSectionBrace, "\\{", "\\}");
	m_lexer.appendSection(tokenCppSectionPthese, "\\(", "\\)");
	m_lexer.appendSection(tokenCppSectionHook, "\\[", "\\]");
	m_lexer.append(tokenCppBranch, "\\b(return|goto|if|else|case|default|break|continue|while|do|for)\\b");
	m_lexer.append(tokenCppSystem, "\\b(new|delete|try|catch)\\b");
	m_lexer.append(tokenCppType, "\\b(bool|char(16_t|32_t)?|double|float|u?int(8|16|32|64|128)?(_t)?|long|short|signed|size_t|unsigned|void|(I|U)(8|16|32|64|128))\\b");
	m_lexer.append(tokenCppVisibility, "\\b(inline|const|virtual|private|public|protected|friend|const|extern|register|static|volatile)\\b");
	m_lexer.append(tokenCppContener, "\\b(class|namespace|struct|union|enum)\\b");
	m_lexer.append(tokenCppTypeDef, "\\btypedef\\b");
	m_lexer.append(tokenCppAuto, "\\bauto\\b");
	m_lexer.append(tokenCppNullptr, "\\b(NULL|nullptr)\\b");
	m_lexer.append(tokenCppSystemDefine, "\\b__(LINE|DATA|FILE|func|TIME|STDC)__\\b");
	m_lexer.append(tokenCppNumericValue, "\\b(((0(x|X)[0-9a-fA-F]*)|(\\d+\\.?\\d*|\\.\\d+)((e|E)(\\+|\\-)?\\d+)?)(L|l|UL|ul|u|U|F|f)?)\\b");
	m_lexer.append(tokenCppBoolean, "\\b(true|false)\\b");
	m_lexer.append(tokenCppCondition, "==|>=|<=|!=|<|>|&&|\\|\\|");
	m_lexer.append(tokenCppAssignation, "(=|\\*|/|-|+|&)");
	m_lexer.append(tokenCppString, "\\w+");
	m_lexer.append(tokenCppSeparator, "(;|,|::|:)");
}

eci::ParserCpp::~ParserCpp() {
	
}

bool eci::ParserCpp::parse(const std::string& _data) {
	m_result = m_lexer.interprete(_data);
		ECI_INFO("find :");
	for (auto &it : m_result.m_list) {
		ECI_INFO("    start=" << it->getStartPos() << " stop=" << it->getStopPos() << " data='" <<std::string(_data, it->getStartPos(), it->getStopPos()-it->getStartPos()) << "'" );
	}
	return false;
}

