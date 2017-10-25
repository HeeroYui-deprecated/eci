/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <memory>
#include <eci/Lexer.hpp>
#include <eci/debug.hpp>

eci::Lexer::Lexer() {
	
}

eci::Lexer::~Lexer() {
	
}

void eci::Lexer::append(int32_t _tokenId, const etk::String& _regularExpression) {
	ECI_INFO("CPP lexer add : [" << _tokenId << "] '" << _regularExpression << "'");
	try {
		m_searchList.pushBack(ememory::makeShared<eci::Lexer::TypeBase>(_tokenId, _regularExpression));
	} catch (etk::Exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpression << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSection(int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const etk::String& _type) {
	ECI_INFO("CPP lexer add section [" << _tokenId << "] : '" << _tockenStart << "' .. '" << _tockenStop << "'");
	try {
		m_searchList.pushBack(ememory::makeShared<eci::Lexer::TypeSection>(_tokenId, _tockenStart, _tockenStop, _type));
	} catch (etk::Exception e){
		ECI_ERROR(" create reg exp : '" << _tockenStart << "' .. '" << _tockenStop << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSub(int32_t _tokenIdParrent, int32_t _tokenId, const etk::String& _regularExpression) {
	ECI_INFO("CPP lexer add sub : [" << _tokenId << "] [" << _tokenIdParrent << "] '" << _regularExpression << "'");
	try {
		m_searchList.pushBack(ememory::makeShared<eci::Lexer::TypeSubBase>(_tokenId, _tokenIdParrent, _regularExpression));
	} catch (etk::Exception e){
		ECI_ERROR(" create reg exp : '" << _regularExpression << "' : what:" << e.what());
	}
}

void eci::Lexer::appendSubSection(int32_t _tokenIdParrent, int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const etk::String& _type) {
	ECI_INFO("CPP lexer add section sub : [" << _tokenId << "] [" << _tokenIdParrent << "] '" << _tockenStart << "' .. '" << _tockenStop << "'");
	try {
		m_searchList.pushBack(ememory::makeShared<eci::Lexer::TypeSubSection>(_tokenId, _tokenIdParrent, _tockenStart, _tockenStop, _type));
	} catch (etk::Exception e){
		ECI_ERROR(" create reg exp : '" << _tockenStart << "' .. '" << _tockenStop << "' : what:" << e.what());
	}
}


eci::LexerResult eci::Lexer::interprete(const etk::String& _data) {
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
					etk::Vector<ememory::SharedPtr<eci::LexerNode>> res = it->parse(_data, start, (*itList)->getStartPos());
					// append it in the buffer:
					if (res.size() > 0) {
						int32_t currentPos = etk::distance(result.m_list.begin(), itList) + res.size();
						for (auto &it: res) {
							result.m_list.insert(itList, it);
							++itList;
						}
						/*
						result.m_list.insert(itList, res.begin(), res.end());
						itList = result.m_list.begin() + currentPos;
						*/
					}
					start = (*itList)->getStopPos();
					++itList;
				}
				// Do the last element :
				if (start < _data.size()) {
					etk::Vector<ememory::SharedPtr<eci::LexerNode>> res = it->parse(_data, start, _data.size());
					for (auto &itRes : res) {
						result.m_list.pushBack(itRes);
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
/*
static etk::RegEx_constants::match_flag_type createFlags(const etk::String& _data, int32_t _start, int32_t _stop) {
	etk::RegEx_constants::match_flag_type flags = etk::RegEx_constants::match_any;
	//ECI_DEBUG("find data at : start=" << _start << " stop=" << _stop << " regex='" << m_regexValue << "'");
	if ((int64_t)_stop <= (int64_t)_data.size()) {
		char val = _data[_stop];
		if (    val != '\n'
		     && val != '\r') {
			//after last char ==> not end of line ($ would not work))
			flags |= etk::RegEx_constants::match_not_eol;
		}
		if (!(    ('a' <= val && val <= 'z')
		       || ('A' <= val && val <= 'Z')
		       || ('0' <= val && val <= '9')
		       || val == '_')) {
			flags |= etk::RegEx_constants::match_not_eow;
		}
	}
	if (_start>0) {
		flags |= etk::RegEx_constants::match_prev_avail;
	}
	return flags;
}
*/
etk::Vector<ememory::SharedPtr<eci::LexerNode>> eci::Lexer::TypeBase::parse(const etk::String& _data, int32_t _start, int32_t _stop) {
	etk::Vector<ememory::SharedPtr<eci::LexerNode>> result;
	ECI_VERBOSE("parse : " << getValue());
	while (true) {
		if (m_regex.parse(_data, _start, _stop) == true) {
			result.pushBack(ememory::makeShared<eci::LexerNode>(m_tockenId, m_regex.start(), m_regex.stop()));
		} else {
			break;
		}
		/*
		std::smatch resultMatch;
		etk::RegEx_constants::match_flag_type flags = createFlags(_data, _start, _stop);
		etk::RegEx_search(_data.begin()+_start, _data.begin()+_stop, resultMatch, regex, flags);
		if (resultMatch.size() > 0) {
			int32_t start = etk::distance(_data.begin(), resultMatch[0].first);
			int32_t stop = etk::distance(_data.begin(), resultMatch[0].second);
			ECI_VERBOSE("    find data at : start=" << start << " stop=" << stop << " data='" <<etk::String(_data, start, stop-start) << "'" );
			_start = stop;
			result.pushBack(ememory::makeShared<eci::LexerNode>(m_tockenId, start, stop));
		} else {
			break;
		}
		*/
	}
	return result;
}

void eci::Lexer::TypeSection::parseSectionCurrent(etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _data) {
	etk::Vector<size_t> posList;
	for (size_t iii=0; iii<_data.size(); ++iii) {
		if (_data[iii] == nullptr) {
			ECI_TODO("remove null shared_ptr");
			continue;
		}
		if (_data[iii]->getTockenId() == tockenStart) {
			ECI_VERBOSE("Detect start TOCKEN " << iii);
			posList.pushBack(iii);
		}
		if (_data[iii]->getTockenId() == tockenStop) {
			if (posList.size() == 0) {
				ECI_ERROR("Detect end of tocken without start");
				continue;
			}
			size_t startId = posList.back();
			ECI_VERBOSE("Detect stop TOCKEN " << startId << " => " << iii << " list size=" << posList.size());
			posList.popBack();
			// agragate the subtoken :
			int32_t startPos = _data[startId]->getStartPos();
			int32_t stopPos = _data[iii]->getStopPos();
			ememory::SharedPtr<eci::LexerNodeContainer> newContainer = ememory::makeShared<eci::LexerNodeContainer>(m_tockenId, startPos, stopPos, type);
			ECI_VERBOSE("    Agregate: " << startId << " -> " << iii);
			newContainer->m_list.insert(0, &_data[startId+1], iii-(startId+1));
			ECI_VERBOSE("    list size=" << newContainer->m_list.size() << " old=" << _data.size());
			_data.erase(startId, iii+1);
			ECI_VERBOSE("    list size=" << newContainer->m_list.size() << " old=" << _data.size());
			_data.insert(startId, newContainer);
			ECI_VERBOSE("    list size=" << newContainer->m_list.size() << " old=" << _data.size());
			iii = startId-1;
		}
	}
}
void eci::Lexer::TypeSection::parseSection(etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _data) {
	ECI_VERBOSE("parse section : " << getValue());
	for (auto &it : _data) {
		if (it == nullptr) {
			continue;
		}
		if (it->isNodeContainer() == true) {
			ememory::SharedPtr<eci::LexerNodeContainer> sec = ememory::dynamicPointerCast<eci::LexerNodeContainer>(it);
			parseSection(sec->m_list);
		}
	}
	parseSectionCurrent(_data);
}

etk::Vector<ememory::SharedPtr<eci::LexerNode>> eci::Lexer::TypeSubBase::parse(const etk::String& _data, int32_t _start, int32_t _stop) {
	etk::Vector<ememory::SharedPtr<eci::LexerNode>> result;
	ECI_TODO("later 2");
	return result;
}

etk::Vector<ememory::SharedPtr<eci::LexerNode>> eci::Lexer::TypeSubSection::parse(const etk::String& _data, int32_t _start, int32_t _stop) {
	etk::Vector<ememory::SharedPtr<eci::LexerNode>> result;
	ECI_TODO("later 3");
	return result;
}