/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_HEAP_H__
#define __ECI_HEAP_H__


void HeapInit(int _stackSize);
void HeapCleanup();
void *HeapAllocStack(int _size);
int HeapPopStack(void *_addr, int _size);
void HeapUnpopStack(int _size);
void HeapPushStackFrame();
int HeapPopStackFrame();
void *HeapAllocMem(int _size);
void HeapFreeMem(void *_mem);

#endif
