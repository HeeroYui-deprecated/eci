/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_VISIBILITY_H__
#define __ECI_VISIBILITY_H__

#include <etk/types.h>

namespace eci {
	enum visibility {
		visibilityNone,
		visibilityPrivate,
		visibilityProtected,
		visibilityPublic,
	};
}

#endif
