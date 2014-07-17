/**
 * @author Edouard DUPIN
 * 
 * @copyright 2011, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_DEBUG_H__
#define __ECI_DEBUG_H__

#include <etk/log.h>

namespace eci {
	int32_t getLogId();
};
// TODO : Review this problem of multiple intanciation of "std::stringbuf sb"
#define ECI_BASE(info,data) \
	do { \
		if (info <= etk::log::getLevel(eci::getLogId())) { \
			std::stringbuf sb; \
			std::ostream tmpStream(&sb); \
			tmpStream << data; \
			etk::log::logStream(eci::getLogId(), info, __LINE__, __class__, __func__, tmpStream); \
		} \
	} while(0)

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

#endif

