/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_LEXER_H__
#define __ECI_LEXER_H__

#include <etk/types.h>
#include <etk/stdTools.h>
#include <regex>
#include <map>
#include <vector>


namespace eci {
	class LexerNode {
		public:
			LexerNode(int32_t _tockenId=-1, int32_t _startPos=-1, int32_t _stopPos=-1) :
			  m_tockenId(_tockenId),
			  m_startPos(_startPos),
			  m_stopPos(_stopPos) {
				
			}
			virtual ~LexerNode() {};
			int32_t m_tockenId;
			int32_t m_startPos;
			int32_t m_stopPos;
			int32_t getStartPos() {
				return m_startPos;
			}
			int32_t getStopPos() {
				return m_stopPos;
			}
	};
	class LexerNodeContainer : public LexerNode {
		public:
			LexerNodeContainer(int32_t _tockenId=-1, int32_t _startPos=-1, int32_t _stopPos=-1) :
			  LexerNode(_tockenId, _startPos, _stopPos) {
				
			}
			virtual ~LexerNodeContainer() {};
			std::vector<std::shared_ptr<eci::LexerNode>> m_list;
	};
	class LexerResult {
		private:
			std::string m_data;
		public:
			LexerResult(const std::string& _data="") :
			  m_data(_data) {
				
			}
			~LexerResult() {};
			std::vector<std::shared_ptr<eci::LexerNode>> m_list;
	};
	
	class Lexer {
		private:
			#define TYPE_UNKNOW (0)
			#define TYPE_BASE (1)
			#define TYPE_SECTION (2)
			#define TYPE_SUB_BASE (3)
			#define TYPE_SUB_SECTION (4)
			class Type {
				protected:
					int32_t m_tockenId;
					std::string m_regexValue;
				public:
					Type(int32_t _tockenId) :
					  m_tockenId(_tockenId) {}
					virtual ~Type() {}
					virtual int32_t getType() {
						return TYPE_UNKNOW;
					}
					int32_t getTockenId() {
						return m_tockenId;
					}
					virtual std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop)=0;
					std::string getValue() {
						return m_regexValue;
					};
					virtual bool isSubParse() {
						return false;
					}
			};
			class TypeBase : public Type {
				public:
					std::regex regex;
					TypeBase(int32_t _tockenId, const std::string& _regex="") :
					  Type(_tockenId),
					  regex(_regex, std::regex_constants::optimize | std::regex_constants::ECMAScript) {
						m_regexValue = _regex;
					}
					virtual int32_t getType() {
						return TYPE_BASE;
					}
					std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop);
			};
			class TypeSection : public Type {
				public:
					std::regex regexStart;
					std::regex regexStop;
					TypeSection(int32_t _tockenId, const std::string& _regexStart="", const std::string& _regexStop="") :
					  Type(_tockenId),
					  regexStart(_regexStart, std::regex_constants::optimize | std::regex_constants::ECMAScript),
					  regexStop(_regexStop, std::regex_constants::optimize | std::regex_constants::ECMAScript) {
						m_regexValue = _regexStart + " -> " + _regexStop;
					}
					virtual int32_t getType() {
						return TYPE_SECTION;
					}
					std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop);
			};
			class TypeSubBase : public TypeBase {
				public:
					int32_t parrent;
					TypeSubBase(int32_t _tockenId, int32_t _tokenIdParrent=-1, const std::string& _regex="") :
					  TypeBase(_tockenId, _regex),
					  parrent(_tokenIdParrent) {}
					virtual int32_t getType() {
						return TYPE_SUB_BASE;
					}
					std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop);
					bool isSubParse() {
						return true;
					}
			};
			class TypeSubSection : public TypeSection {
				public:
					int32_t parrent;
					TypeSubSection(int32_t _tockenId, int32_t _tokenIdParrent=-1, const std::string& _regexStart="", const std::string& _regexStop="") :
					  TypeSection(_tockenId, _regexStart, _regexStop),
					  parrent(_tokenIdParrent) {}
					virtual int32_t getType() {
						return TYPE_SUB_SECTION;
					}
					std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop);
					bool isSubParse() {
						return true;
					}
			};
			std::map<int32_t, std::shared_ptr<eci::Lexer::Type>> m_searchList;
		public:
			Lexer();
			~Lexer();
			/**
			 * @brief Append a Token recognition.
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _regularExpression reconise regular expression.
			 */
			void append(int32_t _tokenId, const std::string& _regularExpression);
			/**
			 * @brief Append a Token recognition (section reconise start and stop with counting the number of start and stop).
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _regularExpressionStart reconise regular expression (start).
			 * @param[in] _regularExpressionStop reconise regular expression (stop).
			 */
			void appendSection(int32_t _tokenId, const std::string& _regularExpressionStart, const std::string& _regularExpressionStop);
			/**
			 * @brief Append a Token recognition (sub parsing).
			 * @param[in] _tokenIdParrent parrent Tocken id value.
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _regularExpression reconise regular expression.
			 */
			void appendSub(int32_t _tokenIdParrent, int32_t _tokenId, const std::string& _regularExpression);
			/**
			 * @brief Append a Token recognition (sub parsing) (section reconise start and stop with counting the number of start and stop).
			 * @param[in] _tokenIdParrent parrent Tocken id value.
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _regularExpressionStart reconise regular expression (start).
			 * @param[in] _regularExpressionStop reconise regular expression (stop).
			 */
			void appendSubSection(int32_t _tokenIdParrent, int32_t _tokenId, const std::string& _regularExpressionStart, const std::string& _regularExpressionStop);
			
			LexerResult interprete(const std::string& _data);
	};
}

#endif
