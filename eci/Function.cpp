/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/Function.h>
#include <eci/debug.h>


eci::Function::Function() :
  m_const(false),
  m_static(false),
  m_visibility(eci::visibilityPublic){
	
}

eci::Function::~Function() {
	
}

std::vector<std::shared_ptr<eci::Value>> eci::Function::call(const std::vector<std::shared_ptr<eci::Value>>& _input) {
	return std::vector<std::shared_ptr<eci::Value>>();
}


