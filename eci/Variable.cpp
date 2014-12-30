/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/Variable.h>
#include <eci/debug.h>

eci::Variable::Variable() :
  m_visibility(eci::visibilityPublic),
  m_const(false) {
	
}

eci::Variable::~Variable() {
	
}