/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include "interpreter.h"

/* maximum size of a value to temporarily copy while we create a variable */
#define MAX_TMP_COPY_BUF 256

/* the table of global definitions */
struct Table GlobalTable;
struct TableEntry *GlobalHashTable[GLOBAL_TABLE_SIZE];

/* the table of string literal values */
struct Table StringLiteralTable;
struct TableEntry *StringLiteralHashTable[STRING_LITERAL_TABLE_SIZE];

/* the stack */
struct StackFrame *TopStackFrame = NULL;


/* initialise the variable system */
void VariableInit() {
	TableInitTable(&GlobalTable, &GlobalHashTable[0], GLOBAL_TABLE_SIZE, TRUE);
	TableInitTable(&StringLiteralTable, &StringLiteralHashTable[0], STRING_LITERAL_TABLE_SIZE, TRUE);
	TopStackFrame = NULL;
}

/* deallocate the contents of a variable */
void VariableFree(struct Value *_value) {
	if (_value->ValOnHeap) {
		/* free function bodies */
		if (    _value->Typ == &FunctionType
		     && _value->Val->FuncDef.Intrinsic == NULL
		     && _value->Val->FuncDef.Body.Pos != NULL) {
			HeapFreeMem((void *)_value->Val->FuncDef.Body.Pos);
		}
		/* free macro bodies */
		if (_value->Typ == &MacroType) {
			HeapFreeMem((void *)_value->Val->MacroDef.Body.Pos);
		}
		/* free the value */
		HeapFreeMem(_value);
	}
}

/* deallocate the global table and the string literal table */
void VariableTableCleanup(struct Table *_hashTable) {
	struct TableEntry *Entry;
	struct TableEntry *NextEntry;
	int Count;
	for (Count = 0; Count < _hashTable->Size; ++Count) {
		for (Entry = _hashTable->HashTable[Count]; Entry != NULL; Entry = NextEntry) {
			NextEntry = Entry->Next;
			VariableFree(Entry->p.v.Val);
			/* free the hash table entry */
			HeapFreeMem(Entry);
		}
	}
}

void VariableCleanup() {
	VariableTableCleanup(&GlobalTable);
	VariableTableCleanup(&StringLiteralTable);
}

/* allocate some memory, either on the heap or the stack and check if we've run out */
void *VariableAlloc(struct ParseState *_parser,
                    int _size,
                    int _onHeap) {
	void *NewValue;
	if (_onHeap) {
		NewValue = HeapAllocMem(_size);
	} else {
		NewValue = HeapAllocStack(_size);
	}
	if (NewValue == NULL) {
		ProgramFail(_parser, "out of memory");
	}
#ifdef DEBUG_HEAP
	if (!_onHeap) {
		printf("pushing %d at 0x%lx\n", _size, (unsigned long)NewValue);
	}
#endif
	return NewValue;
}

/* allocate a value either on the heap or the stack using space dependent on what type we want */
struct Value *VariableAllocValueAndData(struct ParseState *_parser,
                                        int _dataSize,
                                        int _isLValue,
                                        struct Value *_lValueFrom,
                                        int _onHeap) {
	struct Value *NewValue = VariableAlloc(_parser, MEM_ALIGN(sizeof(struct Value)) + _dataSize, _onHeap);
	NewValue->Val = (union AnyValue *)((char *)NewValue + MEM_ALIGN(sizeof(struct Value)));
	NewValue->ValOnHeap = _onHeap;
	NewValue->ValOnStack = !_onHeap;
	NewValue->IsLValue = _isLValue;
	NewValue->LValueFrom = _lValueFrom;
	return NewValue;
}

/* allocate a value given its type */
struct Value *VariableAllocValueFromType(struct ParseState *_parser,
                                         struct ValueType *_type,
                                         int _isLValue,
                                         struct Value *_lValueFrom,
                                         int _onHeap) {
	int Size = TypeSize(_type, _type->ArraySize, FALSE);
	struct Value *NewValue = VariableAllocValueAndData(_parser, Size, _isLValue, _lValueFrom, _onHeap);
	assert(Size > 0 || _type == &VoidType);
	NewValue->Typ = _type;
	return NewValue;
}

/* allocate a value either on the heap or the stack and copy its value. handles overlapping data */
struct Value *VariableAllocValueAndCopy(struct ParseState *_parser,
                                        struct Value *_fromValue,
                                        int _onHeap) {
	struct ValueType *DType = _fromValue->Typ;
	struct Value *NewValue;
	char TmpBuf[MAX_TMP_COPY_BUF];
	int CopySize = TypeSizeValue(_fromValue, TRUE);
	assert(CopySize <= MAX_TMP_COPY_BUF);
	memcpy((void *)&TmpBuf[0], (void *)_fromValue->Val, CopySize);
	NewValue = VariableAllocValueAndData(_parser, CopySize, _fromValue->IsLValue, _fromValue->LValueFrom, _onHeap);
	NewValue->Typ = DType;
	memcpy((void *)NewValue->Val, (void *)&TmpBuf[0], CopySize);
	return NewValue;
}

/* allocate a value either on the heap or the stack from an existing AnyValue and type */
struct Value *VariableAllocValueFromExistingData(struct ParseState *_parser,
                                                 struct ValueType *_type,
                                                 union AnyValue *_fromValue,
                                                 int _isLValue,
                                                 struct Value *_lValueFrom) {
	struct Value *NewValue = VariableAlloc(_parser, sizeof(struct Value), FALSE);
	NewValue->Typ = _type;
	NewValue->Val = _fromValue;
	NewValue->ValOnHeap = FALSE;
	NewValue->ValOnStack = FALSE;
	NewValue->IsLValue = _isLValue;
	NewValue->LValueFrom = _lValueFrom;
	return NewValue;
}

/* allocate a value either on the heap or the stack from an existing Value, sharing the value */
struct Value *VariableAllocValueShared(struct ParseState *_parser,
                                       struct Value *_fromValue) {
	return VariableAllocValueFromExistingData(_parser, _fromValue->Typ, _fromValue->Val, _fromValue->IsLValue, _fromValue->IsLValue ? _fromValue : NULL);
}

/* define a variable. Ident must be registered */
struct Value *VariableDefine(struct ParseState *_parser,
                             char *_ident,
                             struct Value *_initValue,
                             struct ValueType *_type,
                             int _makeWritable) {
	struct Value *AssignValue;
	if (_initValue != NULL) {
		AssignValue = VariableAllocValueAndCopy(_parser, _initValue, TopStackFrame == NULL);
	} else {
		AssignValue = VariableAllocValueFromType(_parser, _type, _makeWritable, NULL, TopStackFrame == NULL);
	}
	AssignValue->IsLValue = _makeWritable;
	if (!TableSet((TopStackFrame == NULL) ? &GlobalTable : &TopStackFrame->LocalTable, _ident, AssignValue, _parser ? ((char *)_parser->FileName) : NULL, _parser ? _parser->Line : 0, _parser ? _parser->CharacterPos : 0)) {
		ProgramFail(_parser, "'%s' is already defined", _ident);
	}
	return AssignValue;
}

/* define a variable. Ident must be registered. If it's a redefinition from the same declaration don't throw an error */
struct Value *VariableDefineButIgnoreIdentical(struct ParseState *_parser,
                                               char *_ident,
                                               struct ValueType *_type,
                                               int _isStatic,
                                               int *_firstVisit) {
	struct Value *ExistingValue;
	const char *DeclFileName;
	int DeclLine;
	int DeclColumn;
	if (_isStatic) {
		char MangledName[LINEBUFFER_MAX];
		char *MNPos = &MangledName[0];
		char *MNEnd = &MangledName[LINEBUFFER_MAX-1];
		const char *RegisteredMangledName;
		/* make the mangled static name (avoiding using sprintf() to minimise library impact) */
		memset((void *)&MangledName, '\0', sizeof(MangledName));
		*MNPos++ = '/';
		strncpy(MNPos, (char *)_parser->FileName, MNEnd - MNPos);
		MNPos += strlen(MNPos);
		if (TopStackFrame != NULL) {
			/* we're inside a function */
			if (MNEnd - MNPos > 0) {
				*MNPos++ = '/';
			}
			strncpy(MNPos, (char *)TopStackFrame->FuncName, MNEnd - MNPos);
			MNPos += strlen(MNPos);
		}
		if (MNEnd - MNPos > 0) {
			*MNPos++ = '/';
		}
		strncpy(MNPos, _ident, MNEnd - MNPos);
		RegisteredMangledName = TableStrRegister(MangledName);
		/* is this static already defined? */
		if (!TableGet(&GlobalTable, RegisteredMangledName, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)) {
			/* define the mangled-named static variable store in the global scope */
			ExistingValue = VariableAllocValueFromType(_parser, _type, TRUE, NULL, TRUE);
			TableSet(&GlobalTable, (char *)RegisteredMangledName, ExistingValue, (char *)_parser->FileName, _parser->Line, _parser->CharacterPos);
			*_firstVisit = TRUE;
		}
		/* static variable exists in the global scope - now make a mirroring variable in our own scope with the short name */
		VariableDefinePlatformVar(_parser, _ident, ExistingValue->Typ, ExistingValue->Val, TRUE);
		return ExistingValue;
	} else {
		if (    _parser->Line != 0
		     && TableGet((TopStackFrame == NULL) ? &GlobalTable : &TopStackFrame->LocalTable, _ident, &ExistingValue, &DeclFileName, &DeclLine, &DeclColumn)
		     && DeclFileName == _parser->FileName
		     && DeclLine == _parser->Line
		     && DeclColumn == _parser->CharacterPos) {
			return ExistingValue;
		} else {
			return VariableDefine(_parser, _ident, NULL, _type, TRUE);
		}
	}
}

/* check if a variable with a given name is defined. Ident must be registered */
int VariableDefined(const char *_ident) {
	struct Value *FoundValue;
	if (    TopStackFrame == NULL
	     || !TableGet(&TopStackFrame->LocalTable, _ident, &FoundValue, NULL, NULL, NULL)) {
		if (!TableGet(&GlobalTable, _ident, &FoundValue, NULL, NULL, NULL)) {
			return FALSE;
		}
	}
	return TRUE;
}

/* get the value of a variable. must be defined. Ident must be registered */
void VariableGet(struct ParseState *_parser,
                 const char *_ident,
                 struct Value **_lValue) {
	if (    TopStackFrame == NULL
	     || !TableGet(&TopStackFrame->LocalTable, _ident, _lValue, NULL, NULL, NULL)) {
		if (!TableGet(&GlobalTable, _ident, _lValue, NULL, NULL, NULL)) {
			ProgramFail(_parser, "'%s' is undefined", _ident);
		}
	}
}

/* define a global variable shared with a platform global. Ident will be registered */
void VariableDefinePlatformVar(struct ParseState *_parser,
                               char *_ident,
                               struct ValueType *_type,
                               union AnyValue *_fromValue,
                               int _isWritable) {
	struct Value *SomeValue = VariableAllocValueAndData(NULL, 0, _isWritable, NULL, TRUE);
	SomeValue->Typ = _type;
	SomeValue->Val = _fromValue;
	if (!TableSet((TopStackFrame == NULL) ? &GlobalTable : &TopStackFrame->LocalTable, TableStrRegister(_ident), SomeValue, _parser ? _parser->FileName : NULL, _parser ? _parser->Line : 0, _parser ? _parser->CharacterPos : 0)) {
		ProgramFail(_parser, "'%s' is already defined", _ident);
	}
}

/* free and/or pop the top value off the stack. Var must be the top value on the stack! */
void VariableStackPop(struct ParseState *_parser,
                      struct Value *_variable) {
	int Success;
#ifdef DEBUG_HEAP
	if (_variable->ValOnStack) {
		printf("popping %ld at 0x%lx\n", (unsigned long)(sizeof(struct Value) + TypeSizeValue(_variable, FALSE)), (unsigned long)_variable);
	}
#endif
	if (_variable->ValOnHeap) {
		if (_variable->Val != NULL) {
			HeapFreeMem(_variable->Val);
		}
		Success = HeapPopStack(_variable, sizeof(struct Value));
		/* free from heap */
	} else if (_variable->ValOnStack) {
		Success = HeapPopStack(_variable, sizeof(struct Value) + TypeSizeValue(_variable, FALSE));  /* free from stack */
	} else {
		Success = HeapPopStack(_variable, sizeof(struct Value));					   /* value isn't our problem */
	}
	if (!Success) {
		ProgramFail(_parser, "stack underrun");
	}
}

/* add a stack frame when doing a function call */
void VariableStackFrameAdd(struct ParseState *_parser,
                           const char *_functionName,
                           int _numberParameters) {
	struct StackFrame *NewFrame;
	HeapPushStackFrame();
	NewFrame = HeapAllocStack(sizeof(struct StackFrame) + sizeof(struct Value *) * _numberParameters);
	if (NewFrame == NULL) {
		ProgramFail(_parser, "out of memory");
	}
	ParserCopy(&NewFrame->ReturnParser, _parser);
	NewFrame->FuncName = _functionName;
	NewFrame->Parameter = (_numberParameters > 0) ? ((void *)((char *)NewFrame + sizeof(struct StackFrame))) : NULL;
	TableInitTable(&NewFrame->LocalTable, &NewFrame->LocalHashTable[0], LOCAL_TABLE_SIZE, FALSE);
	NewFrame->PreviousStackFrame = TopStackFrame;
	TopStackFrame = NewFrame;
}

/* remove a stack frame */
void VariableStackFramePop(struct ParseState *_parser) {
	if (TopStackFrame == NULL) {
		ProgramFail(_parser, "stack is empty - can't go back");
	}
	ParserCopy(_parser, &TopStackFrame->ReturnParser);
	TopStackFrame = TopStackFrame->PreviousStackFrame;
	HeapPopStackFrame();
}

/* get a string literal. assumes that Ident is already registered. NULL if not found */
struct Value *VariableStringLiteralGet(char *_ident) {
	struct Value *LVal = NULL;
	if (TableGet(&StringLiteralTable, _ident, &LVal, NULL, NULL, NULL)) {
		return LVal;
	} else {
		return NULL;
	}
}

/* define a string literal. assumes that Ident is already registered */
void VariableStringLiteralDefine(char *_ident,
                                 struct Value *_value) {
	TableSet(&StringLiteralTable, _ident, _value, NULL, 0, 0);
}

/* check a pointer for validity and dereference it for use */
void *VariableDereferencePointer(struct ParseState *_parser,
                                 struct Value *_pointerValue,
                                 struct Value **_dereferenceValue,
                                 int *_dereferenceOffset,
                                 struct ValueType **_dereferenceType,
                                 int *_dereferenceIsLValue) {
	if (_dereferenceValue != NULL) {
		*_dereferenceValue = NULL;
	}
	if (_dereferenceType != NULL) {
		*_dereferenceType = _pointerValue->Typ->FromType;
	}
	if (_dereferenceOffset != NULL) {
		*_dereferenceOffset = 0;
	}
	if (_dereferenceIsLValue != NULL) {
		*_dereferenceIsLValue = TRUE;
	}
	return _pointerValue->Val->Pointer;
}

