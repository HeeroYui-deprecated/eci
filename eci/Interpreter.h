/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_INTERPRETER_H__
#define __ECI_INTERPRETER_H__

#include <etk/types.h>
#include <eci/Library.h>
#include <eci/File.h>

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
		class Element : public std::enable_shared_from_this<Element> {
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
				std::vector<std::shared_ptr<Element>> m_actions;
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
				std::shared_ptr<Element> m_condition;
				std::shared_ptr<Block> m_block;
				std::shared_ptr<Block> m_blockElse;
			public:
				Condition() :
				  Element(interpreter::typeCondition) {
					
				}
				virtual ~Condition() {}
				
		};
		class For : public Element {
			protected:
				std::shared_ptr<Element> m_init;
				std::shared_ptr<Element> m_condition;
				std::shared_ptr<Element> m_increment;
				std::shared_ptr<Block> m_block;
			public:
				For() :
				  Element(interpreter::typeFor) {
					
				}
				virtual ~For() {}
				
		};
		class While : public Element {
			protected:
				bool m_conditionAtStart;
				std::shared_ptr<Element> m_condition;
				std::shared_ptr<Element> m_action;
			public:
				While() :
				  Element(interpreter::typeWhile) {
					
				}
				virtual ~While() {}
		};
		class Operator : public Element {
			protected:
				std::string m_operator;
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
			std::vector<eci::Library> m_libraries; //!< list of all loaded libraries.
			std::vector<eci::File> m_files; //!< List of all files in the current program.
		public:
			void addFile(const std::string& _filename);
			void main();
	};
}

#endif
