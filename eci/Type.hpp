/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <etk/types.hpp>
#include <etk/Map.hpp>
#include <ememory/memory.hpp>
#include <eci/debug.hpp>

namespace eci {
	class Variable;
	class Type : public ememory::EnableSharedFromThis<eci::Type> {
		protected:
			etk::String m_signature; // !!! <== specific au language ...
		public:
			Type() {};
			virtual ~Type() {};
			virtual ememory::SharedPtr<eci::Variable> callOperator(const ememory::SharedPtr<eci::Variable>& _this,
			                                                       const etk::String& _operatorName,
			                                                       const ememory::SharedPtr<eci::Variable>& _obj) {
				ECI_ERROR("call unknow operator : '" << _operatorName << "'");
				return null;
			}
			virtual etk::Vector<ememory::SharedPtr<eci::Variable>> callFunction(const ememory::SharedPtr<eci::Variable>& _this,
			                                                                    const etk::String& _name,
			                                                                    const etk::Vector<ememory::SharedPtr<eci::Variable>>& _objList) {
				ECI_ERROR("call unknow function : '" << _name << "' with _input.size()=" << _objList.size());
				return etk::Vector<ememory::SharedPtr<eci::Variable>>();
			};
			virtual ememory::SharedPtr<eci::Variable> getVariable(ememory::SharedPtr<eci::Variable> _this,
			                                                   const etk::String& _name) {
				ECI_ERROR("try get unknow Variable : '" << _name << "'");
				return null;
			};
			virtual ememory::SharedPtr<eci::Variable> create(const etk::Vector<ememory::SharedPtr<eci::Variable>>& _objList) {
				return null;
			}
			virtual void destroy(ememory::SharedPtr<eci::Variable>& _obj) {
				if (_obj != null) {
					// TODO : mark as destroyed ...
				}
			}
			virtual ememory::SharedPtr<eci::Variable> clone(const ememory::SharedPtr<eci::Variable>& _obj) {
				return null;
			}
			virtual ememory::SharedPtr<eci::Variable> cast(const ememory::SharedPtr<eci::Variable>& _obj, const eci::Type& _type) {
				return null;
			}
	};
	class TypeNatif : public Type {
		protected:
			// name , opertor * / += / ++ ...
			etk::Map<etk::String, etk::Function<ememory::SharedPtr<eci::Variable>(const ememory::SharedPtr<eci::Variable>&)>> m_operatorList;
			// name , function to call
			etk::Map<etk::String, etk::Function<etk::Vector<ememory::SharedPtr<eci::Variable>>(const etk::Vector<ememory::SharedPtr<eci::Variable>>&)>> m_functionList;
			//
			
		
	};
	
	template<typename T> class TypeBase : public Type {
		
	};
	
	template<> class TypeBase<int32_t> {
		
	};
}

