/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <etk/stdTools.hpp>
#include <etk/RegEx.hpp>
#include <etk/Map.hpp>
#include <etk/Vector.hpp>
#include <eci/Interpreter.hpp>


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
			int32_t getStartPos() const {
				return m_startPos;
			}
			int32_t m_stopPos;
			int32_t getStopPos() const {
				return m_stopPos;
			}
			virtual bool isNodeContainer() const {
				return false;
			}
	};
	class LexerNodeContainer : public LexerNode {
		private:
			etk::String m_type;
		public:
			LexerNodeContainer(int32_t _tockenId=-1, int32_t _startPos=-1, int32_t _stopPos=-1, const etk::String& _type="") :
			  LexerNode(_tockenId, _startPos, _stopPos) {
				m_type = _type;
			}
			virtual ~LexerNodeContainer() {};
			etk::Vector<ememory::SharedPtr<eci::LexerNode>> m_list;
			virtual bool isNodeContainer() const {
				return true;
			}
			const etk::String& getType() const {
				return m_type;
			}
	};
	class LexerResult {
		private:
			etk::String m_data;
		public:
			LexerResult(const etk::String& _data="") :
			  m_data(_data) {
				
			}
			~LexerResult() {};
			etk::Vector<ememory::SharedPtr<eci::LexerNode>> m_list;
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
					etk::String m_regexValue;
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
					virtual etk::Vector<ememory::SharedPtr<eci::LexerNode>> parse(const etk::String& _data, int32_t _start, int32_t _stop) {
						return etk::Vector<ememory::SharedPtr<eci::LexerNode>>();
					};
					virtual void parseSection(etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _data) {
						// nothing to do ...
					};
					
					etk::String getValue() {
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
					etk::RegEx<etk::String> m_regex;
					TypeBase(int32_t _tockenId, const etk::String& _regex="") :
					  Type(_tockenId),
					  m_regex(_regex) {
						m_regexValue = _regex;
					}
					virtual int32_t getType() {
						return TYPE_BASE;
					}
					etk::Vector<ememory::SharedPtr<eci::LexerNode>> parse(const etk::String& _data, int32_t _start, int32_t _stop);
			};
			class TypeSection : public Type {
				public:
					int32_t tockenStart;
					int32_t tockenStop;
					etk::String type;
					TypeSection(int32_t _tockenId, int32_t _tockenStart=-1, int32_t _tockenStop=-1, const etk::String& _type="") :
					  Type(_tockenId),
					  tockenStart(_tockenStart),
					  tockenStop(_tockenStop),
					  type(_type) {
						m_regexValue = "tok=" + etk::toString(tockenStart) + " -> tok=" + etk::toString(tockenStop);
					}
					virtual int32_t getType() {
						return TYPE_SECTION;
					}
					virtual bool isSection() {
						return true;
					}
					void parseSectionCurrent(etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _data);
					void parseSection(etk::Vector<ememory::SharedPtr<eci::LexerNode>>& _data);
			};
			class TypeSubBase : public TypeBase {
				public:
					int32_t parrent;
					TypeSubBase(int32_t _tockenId, int32_t _tokenIdParrent=-1, const etk::String& _regex="") :
					  TypeBase(_tockenId, _regex),
					  parrent(_tokenIdParrent) {}
					virtual int32_t getType() {
						return TYPE_SUB_BASE;
					}
					etk::Vector<ememory::SharedPtr<eci::LexerNode>> parse(const etk::String& _data, int32_t _start, int32_t _stop);
					bool isSubParse() {
						return true;
					}
			};
			class TypeSubSection : public TypeSection {
				public:
					int32_t parrent;
					TypeSubSection(int32_t _tockenId, int32_t _tokenIdParrent=-1, int32_t _tockenStart=-1, int32_t _tockenStop=-1, const etk::String& _type="") :
					  TypeSection(_tockenId, _tockenStart, _tockenStop, _type),
					  parrent(_tokenIdParrent) {}
					virtual int32_t getType() {
						return TYPE_SUB_SECTION;
					}
					etk::Vector<ememory::SharedPtr<eci::LexerNode>> parse(const etk::String& _data, int32_t _start, int32_t _stop);
					bool isSubParse() {
						return true;
					}
			};
			etk::Vector<ememory::SharedPtr<eci::Lexer::Type>> m_searchList;
		public:
			Lexer();
			~Lexer();
			/**
			 * @brief Append a Token recognition.
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _regularExpression reconise regular expression.
			 */
			void append(int32_t _tokenId, const etk::String& _regularExpression);
			/**
			 * @brief Append a Token recognition (section reconise start and stop with counting the number of start and stop).
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _tockenStart Tocken start.
			 * @param[in] _tockenStop Tocken stop.
			 * @param[in] _type register type when we parse it ...
			 */
			void appendSection(int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const etk::String& _type);
			/**
			 * @brief Append a Token recognition (sub parsing).
			 * @param[in] _tokenIdParrent parrent Tocken id value.
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _regularExpression reconise regular expression.
			 */
			void appendSub(int32_t _tokenIdParrent, int32_t _tokenId, const etk::String& _regularExpression);
			/**
			 * @brief Append a Token recognition (sub parsing) (section reconise start and stop with counting the number of start and stop).
			 * @param[in] _tokenIdParrent parrent Tocken id value.
			 * @param[in] _tokenId Tocken id value.
			 * @param[in] _tockenStart Tocken start.
			 * @param[in] _tockenStop Tocken stop.
			 * @param[in] _type register type when we parse it ...
			 */
			void appendSubSection(int32_t _tokenIdParrent, int32_t _tokenId, int32_t _tockenStart, int32_t _tockenStop, const etk::String& _type);
			
			LexerResult interprete(const etk::String& _data);
	};
}
