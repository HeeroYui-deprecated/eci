/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <eci/Library.hpp>
#include <eci/File.hpp>

namespace eci {
	namespace interpreter {
		enum type {
			typeBlock, //!< block area definition
			typeType, //!< type definition
			typeVariable, //!< new variable use
			typeVariableDeclaration, //!< new variable definition
			typeFunction, //!< function definition
			typeClass, //!< Class definition
			typeNamespace, //!< Namespace definition
			typeCondition, //!< Classicle condition (with else)
			typeFor, //!< classicle C cycle (init, inc, condition)
			typeWhile, //!< Call a cycle (option action previous condition or condition previous action)
			typeOperator, //!< Call operator "xx" ex : "*" "++" "=" "=="
			typeReserveId = 5000,
		};
		class Element : public ememory::EnableSharedFromThis<Element> {
			protected:
				int32_t m_tockenId;
			public:
				int32_t getTockenId() {
					return m_tockenId;
				}
			public:
				Element(int32_t _tockenId=-1) :
				  m_tockenId(_tockenId) {
					
				}
				virtual ~Element() {}
		};
		class Block : public Element {
			protected:
				etk::Vector<ememory::SharedPtr<Element>> m_actions;
			public:
				Block() :
				  Element(interpreter::typeBlock) {
					
				}
				virtual ~Block() {}
		};
		class Type : public Element {
			protected:
				
			public:
				Type() :
				  Element(interpreter::typeType) {
					
				}
				virtual ~Type() {}
		};
		class Variable : public Element {
			protected:
				
			public:
				Variable() :
				  Element(interpreter::typeVariable) {
					
				}
				virtual ~Variable() {}
		};
		class VariableDeclaration : public Element {
			protected:
				
			public:
				VariableDeclaration() :
				  Element(interpreter::typeVariableDeclaration) {
					
				}
				virtual ~VariableDeclaration() {}
		};
		class Function : public Element {
			protected:
				
			public:
				Function() :
				  Element(interpreter::typeFunction) {
					
				}
				virtual ~Function() {}
		};
		class Class : public Element {
			protected:
				
			public:
				Class() :
				  Element(interpreter::typeClass) {
					
				}
				virtual ~Class() {}
		};
		class Namespace : public Element {
			protected:
				
			public:
				Namespace() :
				  Element(interpreter::typeNamespace) {
					
				}
				virtual ~Namespace() {}
		};
		class Condition : public Element {
			protected:
				ememory::SharedPtr<Element> m_condition;
				ememory::SharedPtr<Block> m_block;
				ememory::SharedPtr<Block> m_blockElse;
			public:
				Condition() :
				  Element(interpreter::typeCondition) {
					
				}
				virtual ~Condition() {}
				
		};
		class For : public Element {
			protected:
				ememory::SharedPtr<Element> m_init;
				ememory::SharedPtr<Element> m_condition;
				ememory::SharedPtr<Element> m_increment;
				ememory::SharedPtr<Block> m_block;
			public:
				For() :
				  Element(interpreter::typeFor) {
					
				}
				virtual ~For() {}
				
		};
		class While : public Element {
			protected:
				bool m_conditionAtStart;
				ememory::SharedPtr<Element> m_condition;
				ememory::SharedPtr<Element> m_action;
			public:
				While() :
				  Element(interpreter::typeWhile) {
					
				}
				virtual ~While() {}
		};
		class Operator : public Element {
			protected:
				etk::String m_operator;
			public:
				Operator() :
				  Element(interpreter::typeOperator) {
					
				}
				virtual ~Operator() {}
		};
		
	}
	class Interpreter {
		public:
			Interpreter();
			~Interpreter();
		protected:
			etk::Vector<eci::Library> m_libraries; //!< list of all loaded libraries.
			etk::Vector<eci::File> m_files; //!< List of all files in the current program.
		public:
			void addFile(const etk::String& _filename);
			void main();
	};
}
