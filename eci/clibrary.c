/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include "picoc.h"
#include "interpreter.h"

/* the picoc version string */
static const char *VersionString = NULL;

/* endian-ness checking */
static const int __ENDIAN_CHECK__ = 1;

static int BigEndian = 0;
static int LittleEndian = 0;


/* global initialisation for libraries */
void LibraryInit() {
	/* define the version number macro */
	VersionString = TableStrRegister(ECI_VERSION);
	VariableDefinePlatformVar(NULL, "ECI_VERSION", CharPtrType, (union AnyValue *)&VersionString, FALSE);
	/* define endian-ness macros */
	BigEndian = ((*(char*)&__ENDIAN_CHECK__) == 0);
	LittleEndian = ((*(char*)&__ENDIAN_CHECK__) == 1);
	VariableDefinePlatformVar(NULL, "BIG_ENDIAN", &IntType, (union AnyValue *)&BigEndian, FALSE);
	VariableDefinePlatformVar(NULL, "LITTLE_ENDIAN", &IntType, (union AnyValue *)&LittleEndian, FALSE);
}

/* add a library */
void LibraryAdd(struct Table *_globalTable, const char *_libraryName, struct LibraryFunction *_funcList) {
	struct ParseState Parser;
	int Count;
	char *Identifier;
	struct ValueType *ReturnType;
	struct Value *NewValue;
	void *Tokens;
	const char *IntrinsicName = TableStrRegister("c library");
	/* read all the library definitions */
	for (Count = 0; _funcList[Count].Prototype != NULL; Count++) {
		Tokens = LexAnalyse(IntrinsicName, _funcList[Count].Prototype, strlen((char *)_funcList[Count].Prototype), NULL);
		LexInitParser(&Parser, _funcList[Count].Prototype, Tokens, IntrinsicName, TRUE);
		TypeParse(&Parser, &ReturnType, &Identifier, NULL);
		NewValue = ParseFunctionDefinition(&Parser, ReturnType, Identifier);
		NewValue->Val->FuncDef.Intrinsic = _funcList[Count].Func;
		HeapFreeMem(Tokens);
	}
}

/* print a type to a stream without using printf/sprintf */
void PrintType(struct ValueType* _type, IOFILE *_stream) {
	switch (_type->Base) {
		case TypeVoid:
			PrintStr("void", _stream);
			break;
		case TypeInt:
			PrintStr("int", _stream);
			break;
		case TypeShort:
			PrintStr("short", _stream);
			break;
		case TypeChar:
			PrintStr("char", _stream);
			break;
		case TypeLong:
			PrintStr("long", _stream);
			break;
		case TypeUnsignedInt:
			PrintStr("unsigned int", _stream);
			break;
		case TypeUnsignedShort:
			PrintStr("unsigned short", _stream);
			break;
		case TypeUnsignedLong:
			PrintStr("unsigned long", _stream);
			break;
		case TypeFP:
			PrintStr("double", _stream);
			break;
		case TypeFunction:
			PrintStr("function", _stream);
			break;
		case TypeMacro:
			PrintStr("macro", _stream);
			break;
		case TypePointer:
			if (_type->FromType) {
				PrintType(_type->FromType, _stream);
			}
			PrintCh('*', _stream);
			break;
		case TypeArray:
			PrintType(_type->FromType, _stream);
			PrintCh('[', _stream);
			if (_type->ArraySize != 0) {
				PrintSimpleInt(_type->ArraySize, _stream);
			}
			PrintCh(']', _stream);
			break;
		case TypeStruct:
			PrintStr("struct ", _stream);
			PrintStr(_type->Identifier, _stream);
			break;
		case TypeUnion:
			PrintStr("union ", _stream);
			PrintStr(_type->Identifier, _stream);
			break;
		case TypeEnum:
			PrintStr("enum ", _stream);
			PrintStr(_type->Identifier, _stream);
			break;
		case TypeGotoLabel:
			PrintStr("goto label ", _stream);
			break;
		case Type_Type:
			PrintStr("type ", _stream);
			break;
	}
}

