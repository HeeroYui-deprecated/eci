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
#include <eci/Interpreter.h>


namespace eci {
	class LexerNode : public interpreter::Element {
		public:
			LexerNode(int32_t _tockenId=-1, int32_t _startPos=-1, int32_t _stopPos=-1) :
			  interpreter::Element(_tockenId),
			  m_startPos(_startPos),
			  m_stopPos(_stopPos) {
				
			}
			virtual ~LexerNode() {};
			int32_t m_startPos;
			int32_t getStartPos() {
				return m_startPos;
			}
			int32_t m_stopPos;
			int32_t getStopPos() {
				return m_stopPos;
			}
			virtual bool isNodeContainer() {
				return false;
			}
	};
	class LexerNodeContainer : public LexerNode {
		private:
			std::string m_type;
		public:
			LexerNodeContainer(int32_t _tockenId=-1, int32_t _startPos=-1, int32_t _stopPos=-1, const std::string& _type="") :
			  LexerNode(_tockenId, _startPos, _stopPos) {
				m_type = _type;
			}
			virtual ~LexerNodeContainer() {};
			std::vector<std::shared_ptr<eci::LexerNode>> m_list;
			virtual bool isNodeContainer() {
				return true;
			}
			const std::string& getType() const {
				return m_type;
			}
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
					virtual std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop) {
						return std::vector<std::shared_ptr<eci::LexerNode>>();
					};
					virtual void parseSection(std::vector<std::shared_ptr<eci::LexerNode>>& _data) {
						// nothing to do ...
					};
					
					std::string getValue() {
						return m_regexValue;
					};
					virtual bool isSubParse() {
						return false;
					}
					virtual bool isSection() {
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
					int32_t tockenStart;
					int32_t tockenStop;
					std::string type;
					TypeSection(int32_t _tockenId, int32_t _tockenStart=-1, int32_t _tockenStop=-1, const std::string& _type="") :
					  Type(_tockenId),
					  tockenStart(_tockenStart),
					  tockenStop(_tockenStop),
					  type(_type) {
						m_regexValue = "tok=" + etk::to_string(tockenStart) + " -> tok=" + etk::to_string(tockenStop);
					}
					virtual int32_t getType() {
						return TYPE_SECTION;
					}
					virtual bool isSection() {
						return true;
					}
					void parseSectionCurrent(std::vector<std::shared_ptr<eci::LexerNode>>& _data);
					void parseSection(std::vector<std::shared_ptr<eci::LexerNode>>& _data);
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
					TypeSubSection(int32_t _tockenId, int32_t _tokenIdParrent=-1, int32_t _tockenStart=-1, int32_t _tockenStop=-1, const std::string& _type="") :
					  TypeSection(_tockenId, _tockenStart, _tockenStop, _type),
					  parrent(_tokenIdParrent) {}
					virtual int32_t getType() {
						return TYPE_SUB_SECTION;
					}
					std::vector<std::shared_ptr<eci::LexerNode>> parse(const std::string& _data, int32_t _start, int32_t _stop);
					bool isSubParse() {
						return true;
					}
			};
			std::vector<std::shared_ptr<eci::Lexer::Type>> m_searchList;
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
			 * @param[in] _tockenStart Tocken start.
			 * @param[in] _tockenStop Tocken stop.
			 * @param[in] _type register type when we parse it ...
			 */
			void appendSection(int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const std::string& _type);
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
			 * @param[in] _tockenStart Tocken start.
			 * @param[in] _tockenStop Tocken stop.
			 * @param[in] _type register type when we parse it ...
			 */
			void appendSubSection(int32_t _tokenIdParrent, int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const std::string& _type);
			
			LexerResult interprete(const std::string& _data);
	};
}

#endif
