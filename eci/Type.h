/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_TYPE_H__
#define __ECI_TYPE_H__

#include <etk/types.h>
#include <map>
#include <memory>
#include <eci/debug.h>

namespace eci {
	class Variable;
	class Type : public std::enable_shared_from_this<eci::Type> {
		protected:
			std::string m_signature; // !!! <== specific au language ...
		public:
			Type() {};
			virtual ~Type() {};
			virtual std::shared_ptr<eci::Variable> callOperator(const std::shared_ptr<eci::Variable>& _this,
			                                                    const std::string& _operatorName,
			                                                    const std::shared_ptr<eci::Variable>& _obj) {
				ECI_ERROR("call unknow operator : '" << _operatorName << "'");
				return nullptr;
			}
			virtual std::vector<std::shared_ptr<eci::Variable>> callFunction(const std::shared_ptr<eci::Variable>& _this,
			                                                                 const std::string& _name,
			                                                                 const std::vector<std::shared_ptr<eci::Variable>>& _objList) {
				ECI_ERROR("call unknow function : '" << _name << "' with _input.size()=" << _objList.size());
				return std::vector<std::shared_ptr<eci::Variable>>();
			};
			virtual std::shared_ptr<eci::Variable> getVariable(std::shared_ptr<eci::Variable> _this,
			                                                   const std::string& _name) {
				ECI_ERROR("try get unknow Variable : '" << _name << "'");
				return nullptr;
			};
			virtual std::shared_ptr<eci::Variable> create(const std::vector<std::shared_ptr<eci::Variable>>& _objList) {
				return nullptr;
			}
			virtual void destroy(std::shared_ptr<eci::Variable>& _obj) {
				if (_obj != nullptr) {
					// TODO : mark as destroyed ...
				}
			}
			virtual std::shared_ptr<eci::Variable> clone(const std::shared_ptr<eci::Variable>& _obj) {
				return nullptr;
			}
			virtual std::shared_ptr<eci::Variable> cast(const std::shared_ptr<eci::Variable>& _obj, const eci::Type& _type) {
				return nullptr;
			}
	};
	class TypeNatif : public Type {
		protected:
			// name , opertor * / += / ++ ...
			std::map<std::string, std::function<std::shared_ptr<eci::Variable>(const std::shared_ptr<eci::Variable>&)>> m_operatorList;
			// name , function to call
			std::map<std::string, std::function<std::vector<std::shared_ptr<eci::Variable>>(const std::vector<std::shared_ptr<eci::Variable>>&)>> m_functionList;
			//
			
		
	};
	
	template<typename T> class TypeBase : public Type {
		
	};
	
	template<> class TypeBase<int32_t> {
		
	};
}

#endif