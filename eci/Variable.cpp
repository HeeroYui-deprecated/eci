/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <etk/types.hpp>
#include <eci/Variable.hpp>
#include <eci/debug.hpp>

eci::Variable::Variable() :
  m_visibility(eci::visibilityPublic),
  m_const(false) {
	
}

eci::Variable::~Variable() {
	
}