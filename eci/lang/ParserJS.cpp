/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/lang/ParserJS.hpp>
#include <eci/debug.hpp>


eci::ParserJS::ParserJS() {
	m_lexer.append(tokenJSCommentMultiline, "/\\*(.|\\r|\\n)*?(\\*/|\\0)");
	m_lexer.append(tokenJSCommentSingleLine, "//.*");
	m_lexer.append(tokenJSStringDoubleQuote, "\"(.|\\\\[\\\\\"])*?\"");
	m_lexer.append(tokenJSStringSimpleQuote, "'\\?.'");
	m_lexer.append(tokenJSBraceIn, "\\{");
	m_lexer.append(tokenJSBraceOut, "\\}");
	m_lexer.append(tokenJSPtheseIn, "\\(");
	m_lexer.append(tokenJSPtheseOut, "\\)");
	m_lexer.append(tokenJSHookIn, "\\[");
	m_lexer.append(tokenJSHookOut, "\\]");
	m_lexer.append(tokenJSBranch, "\\b(return|if|else|while|do|for)\\b");
	m_lexer.append(tokenJSType, "\\b(bool|char(16_t|32_t)?|double|float|u?int(8|16|32|64|128)?(_t)?|long|short|signed|size_t|unsigned|void)\\b");
	m_lexer.append(tokenJSContener, "\\b(var|function)\\b");
	m_lexer.append(tokenJSNumericValue, "\\b(((0(x|X)[0-9a-fA-F]*)|(\\d+\\.?\\d*|\\.\\d+)((e|E)(\\+|\\-)?\\d+)?)(L|l|UL|ul|u|U|F|f)?)\\b");
	m_lexer.append(tokenJSBoolean, "\\b(true|false)\\b");
	m_lexer.append(tokenJSCondition, "===|!==|==|>=|<=|!=|<|>|&&|\\|\\|");
	m_lexer.append(tokenJSAssignation, "(\\+=|-=|\\*=|/=|=|\\*|/|--|-|\\+\\+|\\+|&)");
	m_lexer.append(tokenJSString, "\\w+");
	m_lexer.append(tokenJSSeparator, "(;|,)");
	m_lexer.appendSection(tokenJSSectionBrace, tokenJSBraceIn, tokenJSBraceOut, "{}");
	m_lexer.appendSection(tokenJSSectionPthese, tokenJSPtheseIn, tokenJSPtheseOut, "()");
	m_lexer.appendSection(tokenJSSectionHook, tokenJSHookIn, tokenJSHookOut, "[]");
}

eci::ParserJS::~ParserJS() {
	
}

static void printNode(const etk::String& _data, const etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _nodes, int32_t _level=0) {
	etk::String offset;
	for (int32_t iii=0; iii<_level; ++iii) {
		offset += "    ";
	}
	for (auto &it : _nodes) {
		if (it->isNodeContainer() == true) {
			ememory::SharedPtr<eci::LexerNodeContainer> sec = ememory::dynamicPointerCast<eci::LexerNodeContainer>(it);
			if (sec != null) {
				ECI_INFO(offset << "  " << sec->getStartPos() << "->" << sec->getStopPos() << " container: " << sec->getType());
				printNode(_data, sec->m_list, _level+1);
			}
		} else {
			ECI_INFO(offset << it->getStartPos() << "->" << it->getStopPos() << " data='" <<etk::String(_data, it->getStartPos(), it->getStopPos()-it->getStartPos()) << "'" );
		}
	}
}

bool eci::ParserJS::parse(const etk::String& _data) {
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

