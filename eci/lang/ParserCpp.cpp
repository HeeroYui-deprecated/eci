/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/lang/ParserCpp.hpp>
#include <eci/debug.hpp>


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
	//m_lexer.appendSubSection(tokenCppPreProcessor, tokenCppPreProcessorSectionPthese, "\\(", "\\)");
	m_lexer.append(tokenCppStringDoubleQuote, "\"(.|\\\\[\\\\\"])*?\"");
	m_lexer.append(tokenCppStringSimpleQuote, "'\\?.'");
	m_lexer.append(tokenCppBraceIn, "\\{");
	m_lexer.append(tokenCppBraceOut, "\\}");
	m_lexer.append(tokenCppPtheseIn, "\\(");
	m_lexer.append(tokenCppPtheseOut, "\\)");
	m_lexer.append(tokenCppHookIn, "\\[");
	m_lexer.append(tokenCppHookOut, "\\]");
	m_lexer.append(tokenCppBranch, "\\b(return|goto|if|else|case|default|break|continue|while|do|for)\\b");
	m_lexer.append(tokenCppSystem, "\\b(new|delete|try|catch)\\b");
	m_lexer.append(tokenCppType, "\\b(bool|char(16_t|32_t)?|double|float|u?int(8|16|32|64|128)?(_t)?|long|short|signed|size_t|unsigned|void)\\b");
	m_lexer.append(tokenCppVisibility, "\\b(inline|const|virtual|private|public|protected|friend|const|extern|register|static|volatile)\\b");
	m_lexer.append(tokenCppContener, "\\b(class|namespace|struct|union|enum)\\b");
	m_lexer.append(tokenCppTypeDef, "\\btypedef\\b");
	m_lexer.append(tokenCppAuto, "\\bauto\\b");
	m_lexer.append(tokenCppNullptr, "\\b(NULL|nullptr)\\b");
	m_lexer.append(tokenCppSystemDefine, "\\b__(LINE|DATA|FILE|func|TIME|STDC)__\\b");
	m_lexer.append(tokenCppNumericValue, "\\b(((0(x|X)[0-9a-fA-F]*)|(\\d+\\.?\\d*|\\.\\d+)((e|E)(\\+|\\-)?\\d+)?)(L|l|UL|ul|u|U|F|f)?)\\b");
	m_lexer.append(tokenCppBoolean, "\\b(true|false)\\b");
	m_lexer.append(tokenCppCondition, "==|>=|<=|!=|<|>|&&|\\|\\|");
	m_lexer.append(tokenCppAssignation, "(\\+=|-=|\\*=|/=|=|\\*|/|--|-|\\+\\+|\\+|&)");
	m_lexer.append(tokenCppString, "\\w+");
	m_lexer.append(tokenCppSeparator, "(;|,|::|:)");
	m_lexer.appendSection(tokenCppSectionBrace, tokenCppBraceIn, tokenCppBraceOut, "{}");
	m_lexer.appendSection(tokenCppSectionPthese, tokenCppPtheseIn, tokenCppPtheseOut, "()");
	m_lexer.appendSection(tokenCppSectionHook, tokenCppHookIn, tokenCppHookOut, "[]");
}

eci::ParserCpp::~ParserCpp() {
	
}

static void printNode(const etk::String& _data, const etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _nodes, int32_t _level=0) {
	etk::String offset;
	for (int32_t iii=0; iii<_level; ++iii) {
		offset += "    ";
	}
	for (auto &it : _nodes) {
		if (it->isNodeContainer() == true) {
			ememory::SharedPtr<eci::LexerNodeContainer> sec = ememory::dynamicPointerCast<eci::LexerNodeContainer>(it);
			if (sec != nullptr) {
				ECI_INFO(offset << "  " << sec->getStartPos() << "->" << sec->getStopPos() << " container: " << sec->getType());
				printNode(_data, sec->m_list, _level+1);
			}
		} else {
			ECI_INFO(offset << it->getStartPos() << "->" << it->getStopPos() << " data='" <<etk::String(_data, it->getStartPos(), it->getStopPos()-it->getStartPos()) << "'" );
		}
	}
}

bool eci::ParserCpp::parse(const etk::String& _data) {
	m_result = m_lexer.interprete(_data);
	
	// TODO: Agregate type
	// TODO: Agregate Action
	// TODO: Agregate Function
	
	ECI_INFO("find :");
	printNode(_data, m_result.m_list);
	/*
	for (auto &it : m_result.m_list) {
		ECI_INFO("    start=" << it->getStartPos() << " stop=" << it->getStopPos() << " data='" <<etk::String(_data, it->getStartPos(), it->getStopPos()-it->getStartPos()) << "'" );
	}
	*/
	return false;
}

