/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_C_LIBRARY_H__
#define __ECI_C_LIBRARY_H__



void LibraryInit();
void LibraryAdd(struct Table* _globalTable, const char* _libraryName, struct LibraryFunction* _funcList);
void CLibraryInit();
void PrintType(struct ValueType* _type, IOFILE* _stream);


#endif
