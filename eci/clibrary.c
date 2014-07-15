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
	VersionString = TableStrRegister(PICOC_VERSION);
	VariableDefinePlatformVar(NULL, "PICOC_VERSION", CharPtrType, (union AnyValue *)&VersionString, FALSE);
	/* define endian-ness macros */
	BigEndian = ((*(char*)&__ENDIAN_CHECK__) == 0);
	LittleEndian = ((*(char*)&__ENDIAN_CHECK__) == 1);
	VariableDefinePlatformVar(NULL, "BIG_ENDIAN", &IntType, (union AnyValue *)&BigEndian, FALSE);
	VariableDefinePlatformVar(NULL, "LITTLE_ENDIAN", &IntType, (union AnyValue *)&LittleEndian, FALSE);
}

/* add a library */
void LibraryAdd(struct Table *GlobalTable, const char *LibraryName, struct LibraryFunction *FuncList) {
	struct ParseState Parser;
	int Count;
	char *Identifier;
	struct ValueType *ReturnType;
	struct Value *NewValue;
	void *Tokens;
	const char *IntrinsicName = TableStrRegister("c library");
	/* read all the library definitions */
	for (Count = 0; FuncList[Count].Prototype != NULL; Count++) {
		Tokens = LexAnalyse(IntrinsicName, FuncList[Count].Prototype, strlen((char *)FuncList[Count].Prototype), NULL);
		LexInitParser(&Parser, FuncList[Count].Prototype, Tokens, IntrinsicName, TRUE);
		TypeParse(&Parser, &ReturnType, &Identifier, NULL);
		NewValue = ParseFunctionDefinition(&Parser, ReturnType, Identifier);
		NewValue->Val->FuncDef.Intrinsic = FuncList[Count].Func;
		HeapFreeMem(Tokens);
	}
}

/* print a type to a stream without using printf/sprintf */
void PrintType(struct ValueType *Typ, IOFILE *Stream) {
	switch (Typ->Base) {
		case TypeVoid:
			PrintStr("void", Stream);
			break;
		case TypeInt:
			PrintStr("int", Stream);
			break;
		case TypeShort:
			PrintStr("short", Stream);
			break;
		case TypeChar:
			PrintStr("char", Stream);
			break;
		case TypeLong:
			PrintStr("long", Stream);
			break;
		case TypeUnsignedInt:
			PrintStr("unsigned int", Stream);
			break;
		case TypeUnsignedShort:
			PrintStr("unsigned short", Stream);
			break;
		case TypeUnsignedLong:
			PrintStr("unsigned long", Stream);
			break;
		case TypeFP:
			PrintStr("double", Stream);
			break;
		case TypeFunction:
			PrintStr("function", Stream);
			break;
		case TypeMacro:
			PrintStr("macro", Stream);
			break;
		case TypePointer:
			if (Typ->FromType) {
				PrintType(Typ->FromType, Stream);
			}
			PrintCh('*', Stream);
			break;
		case TypeArray:
			PrintType(Typ->FromType, Stream);
			PrintCh('[', Stream);
			if (Typ->ArraySize != 0) {
				PrintSimpleInt(Typ->ArraySize, Stream);
			}
			PrintCh(']', Stream);
			break;
		case TypeStruct:
			PrintStr("struct ", Stream);
			PrintStr(Typ->Identifier, Stream);
			break;
		case TypeUnion:
			PrintStr("union ", Stream);
			PrintStr(Typ->Identifier, Stream);
			break;
		case TypeEnum:
			PrintStr("enum ", Stream);
			PrintStr(Typ->Identifier, Stream);
			break;
		case TypeGotoLabel:
			PrintStr("goto label ", Stream);
			break;
		case Type_Type:
			PrintStr("type ", Stream);
			break;
	}
}

