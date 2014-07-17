/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include <eci/debug.h>

int32_t eci::getLogId() {
	static int32_t g_val = etk::log::registerInstance("eci");
	return g_val;
}
