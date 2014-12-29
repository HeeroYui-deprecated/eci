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
	ECI_INFO("CPP lexer add : '" << _regularExpression << "'");
	try {
		m_searchList.insert(std::make_pair(_tokenId, std::make_shared<eci::Lexer::TypeBase>(_tokenId, _regularExpression)));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpression << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSection(int32_t _tokenId, const std::string& _regularExpressionStart, const std::string& _regularExpressionStop) {
	ECI_INFO("CPP lexer add section : '" << _regularExpressionStart << "' .. '" << _regularExpressionStop << "'");
	try {
		m_searchList.insert(std::make_pair(_tokenId, std::make_shared<eci::Lexer::TypeSection>(_tokenId, _regularExpressionStart, _regularExpressionStop)));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpressionStart << "' .. '" << _regularExpressionStop << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSub(int32_t _tokenIdParrent, int32_t _tokenId, const std::string& _regularExpression) {
	ECI_INFO("CPP lexer add sub : [" << _tokenIdParrent << "] '" << _regularExpression << "'");
	try {
		m_searchList.insert(std::make_pair(_tokenId, std::make_shared<eci::Lexer::TypeSubBase>(_tokenId, _tokenIdParrent, _regularExpression)));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpression << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSubSection(int32_t _tokenIdParrent, int32_t _tokenId, const std::string& _regularExpressionStart, const std::string& _regularExpressionStop) {
	ECI_INFO("CPP lexer add section sub : [" << _tokenIdParrent << "] '" << _regularExpressionStart << "' .. '" << _regularExpressionStop << "'");
	try {
		m_searchList.insert(std::make_pair(_tokenId, std::make_shared<eci::Lexer::TypeSubSection>(_tokenId, _tokenIdParrent, _regularExpressionStart, _regularExpressionStop)));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpressionStart << "' .. '" << _regularExpressionStop << "' : what:" << e.what());
	}
}


eci::LexerResult eci::Lexer::interprete(const std::string& _data) {
	eci::LexerResult result(_data);
	ECI_INFO("Parse : \n" << _data);
	for (auto &it : m_searchList) {
		//ECI_INFO("Parse RegEx : " << it.first << " : " << it.second.getRegExDecorated());
		if (it.second == nullptr) {
			continue;
		}
		if (it.second->isSubParse() == true) {
			continue;
		}
		if (result.m_list.size() == 0) {
			result.m_list = it.second->parse(_data, 0, _data.size());
		} else {
			int32_t start = 0;
			auto itList(result.m_list.begin());
			while (itList != result.m_list.end()) {
				if (*itList == nullptr) {
					ECI_TODO("remove null shared_ptr");
					++itList;
					continue;
				}
				if ((*itList)->getStartPos() == start) {
					// nothing to do ..
					start = (*itList)->getStopPos();
					++itList;
					continue;
				}
				std::vector<std::shared_ptr<eci::LexerNode>> res = it.second->parse(_data, start, (*itList)->getStartPos());
				// append it in the buffer:
				if (res.size() > 0) {
					int32_t currentPos = std::distance(result.m_list.begin(), itList) + res.size() ;
					result.m_list.insert(itList, res.begin(), res.end());
					itList = result.m_list.begin() + currentPos;
				}
				start = (*itList)->getStopPos();
				++itList;
			}
			// Do the last element :
			if (start < _data.size()) {
				std::vector<std::shared_ptr<eci::LexerNode>> res = it.second->parse(_data, start, _data.size());
				for (auto &itRes : res) {
					result.m_list.push_back(itRes);
				}
			}
		}
	}
	return result;
}

std::vector<std::shared_ptr<eci::LexerNode>> eci::Lexer::TypeBase::parse(const std::string& _data, int32_t _start, int32_t _stop) {
	std::vector<std::shared_ptr<eci::LexerNode>> result;
	ECI_DEBUG("parse : " << getValue());
	while (true) {
		std::smatch resultMatch;
		std::regex_constants::match_flag_type flags = std::regex_constants::match_any;
		//APPL_DEBUG("find data at : start=" << _start << " stop=" << _stop << " regex='" << m_regexValue << "'");
		if ((int64_t)_stop <= (int64_t)_data.size()) {
			char val = _data[_stop];
			if (    val != '\n'
			     && val != '\r') {
				//after last char ==> not end of line ($ would not work))
				flags |= std::regex_constants::match_not_eol;
			}
			if (!(    ('a' <= val && val <= 'z')
			       || ('A' <= val && val <= 'Z')
			       || ('0' <= val && val <= '9')
			       || val == '_')) {
				flags |= std::regex_constants::match_not_eow;
			}
		}
		if (_start>0) {
			flags |= std::regex_constants::match_prev_avail;
		}
		std::regex_search(_data.begin()+_start, _data.begin()+_stop, resultMatch, regex, flags);
		if (resultMatch.size() > 0) {
			int32_t start = std::distance(_data.begin(), resultMatch[0].first);
			int32_t stop = std::distance(_data.begin(), resultMatch[0].second);
			ECI_DEBUG("    find data at : start=" << start << " stop=" << stop << " data='" <<std::string(_data, start, stop-start) << "'" );
			_start = stop;
			result.push_back(std::make_shared<eci::LexerNode>(m_tockenId, start, stop));
		} else {
			break;
		}
	}
	return result;
}

std::vector<std::shared_ptr<eci::LexerNode>> eci::Lexer::TypeSection::parse(const std::string& _data, int32_t _start, int32_t _stop) {
	std::vector<std::shared_ptr<eci::LexerNode>> result;
	ECI_TODO("later 1");
	return result;
}

std::vector<std::shared_ptr<eci::LexerNode>> eci::Lexer::TypeSubBase::parse(const std::string& _data, int32_t _start, int32_t _stop) {
	std::vector<std::shared_ptr<eci::LexerNode>> result;
	ECI_TODO("later 2");
	return result;
}

std::vector<std::shared_ptr<eci::LexerNode>> eci::Lexer::TypeSubSection::parse(const std::string& _data, int32_t _start, int32_t _stop) {
	std::vector<std::shared_ptr<eci::LexerNode>> result;
	ECI_TODO("later 3");
	return result;
}