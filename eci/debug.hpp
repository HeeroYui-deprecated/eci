/**
 * @author Edouard DUPIN
 * @copyright 2014, Edouard DUPIN, all right reserved
 * @license MPL-2 (see license file)
 */

#pragma once

#include <elog/log.hpp>

namespace eci {
	int32_t getLogId();
};
#define ECI_BASE(info,data) ELOG_BASE(eci::getLogId(),info,data)

#define ECI_PRINT(data)         ECI_BASE(-1, data)
#define ECI_CRITICAL(data)      ECI_BASE(1, data)
#define ECI_ERROR(data)         ECI_BASE(2, data)
#define ECI_WARNING(data)       ECI_BASE(3, data)
#ifdef DEBUG
	#define ECI_INFO(data)          ECI_BASE(4, data)
	#define ECI_DEBUG(data)         ECI_BASE(5, data)
	#define ECI_VERBOSE(data)       ECI_BASE(6, data)
	#define ECI_TODO(data)          ECI_BASE(4, "TODO : " << data)
#else
	#define ECI_INFO(data)          do { } while(false)
	#define ECI_DEBUG(data)         do { } while(false)
	#define ECI_VERBOSE(data)       do { } while(false)
	#define ECI_TODO(data)          do { } while(false)
#endif

#define ECI_ASSERT(cond,data) \
	do { \
		if (!(cond)) { \
			ECI_CRITICAL(data); \
			assert(!#cond); \
		} \
	} while (0)


