/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/Function.hpp>
#include <eci/debug.hpp>


eci::Function::Function() :
  m_const(false),
  m_static(false),
  m_visibility(eci::visibilityPublic){
	
}

eci::Function::~Function() {
	
}

etk::Vector<ememory::SharedPtr<eci::Value>> eci::Function::call(const etk::Vector<ememory::SharedPtr<eci::Value>>& _input) {
	return etk::Vector<ememory::SharedPtr<eci::Value>>();
}


