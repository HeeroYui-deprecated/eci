/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#include <eci/debug.hpp>

int32_t eci::getLogId() {
	static int32_t g_val = elog::registerInstance("eci");
	return g_val;
}
