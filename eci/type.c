/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include "interpreter.h"

/* some basic types */
struct ValueType UberType;
struct ValueType IntType;
struct ValueType ShortType;
struct ValueType CharType;
struct ValueType LongType;
struct ValueType UnsignedIntType;
struct ValueType UnsignedShortType;
struct ValueType UnsignedLongType;
struct ValueType FPType;
struct ValueType VoidType;
struct ValueType TypeType;
struct ValueType FunctionType;
struct ValueType MacroType;
struct ValueType EnumType;
struct ValueType GotoLabelType;
struct ValueType *CharPtrType;
struct ValueType *CharPtrPtrType;
struct ValueType *CharArrayType;
struct ValueType *VoidPtrType;

static int PointerAlignBytes;
static int IntAlignBytes;

/* add a new type to the set of types we know about */
struct ValueType *TypeAdd(struct ParseState *_parser, struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int Sizeof, int AlignBytes) {
	struct ValueType *NewType = VariableAlloc(_parser, sizeof(struct ValueType), TRUE);
	NewType->Base = Base;
	NewType->ArraySize = ArraySize;
	NewType->Sizeof = Sizeof;
	NewType->AlignBytes = AlignBytes;
	NewType->Identifier = Identifier;
	NewType->Members = NULL;
	NewType->FromType = ParentType;
	NewType->DerivedTypeList = NULL;
	NewType->OnHeap = TRUE;
	NewType->Next = ParentType->DerivedTypeList;
	ParentType->DerivedTypeList = NewType;
	return NewType;
}

/* given a parent type, get a matching derived type and make one if necessary.
 * Identifier should be registered with the shared string table. */
struct ValueType *TypeGetMatching(struct ParseState *_parser, struct ValueType *ParentType, enum BaseType Base, int ArraySize, const char *Identifier, int AllowDuplicates) {
	int Sizeof;
	int AlignBytes;
	struct ValueType *ThisType = ParentType->DerivedTypeList;
	while (    ThisType != NULL
	        && (    ThisType->Base != Base
	             || ThisType->ArraySize != ArraySize
	             || ThisType->Identifier != Identifier) ) {
		ThisType = ThisType->Next;
	}
	if (ThisType != NULL) {
		if (AllowDuplicates) {
			return ThisType;
		} else {
			ProgramFail(_parser, "data type '%s' is already defined", Identifier);
		}
	}
	switch (Base) {
		case TypePointer:
			Sizeof = sizeof(void *);
			AlignBytes = PointerAlignBytes;
			break;
		case TypeArray:
			Sizeof = ArraySize * ParentType->Sizeof;
			AlignBytes = ParentType->AlignBytes;
			break;
		case TypeEnum:
			Sizeof = sizeof(int);
			AlignBytes = IntAlignBytes;
			break;
		default:
			Sizeof = 0;
			AlignBytes = 0;
			break;
			/* structs and unions will get bigger when we add members to them */
	}
	return TypeAdd(_parser, ParentType, Base, ArraySize, Identifier, Sizeof, AlignBytes);
}

/* stack space used by a value */
int TypeStackSizeValue(struct Value *Val) {
	if (Val != NULL && Val->ValOnStack) {
		return TypeSizeValue(Val, FALSE);
	} else {
		return 0;
	}
}

/* memory used by a value */
int TypeSizeValue(struct Value *Val, int Compact) {
	if (IS_INTEGER_NUMERIC(Val) && !Compact) {
		return sizeof(ALIGN_TYPE);	 /* allow some extra room for type extension */
	} else if (Val->Typ->Base != TypeArray) {
		return Val->Typ->Sizeof;
	} else {
		return Val->Typ->FromType->Sizeof * Val->Typ->ArraySize;
	}
}

/* memory used by a variable given its type and array size */
int TypeSize(struct ValueType *Typ, int ArraySize, int Compact) {
	if (IS_INTEGER_NUMERIC_TYPE(Typ) && !Compact) {
		return sizeof(ALIGN_TYPE);	 /* allow some extra room for type extension */
	} else if (Typ->Base != TypeArray) {
		return Typ->Sizeof;
	} else {
		return Typ->FromType->Sizeof * ArraySize;
	}
}

/* add a base type */
void TypeAddBaseType(struct ValueType *TypeNode, enum BaseType Base, int Sizeof, int AlignBytes) {
	TypeNode->Base = Base;
	TypeNode->ArraySize = 0;
	TypeNode->Sizeof = Sizeof;
	TypeNode->AlignBytes = AlignBytes;
	TypeNode->Identifier = StrEmpty;
	TypeNode->Members = NULL;
	TypeNode->FromType = NULL;
	TypeNode->DerivedTypeList = NULL;
	TypeNode->OnHeap = FALSE;
	TypeNode->Next = UberType.DerivedTypeList;
	UberType.DerivedTypeList = TypeNode;
}

/* initialise the type system */
void TypeInit() {
	struct IntAlign {
		char x;
		int y;
	} ia;
	struct ShortAlign {
		char x;
		short y;
	} sa;
	struct CharAlign {
		char x;
		char y;
	} ca;
	struct LongAlign {
		char x;
		long y;
	} la;
	struct DoubleAlign {
		char x;
		double y;
	} da;
	struct PointerAlign {
		char x;
		void *y;
	} pa;
	
	IntAlignBytes = (char *)&ia.y - &ia.x;
	PointerAlignBytes = (char *)&pa.y - &pa.x;
	
	UberType.DerivedTypeList = NULL;
	TypeAddBaseType(&IntType, TypeInt, sizeof(int), IntAlignBytes);
	TypeAddBaseType(&ShortType, TypeShort, sizeof(short), (char *)&sa.y - &sa.x);
	TypeAddBaseType(&CharType, TypeChar, sizeof(unsigned char), (char *)&ca.y - &ca.x);
	TypeAddBaseType(&LongType, TypeLong, sizeof(long), (char *)&la.y - &la.x);
	TypeAddBaseType(&UnsignedIntType, TypeUnsignedInt, sizeof(unsigned int), IntAlignBytes);
	TypeAddBaseType(&UnsignedShortType, TypeUnsignedShort, sizeof(unsigned short), (char *)&sa.y - &sa.x);
	TypeAddBaseType(&UnsignedLongType, TypeUnsignedLong, sizeof(unsigned long), (char *)&la.y - &la.x);
	TypeAddBaseType(&VoidType, TypeVoid, 0, 1);
	TypeAddBaseType(&FunctionType, TypeFunction, sizeof(int), IntAlignBytes);
	TypeAddBaseType(&MacroType, TypeMacro, sizeof(int), IntAlignBytes);
	TypeAddBaseType(&GotoLabelType, TypeGotoLabel, 0, 1);
	TypeAddBaseType(&FPType, TypeFP, sizeof(double), (char *)&da.y - &da.x);
	TypeAddBaseType(&TypeType, Type_Type, sizeof(double), (char *)&da.y - &da.x);  /* must be large enough to cast to a double */
	CharArrayType = TypeAdd(NULL, &CharType, TypeArray, 0, StrEmpty, sizeof(char), (char *)&ca.y - &ca.x);
	CharPtrType = TypeAdd(NULL, &CharType, TypePointer, 0, StrEmpty, sizeof(void *), PointerAlignBytes);
	CharPtrPtrType = TypeAdd(NULL, CharPtrType, TypePointer, 0, StrEmpty, sizeof(void *), PointerAlignBytes);
	VoidPtrType = TypeAdd(NULL, &VoidType, TypePointer, 0, StrEmpty, sizeof(void *), PointerAlignBytes);
}

/* deallocate heap-allocated types */
void TypeCleanupNode(struct ValueType *Typ) {
	struct ValueType *SubType;
	struct ValueType *NextSubType;
	/* clean up and free all the sub-nodes */
	for (SubType = Typ->DerivedTypeList; SubType != NULL; SubType = NextSubType) {
		NextSubType = SubType->Next;
		TypeCleanupNode(SubType);
		if (SubType->OnHeap) {
			/* if it's a struct or union deallocate all the member values */
			if (SubType->Members != NULL) {
				VariableTableCleanup(SubType->Members);
				HeapFreeMem(SubType->Members);
			}
			/* free this node */
			HeapFreeMem(SubType);
		}
	}
}

void TypeCleanup() {
	TypeCleanupNode(&UberType);
}

/* parse a struct or union declaration */
void TypeParseStruct(struct ParseState *_parser, struct ValueType **Typ, int IsStruct) {
	struct Value *LexValue;
	struct ValueType *MemberType;
	char *MemberIdentifier;
	char *StructIdentifier;
	struct Value *MemberValue;
	enum LexToken Token;
	int AlignBoundary;
	Token = LexGetToken(_parser, &LexValue, FALSE);
	if (Token == TokenIdentifier) {
		LexGetToken(_parser, &LexValue, TRUE);
		StructIdentifier = LexValue->Val->Identifier;
		Token = LexGetToken(_parser, NULL, FALSE);
	} else {
		static char TempNameBuf[7] = "^s0000";
		StructIdentifier = PlatformMakeTempName(TempNameBuf);
	}
	*Typ = TypeGetMatching(_parser, &UberType, IsStruct ? TypeStruct : TypeUnion, 0, StructIdentifier, Token != TokenLeftBrace);
	Token = LexGetToken(_parser, NULL, FALSE);
	if (Token != TokenLeftBrace) {
		/* use the already defined structure */
		if ((*Typ)->Members == NULL) {
			ProgramFail(_parser, "structure '%s' isn't defined", LexValue->Val->Identifier);
		}
		return;
	}
	if (TopStackFrame != NULL) {
		ProgramFail(_parser, "struct/union definitions can only be globals");
	}
	LexGetToken(_parser, NULL, TRUE);
	(*Typ)->Members = VariableAlloc(_parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE);
	(*Typ)->Members->HashTable = (struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table));
	TableInitTable((*Typ)->Members, (struct TableEntry **)((char *)(*Typ)->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
	do {
		TypeParse(_parser, &MemberType, &MemberIdentifier, NULL);
		if (MemberType == NULL || MemberIdentifier == NULL) {
			ProgramFail(_parser, "invalid type in struct");
		}
		MemberValue = VariableAllocValueAndData(_parser, sizeof(int), FALSE, NULL, TRUE);
		MemberValue->Typ = MemberType;
		if (IsStruct) {
			/* allocate this member's location in the struct */
			AlignBoundary = MemberValue->Typ->AlignBytes;
			if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0) {
				(*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
			}
			MemberValue->Val->Integer = (*Typ)->Sizeof;
			(*Typ)->Sizeof += TypeSizeValue(MemberValue, TRUE);
		} else {
			/* union members always start at 0, make sure it's big enough to hold the largest member */
			MemberValue->Val->Integer = 0;
			if (MemberValue->Typ->Sizeof > (*Typ)->Sizeof) {
				(*Typ)->Sizeof = TypeSizeValue(MemberValue, TRUE);
			}
		}
		/* make sure to align to the size of the largest member's alignment */
		if ((*Typ)->AlignBytes < MemberValue->Typ->AlignBytes) {
			(*Typ)->AlignBytes = MemberValue->Typ->AlignBytes;
		}
		/* define it */
		if (!TableSet((*Typ)->Members, MemberIdentifier, MemberValue, _parser->FileName, _parser->Line, _parser->CharacterPos)) {
			ProgramFail(_parser, "member '%s' already defined", &MemberIdentifier);
		}
		if (LexGetToken(_parser, NULL, TRUE) != TokenSemicolon) {
			ProgramFail(_parser, "semicolon expected");
		}
	} while (LexGetToken(_parser, NULL, FALSE) != TokenRightBrace);
	/* now align the structure to the size of its largest member's alignment */
	AlignBoundary = (*Typ)->AlignBytes;
	if (((*Typ)->Sizeof & (AlignBoundary-1)) != 0) {
		(*Typ)->Sizeof += AlignBoundary - ((*Typ)->Sizeof & (AlignBoundary-1));
	}
	LexGetToken(_parser, NULL, TRUE);
}

/* create a system struct which has no user-visible members */
struct ValueType *TypeCreateOpaqueStruct(struct ParseState *_parser, const char *StructName, int Size) {
	struct ValueType *Typ = TypeGetMatching(_parser, &UberType, TypeStruct, 0, StructName, FALSE);
	/* create the (empty) table */
	Typ->Members = VariableAlloc(_parser, sizeof(struct Table) + STRUCT_TABLE_SIZE * sizeof(struct TableEntry), TRUE);
	Typ->Members->HashTable = (struct TableEntry **)((char *)Typ->Members + sizeof(struct Table));
	TableInitTable(Typ->Members, (struct TableEntry **)((char *)Typ->Members + sizeof(struct Table)), STRUCT_TABLE_SIZE, TRUE);
	Typ->Sizeof = Size;
	return Typ;
}

/* parse an enum declaration */
void TypeParseEnum(struct ParseState *_parser, struct ValueType **Typ) {
	struct Value *LexValue;
	struct Value InitValue;
	enum LexToken Token;
	struct ValueType *EnumType;
	int EnumValue = 0;
	char *EnumIdentifier;
	Token = LexGetToken(_parser, &LexValue, FALSE);
	if (Token == TokenIdentifier) {
		LexGetToken(_parser, &LexValue, TRUE);
		EnumIdentifier = LexValue->Val->Identifier;
		Token = LexGetToken(_parser, NULL, FALSE);
	} else {
		static char TempNameBuf[7] = "^e0000";
		EnumIdentifier = PlatformMakeTempName(TempNameBuf);
	}
	EnumType = TypeGetMatching(_parser, &UberType, TypeEnum, 0, EnumIdentifier, Token != TokenLeftBrace);
	*Typ = &IntType;
	if (Token != TokenLeftBrace) {
		/* use the already defined enum */
		if ((*Typ)->Members == NULL) {
			ProgramFail(_parser, "enum '%s' isn't defined", EnumIdentifier);
		}
		return;
	}
	if (TopStackFrame != NULL) {
		ProgramFail(_parser, "enum definitions can only be globals");
	}
	LexGetToken(_parser, NULL, TRUE);
	(*Typ)->Members = &GlobalTable;
	memset((void *)&InitValue, '\0', sizeof(struct Value));
	InitValue.Typ = &IntType;
	InitValue.Val = (union AnyValue *)&EnumValue;
	do {
		if (LexGetToken(_parser, &LexValue, TRUE) != TokenIdentifier) {
			ProgramFail(_parser, "identifier expected");
		}
		EnumIdentifier = LexValue->Val->Identifier;
		if (LexGetToken(_parser, NULL, FALSE) == TokenAssign) {
			LexGetToken(_parser, NULL, TRUE);
			EnumValue = ExpressionParseInt(_parser);
		}
		VariableDefine(_parser, EnumIdentifier, &InitValue, NULL, FALSE);
		
		Token = LexGetToken(_parser, NULL, TRUE);
		if (Token != TokenComma && Token != TokenRightBrace) {
			ProgramFail(_parser, "comma expected");
		}
		EnumValue++;
	} while (Token == TokenComma);
}

/* parse a type - just the basic type */
int TypeParseFront(struct ParseState *_parser, struct ValueType **Typ, int *IsStatic) {
	struct ParseState Before;
	struct Value *LexerValue;
	enum LexToken Token;
	int Unsigned = FALSE;
	struct Value *VarValue;
	int StaticQualifier = FALSE;
	*Typ = NULL;
	/* ignore leading type qualifiers */
	ParserCopy(&Before, _parser);
	Token = LexGetToken(_parser, &LexerValue, TRUE);
	while (    Token == TokenStaticType
	        || Token == TokenAutoType
	        || Token == TokenRegisterType
	        || Token == TokenExternType) {
		if (Token == TokenStaticType) {
			StaticQualifier = TRUE;
		}
		Token = LexGetToken(_parser, &LexerValue, TRUE);
	}
	if (IsStatic != NULL) {
		*IsStatic = StaticQualifier;
	}
	/* handle signed/unsigned with no trailing type */
	if (Token == TokenSignedType || Token == TokenUnsignedType) {
		enum LexToken FollowToken = LexGetToken(_parser, &LexerValue, FALSE);
		Unsigned = (Token == TokenUnsignedType);
		if (    FollowToken != TokenIntType
		     && FollowToken != TokenLongType
		     && FollowToken != TokenShortType
		     && FollowToken != TokenCharType) {
			if (Token == TokenUnsignedType) {
				*Typ = &UnsignedIntType;
			} else {
				*Typ = &IntType;
			}
			return TRUE;
		}
		Token = LexGetToken(_parser, &LexerValue, TRUE);
	}
	switch (Token) {
		case TokenIntType:
			*Typ = Unsigned ? &UnsignedIntType : &IntType;
			break;
		case TokenShortType:
			*Typ = Unsigned ? &UnsignedShortType : &ShortType;
			break;
		case TokenCharType:
			*Typ = &CharType;
			break;
		case TokenLongType:
			*Typ = Unsigned ? &UnsignedLongType : &LongType;
			break;
		case TokenFloatType:
		case TokenDoubleType:
			*Typ = &FPType;
			break;
		case TokenVoidType:
			*Typ = &VoidType;
			break;
		case TokenStructType:
		case TokenUnionType:
			if (*Typ != NULL) {
				ProgramFail(_parser, "bad type declaration");
			}
			TypeParseStruct(_parser, Typ, Token == TokenStructType);
			break;
		case TokenEnumType:
			if (*Typ != NULL) {
				ProgramFail(_parser, "bad type declaration");
			}
			TypeParseEnum(_parser, Typ);
			break;
		case TokenIdentifier:
			/* we already know it's a typedef-defined type because we got here */
			VariableGet(_parser, LexerValue->Val->Identifier, &VarValue);
			*Typ = VarValue->Val->Typ;
			break;
		default:
			ParserCopy(_parser, &Before);
			return FALSE;
	}
	return TRUE;
}

/* parse a type - the part at the end after the identifier. eg. array specifications etc. */
struct ValueType *TypeParseBack(struct ParseState *_parser, struct ValueType *FromType) {
	enum LexToken Token;
	struct ParseState Before;
	ParserCopy(&Before, _parser);
	Token = LexGetToken(_parser, NULL, TRUE);
	if (Token == TokenLeftSquareBracket) {
		/* add another array bound */
		enum RunMode OldMode = _parser->Mode;
		int ArraySize;
		_parser->Mode = RunModeRun;
		ArraySize = ExpressionParseInt(_parser);
		_parser->Mode = OldMode;
		if (LexGetToken(_parser, NULL, TRUE) != TokenRightSquareBracket) {
			ProgramFail(_parser, "']' expected");
		}
		return TypeGetMatching(_parser, TypeParseBack(_parser, FromType), TypeArray, ArraySize, StrEmpty, TRUE);
	} else {
		/* the type specification has finished */
		ParserCopy(_parser, &Before);
		return FromType;
	}
}

/* parse a type - the part which is repeated with each identifier in a declaration list */
void TypeParseIdentPart(struct ParseState *_parser, struct ValueType *BasicTyp, struct ValueType **Typ, char **Identifier) {
	struct ParseState Before;
	enum LexToken Token;
	struct Value *LexValue;
	int Done = FALSE;
	*Typ = BasicTyp;
	*Identifier = StrEmpty;
	while (!Done) {
		ParserCopy(&Before, _parser);
		Token = LexGetToken(_parser, &LexValue, TRUE);
		switch (Token) {
			case TokenOpenBracket:
				if (*Typ != NULL) {
					ProgramFail(_parser, "bad type declaration");
				}
				TypeParse(_parser, Typ, Identifier, NULL);
				if (LexGetToken(_parser, NULL, TRUE) != TokenCloseBracket) {
					ProgramFail(_parser, "')' expected");
				}
				break;
			case TokenAsterisk:
				if (*Typ == NULL) {
					ProgramFail(_parser, "bad type declaration");
				}
				*Typ = TypeGetMatching(_parser, *Typ, TypePointer, 0, StrEmpty, TRUE);
				break;
			case TokenIdentifier:
				if (    *Typ == NULL
				     || *Identifier != StrEmpty) {
					ProgramFail(_parser, "bad type declaration");
				}
				*Identifier = LexValue->Val->Identifier;
				Done = TRUE;
				break;
			default:
				ParserCopy(_parser, &Before);
				Done = TRUE;
				break;
		}
	}
	if (*Typ == NULL) {
		ProgramFail(_parser, "bad type declaration");
	}
	if (*Identifier != StrEmpty) {
		/* parse stuff after the identifier */
		*Typ = TypeParseBack(_parser, *Typ);
	}
}

/* parse a type - a complete declaration including identifier */
void TypeParse(struct ParseState *_parser, struct ValueType **Typ, char **Identifier, int *IsStatic) {
	struct ValueType *BasicType;
	TypeParseFront(_parser, &BasicType, IsStatic);
	TypeParseIdentPart(_parser, BasicType, Typ, Identifier);
}

