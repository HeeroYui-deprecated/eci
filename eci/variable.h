/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_VARIABLE_H__
#define __ECI_VARIABLE_H__


void VariableInit();
void VariableCleanup();
void VariableFree(struct Value* _value);
void VariableTableCleanup(struct Table* _hashTable);
void *VariableAlloc(struct ParseState* _parser, int _size, int _onHeap);
void VariableStackPop(struct ParseState *_parser, struct Value *_value);
struct Value *VariableAllocValueAndData(struct ParseState* _parser, int _dataSize, int _isLValue, struct Value *_lValueFrom, int _onHeap);
struct Value *VariableAllocValueAndCopy(struct ParseState* _parser, struct Value *_fromValue, int _onHeap);
struct Value *VariableAllocValueFromType(struct ParseState *_parser, struct ValueType *_type, int _isLValue, struct Value *_lValueFrom, int _onHeap);
struct Value *VariableAllocValueFromExistingData(struct ParseState *_parser, struct ValueType *_type, union AnyValue *_fromValue, int _isLValue, struct Value *_lValueFrom);
struct Value *VariableAllocValueShared(struct ParseState *_parser, struct Value *_fromValue);
struct Value *VariableDefine(struct ParseState *_parser, char *_ident, struct Value *_initValue, struct ValueType *_type, int _makeWritable);
struct Value *VariableDefineButIgnoreIdentical(struct ParseState *_parser, char *_ident, struct ValueType *_type, int _isStatic, int *_firstVisit);
int VariableDefined(const char *Ident);
void VariableGet(struct ParseState *_parser, const char *_ident, struct Value **_lVal);
void VariableDefinePlatformVar(struct ParseState *_parser, char *_ident, struct ValueType *_type, union AnyValue *_fromValue, int _isWritable);
void VariableStackFrameAdd(struct ParseState *_parser, const char *_funcName, int _numParams);
void VariableStackFramePop(struct ParseState *_parser);
struct Value *VariableStringLiteralGet(char *_ident);
void VariableStringLiteralDefine(char *_ident, struct Value *_value);
void *VariableDereferencePointer(struct ParseState *_parser, struct Value *_pointerValue, struct Value **_dereferenceVal, int *_dereferenceOffset, struct ValueType **_dereferenceType, int *_derefIsLValue);

#endif
