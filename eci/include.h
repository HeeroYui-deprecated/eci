/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_INCLUDE_H__
#define __ECI_INCLUDE_H__


void IncludeInit();
void IncludeCleanup();
void IncludeRegister(const char *_includeName, void (*_setupFunction)(void), struct LibraryFunction *_funcList, const char *_setupCSource);
void IncludeFile(char *_filename);
/* the following is defined in picoc.h:
 * void PicocIncludeAllSystemHeaders(); */


#endif
