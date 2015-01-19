/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <memory>
#include <eci/Lexer.h>
#include <eci/debug.h>

eci::Lexer::Lexer() {
	
}

eci::Lexer::~Lexer() {
	
}

void eci::Lexer::append(int32_t _tokenId, const std::string& _regularExpression) {
	ECI_INFO("CPP lexer add : [" << _tokenId << "] '" << _regularExpression << "'");
	try {
		m_searchList.push_back(std::make_shared<eci::Lexer::TypeBase>(_tokenId, _regularExpression));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpression << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSection(int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const std::string& _type) {
	ECI_INFO("CPP lexer add section [" << _tokenId << "] : '" << _tockenStart << "' .. '" << _tockenStop << "'");
	try {
		m_searchList.push_back(std::make_shared<eci::Lexer::TypeSection>(_tokenId, _tockenStart, _tockenStop, _type));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _tockenStart << "' .. '" << _tockenStop << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSub(int32_t _tokenIdParrent, int32_t _tokenId, const std::string& _regularExpression) {
	ECI_INFO("CPP lexer add sub : [" << _tokenId << "] [" << _tokenIdParrent << "] '" << _regularExpression << "'");
	try {
		m_searchList.push_back(std::make_shared<eci::Lexer::TypeSubBase>(_tokenId, _tokenIdParrent, _regularExpression));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpression << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSubSection(int32_t _tokenIdParrent, int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const std::string& _type) {
	ECI_INFO("CPP lexer add section sub : [" << _tokenId << "] [" << _tokenIdParrent << "] '" << _tockenStart << "' .. '" << _tockenStop << "'");
	try {
		m_searchList.push_back(std::make_shared<eci::Lexer::TypeSubSection>(_tokenId, _tokenIdParrent, _tockenStart, _tockenStop, _type));
	} catch (std::exception e){
		ECI_ERROR(" create reg exp : '" << _tockenStart << "' .. '" << _tockenStop << "' : what:" << e.what());
	}
}


eci::LexerResult eci::Lexer::interprete(const std::string& _data) {
	eci::LexerResult result(_data);
	ECI_INFO("Parse : \n" << _data);
	for (auto &it : m_searchList) {
		//ECI_INFO("Parse RegEx : " << it.first << " : " << it.second.getRegExDecorated());
		if (it == nullptr) {
			continue;
		}
		if (it->isSubParse() == true) {
			continue;
		}
		if (it->isSection() == false) {
			if (result.m_list.size() == 0) {
				result.m_list = it->parse(_data, 0, _data.size());
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
					std::vector<std::shared_ptr<eci::LexerNode>> res = it->parse(_data, start, (*itList)->getStartPos());
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
					std::vector<std::shared_ptr<eci::LexerNode>> res = it->parse(_data, start, _data.size());
					for (auto &itRes : res) {
						result.m_list.push_back(itRes);
					}
				}
			}
		} else {
			if (result.m_list.size() == 0) {
				continue;
			}
			it->parseSection(result.m_list);
		}
	}
	return result;
}

static std::regex_constants::match_flag_type createFlags(const std::string& _data, int32_t _start, int32_t _stop) {
	std::regex_constants::match_flag_type flags = std::regex_constants::match_any;
	//ECI_DEBUG("find data at : start=" << _start << " stop=" << _stop << " regex='" << m_regexValue << "'");
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
	return flags;
}

std::vector<std::shared_ptr<eci::LexerNode>> eci::Lexer::TypeBase::parse(const std::string& _data, int32_t _start, int32_t _stop) {
	std::vector<std::shared_ptr<eci::LexerNode>> result;
	ECI_VERBOSE("parse : " << getValue());
	while (true) {
		std::smatch resultMatch;
		std::regex_constants::match_flag_type flags = createFlags(_data, _start, _stop);
		std::regex_search(_data.begin()+_start, _data.begin()+_stop, resultMatch, regex, flags);
		if (resultMatch.size() > 0) {
			int32_t start = std::distance(_data.begin(), resultMatch[0].first);
			int32_t stop = std::distance(_data.begin(), resultMatch[0].second);
			ECI_VERBOSE("    find data at : start=" << start << " stop=" << stop << " data='" <<std::string(_data, start, stop-start) << "'" );
			_start = stop;
			result.push_back(std::make_shared<eci::LexerNode>(m_tockenId, start, stop));
		} else {
			break;
		}
	}
	return result;
}

void eci::Lexer::TypeSection::parseSectionCurrent(std::vector<std::shared_ptr<eci::LexerNode>>& _data) {
	std::vector<size_t> posList;
	for (size_t iii=0; iii<_data.size(); ++iii) {
		if (_data[iii] == nullptr) {
			ECI_TODO("remove null shared_ptr");
			continue;
		}
		if (_data[iii]->getTockenId() == tockenStart) {
			ECI_VERBOSE("Detect start TOCKEN " << iii);
			posList.push_back(iii);
		}
		if (_data[iii]->getTockenId() == tockenStop) {
			if (posList.size() == 0) {
				ECI_ERROR("Detect end of tocken without start");
				continue;
			}
			size_t startId = posList.back();
			ECI_VERBOSE("Detect stop TOCKEN " << startId << " => " << iii << " list size=" << posList.size());
			posList.pop_back();
			// agragate the subtoken :
			int32_t startPos = _data[startId]->getStartPos();
			int32_t stopPos = _data[iii]->getStopPos();
			std::shared_ptr<eci::LexerNodeContainer> newContainer = std::make_shared<eci::LexerNodeContainer>(m_tockenId, startPos, stopPos, type);
			ECI_VERBOSE("    Agregate: " << startId << " -> " << iii);
			newContainer->m_list.insert(newContainer->m_list.begin(), _data.begin()+startId+1, _data.begin()+iii);
			ECI_VERBOSE("    list size=" << newContainer->m_list.size() << " old=" << _data.size());
			_data.erase(_data.begin()+startId, _data.begin()+iii+1);
			ECI_VERBOSE("    list size=" << newContainer->m_list.size() << " old=" << _data.size());
			_data.insert(_data.begin()+startId, newContainer);
			ECI_VERBOSE("    list size=" << newContainer->m_list.size() << " old=" << _data.size());
			iii = startId-1;
		}
	}
}
void eci::Lexer::TypeSection::parseSection(std::vector<std::shared_ptr<eci::LexerNode>>& _data) {
	ECI_VERBOSE("parse section : " << getValue());
	for (auto &it : _data) {
		if (it == nullptr) {
			continue;
		}
		if (it->isNodeContainer() == true) {
			std::shared_ptr<eci::LexerNodeContainer> sec = std::dynamic_pointer_cast<eci::LexerNodeContainer>(it);
			parseSection(sec->m_list);
		}
	}
	parseSectionCurrent(_data);
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