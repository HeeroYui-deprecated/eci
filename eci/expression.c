/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include "interpreter.h"


/* whether evaluation is left to right for a given precedence level */
#define IS_LEFT_TO_RIGHT(p) ((p) != 2 && (p) != 14)
#define BRACKET_PRECEDENCE 20
#define IS_TYPE_TOKEN(t) ((t) >= TokenIntType && (t) <= TokenUnsignedType)

#define DEEP_PRECEDENCE (BRACKET_PRECEDENCE*1000)

#ifdef DEBUG_EXPRESSIONS
#define debugf printf
#else
void debugf(char *Format, ...) {
}
#endif

/* local prototypes */
enum OperatorOrder {
	OrderNone,
	OrderPrefix,
	OrderInfix,
	OrderPostfix
};

/* a stack of expressions we use in evaluation */
struct ExpressionStack
{
	struct ExpressionStack *Next;	   /* the next lower item on the stack */
	struct Value *Val;				  /* the value for this stack node */
	enum LexToken Op;				   /* the operator */
	short unsigned int Precedence;	  /* the operator precedence of this node */
	unsigned char Order;				/* the evaluation order of this operator */
};

/* operator precedence definitions */
struct OpPrecedence {
	unsigned int PrefixPrecedence:4;
	unsigned int PostfixPrecedence:4;
	unsigned int InfixPrecedence:4;
	char *Name;
};

/* NOTE: the order of this array must correspond exactly to the order of these tokens in enum LexToken */
static struct OpPrecedence OperatorPrecedence[] =
{
	/* TokenNone, */ { 0, 0, 0, "none" },
	/* TokenComma, */ { 0, 0, 0, "," },
	/* TokenAssign, */ { 0, 0, 2, "=" },
	/* TokenAddAssign, */ { 0, 0, 2, "+=" },
	/* TokenSubtractAssign, */ { 0, 0, 2, "-=" }, 
	/* TokenMultiplyAssign, */ { 0, 0, 2, "*=" },
	/* TokenDivideAssign, */ { 0, 0, 2, "/=" },
	/* TokenModulusAssign, */ { 0, 0, 2, "%=" },
	/* TokenShiftLeftAssign, */ { 0, 0, 2, "<<=" },
	/* TokenShiftRightAssign, */ { 0, 0, 2, ">>=" },
	/* TokenArithmeticAndAssign, */ { 0, 0, 2, "&=" }, 
	/* TokenArithmeticOrAssign, */ { 0, 0, 2, "|=" },
	/* TokenArithmeticExorAssign, */ { 0, 0, 2, "^=" },
	/* TokenQuestionMark, */ { 0, 0, 3, "?" },
	/* TokenColon, */ { 0, 0, 3, ":" },
	/* TokenLogicalOr, */ { 0, 0, 4, "||" },
	/* TokenLogicalAnd, */ { 0, 0, 5, "&&" },
	/* TokenArithmeticOr, */ { 0, 0, 6, "|" },
	/* TokenArithmeticExor, */ { 0, 0, 7, "^" },
	/* TokenAmpersand, */ { 14, 0, 8, "&" },
	/* TokenEqual, */  { 0, 0, 9, "==" },
	/* TokenNotEqual, */ { 0, 0, 9, "!=" },
	/* TokenLessThan, */ { 0, 0, 10, "<" },
	/* TokenGreaterThan, */ { 0, 0, 10, ">" },
	/* TokenLessEqual, */ { 0, 0, 10, "<=" },
	/* TokenGreaterEqual, */ { 0, 0, 10, ">=" },
	/* TokenShiftLeft, */ { 0, 0, 11, "<<" },
	/* TokenShiftRight, */ { 0, 0, 11, ">>" },
	/* TokenPlus, */ { 14, 0, 12, "+" },
	/* TokenMinus, */ { 14, 0, 12, "-" },
	/* TokenAsterisk, */ { 14, 0, 13, "*" },
	/* TokenSlash, */ { 0, 0, 13, "/" },
	/* TokenModulus, */ { 0, 0, 13, "%" },
	/* TokenIncrement, */ { 14, 15, 0, "++" },
	/* TokenDecrement, */ { 14, 15, 0, "--" },
	/* TokenUnaryNot, */ { 14, 0, 0, "!" },
	/* TokenUnaryExor, */ { 14, 0, 0, "~" },
	/* TokenSizeof, */ { 14, 0, 0, "sizeof" },
	/* TokenCast, */ { 14, 0, 0, "cast" },
	/* TokenLeftSquareBracket, */ { 0, 0, 15, "[" },
	/* TokenRightSquareBracket, */ { 0, 15, 0, "]" },
	/* TokenDot, */ { 0, 0, 15, "." },
	/* TokenArrow, */ { 0, 0, 15, "->" },
	/* TokenOpenBracket, */ { 15, 0, 0, "(" },
	/* TokenCloseBracket, */ { 0, 15, 0, ")" }
};

void ExpressionParseFunctionCall(struct ParseState *_parser, struct ExpressionStack **_stackTop, const char *_funcName, int _runIt);

#ifdef DEBUG_EXPRESSIONS
/* show the contents of the expression stack */
void ExpressionStackShow(struct ExpressionStack *_stackTop) {
	printf("Expression stack [0x%lx,0x%lx]: ", (long)HeapStackTop, (long)_stackTop);
	
	while (_stackTop != NULL) {
		if (_stackTop->Order == OrderNone) { 
			/* it's a value */
			if (_stackTop->Val->IsLValue) {
				printf("lvalue=");
			}else {
				printf("value=");
			}
			switch (_stackTop->Val->Typ->Base) {
				case TypeVoid:          printf("void"); break;
				case TypeInt:           printf("%d:int", _stackTop->Val->Val->Integer); break;
				case TypeShort:         printf("%d:short", _stackTop->Val->Val->ShortInteger); break;
				case TypeChar:          printf("%d:char", _stackTop->Val->Val->Character); break;
				case TypeLong:          printf("%d:long", _stackTop->Val->Val->LongInteger); break;
				case TypeUnsignedShort: printf("%d:unsigned short", _stackTop->Val->Val->UnsignedShortInteger); break;
				case TypeUnsignedInt:   printf("%d:unsigned int", _stackTop->Val->Val->UnsignedInteger); break;
				case TypeUnsignedLong:  printf("%d:unsigned long", _stackTop->Val->Val->UnsignedLongInteger); break;
				case TypeFP:            printf("%f:fp", _stackTop->Val->Val->FP); break;
				case TypeFunction:      printf("%s:function", _stackTop->Val->Val->Identifier); break;
				case TypeMacro:         printf("%s:macro", _stackTop->Val->Val->Identifier); break;
				case TypePointer:
					if (_stackTop->Val->Val->Pointer == NULL)
						printf("ptr(NULL)");
					else if (_stackTop->Val->Typ->FromType->Base == TypeChar)
						printf("\"%s\":string", (char *)_stackTop->Val->Val->Pointer);
					else
						printf("ptr(0x%lx)", (long)_stackTop->Val->Val->Pointer); 
					break;
				case TypeArray:   printf("array"); break;
				case TypeStruct:  printf("%s:struct", _stackTop->Val->Val->Identifier); break;
				case TypeUnion:   printf("%s:union", _stackTop->Val->Val->Identifier); break;
				case TypeEnum:    printf("%s:enum", _stackTop->Val->Val->Identifier); break;
				case Type_Type:   PrintType(_stackTop->Val->Val->Typ, CStdOut); printf(":type"); break;
				default:          printf("unknown"); break;
			}
			printf("[0x%lx,0x%lx]", (long)_stackTop, (long)_stackTop->Val);
		} else {
			/* it's an operator */
			printf("op='%s' %s %d", OperatorPrecedence[(int)_stackTop->Op].Name, 
				(_stackTop->Order == OrderPrefix) ? "prefix" : ((_stackTop->Order == OrderPostfix) ? "postfix" : "infix"), 
				_stackTop->Precedence);
			printf("[0x%lx]", (long)_stackTop);
		}
		_stackTop = _stackTop->Next;
		if (_stackTop != NULL) {
			printf(", ");
		}
	}
	printf("\n");
}
#endif

long ExpressionCoerceInteger(struct Value *_value) {
	switch (_value->Typ->Base) {
		case TypeInt:
			return (long)_value->Val->Integer;
		case TypeChar:
			return (long)_value->Val->Character;
		case TypeShort:
			return (long)_value->Val->ShortInteger;
		case TypeLong:
			return (long)_value->Val->LongInteger;
		case TypeUnsignedInt:
			return (long)_value->Val->UnsignedInteger;
		case TypeUnsignedShort:
			return (long)_value->Val->UnsignedShortInteger;
		case TypeUnsignedLong:
			return (long)_value->Val->UnsignedLongInteger;
		case TypePointer:
			return (long)_value->Val->Pointer;
		case TypeFP:
			return (long)_value->Val->FP;
		default:
			return 0;
	}
}

unsigned long ExpressionCoerceUnsignedInteger(struct Value *_value) {
	switch (_value->Typ->Base) {
		case TypeInt:
			return (unsigned long)_value->Val->Integer;
		case TypeChar:
			return (unsigned long)_value->Val->Character;
		case TypeShort:
			return (unsigned long)_value->Val->ShortInteger;
		case TypeLong:
			return (unsigned long)_value->Val->LongInteger;
		case TypeUnsignedInt:
			return (unsigned long)_value->Val->UnsignedInteger;
		case TypeUnsignedShort:
			return (unsigned long)_value->Val->UnsignedShortInteger;
		case TypeUnsignedLong:
			return (unsigned long)_value->Val->UnsignedLongInteger;
		case TypePointer:
			return (unsigned long)_value->Val->Pointer;
		case TypeFP:
			return (unsigned long)_value->Val->FP;
		default:
			return 0;
	}
}

double ExpressionCoerceFP(struct Value *_value) {
	switch (_value->Typ->Base) {
		case TypeInt:
			return (double)_value->Val->Integer;
		case TypeChar:
			return (double)_value->Val->Character;
		case TypeShort:
			return (double)_value->Val->ShortInteger;
		case TypeLong:
			return (double)_value->Val->LongInteger;
		case TypeUnsignedInt:
			return (double)_value->Val->UnsignedInteger;
		case TypeUnsignedShort:
			return (double)_value->Val->UnsignedShortInteger;
		case TypeUnsignedLong:
			return (double)_value->Val->UnsignedLongInteger;
		case TypeFP:
			return (double)_value->Val->FP;
		default:
			return 0.0;
	}
}

/* assign an integer value */
long ExpressionAssignInt(struct ParseState *_parser, struct Value *_destValue, long _fromInt, int _after) {
	long Result;
	if (!_destValue->IsLValue) {
		ProgramFail(_parser, "can't assign to this"); 
	}
	if (_after) {
		Result = ExpressionCoerceInteger(_destValue);
	} else {
		Result = _fromInt;
	}
	switch (_destValue->Typ->Base) {
		case TypeInt:
			_destValue->Val->Integer = _fromInt;
			break;
		case TypeShort:
			_destValue->Val->ShortInteger = (short)_fromInt;
			break;
		case TypeChar:
			_destValue->Val->Character = (unsigned char)_fromInt;
			break;
		case TypeLong:
			_destValue->Val->LongInteger = (long)_fromInt;
			break;
		case TypeUnsignedInt:
			_destValue->Val->UnsignedInteger = (unsigned int)_fromInt;
			break;
		case TypeUnsignedShort:
			_destValue->Val->UnsignedShortInteger = (unsigned short)_fromInt;
			break;
		case TypeUnsignedLong:
			_destValue->Val->UnsignedLongInteger = (unsigned long)_fromInt;
			break;
		default:
			break;
	}
	return Result;
}

/* assign a floating point value */
double ExpressionAssignFP(struct ParseState *_parser, struct Value *_destValue, double _fromFP) {
	if (!_destValue->IsLValue) {
		ProgramFail(_parser, "can't assign to this"); 
	}
	_destValue->Val->FP = _fromFP;
	return _fromFP;
}

/* push a node on to the expression stack */
void ExpressionStackPushValueNode(struct ParseState *_parser, struct ExpressionStack **_stackTop, struct Value *_valueLoc) {
	struct ExpressionStack *StackNode = VariableAlloc(_parser, sizeof(struct ExpressionStack), FALSE);
	StackNode->Next = *_stackTop;
	StackNode->Val = _valueLoc;
	*_stackTop = StackNode;
#ifdef FANCY_ERROR_MESSAGES
	StackNode->Line = _parser->Line;
	StackNode->CharacterPos = _parser->CharacterPos;
#endif
#ifdef DEBUG_EXPRESSIONS
	ExpressionStackShow(*_stackTop);
#endif
}

/* push a blank value on to the expression stack by type */
struct Value *ExpressionStackPushValueByType(struct ParseState *_parser,
                                             struct ExpressionStack **_stackTop,
                                             struct ValueType *_pushType) {
	struct Value *valueLoc = VariableAllocValueFromType(_parser, _pushType, FALSE, NULL, FALSE);
	ExpressionStackPushValueNode(_parser, _stackTop, valueLoc);
	return valueLoc;
}

/* push a value on to the expression stack */
void ExpressionStackPushValue(struct ParseState *_parser,
                              struct ExpressionStack **_stackTop,
                              struct Value *_pushValue) {
	struct Value *valueLoc = VariableAllocValueAndCopy(_parser, _pushValue, FALSE);
	ExpressionStackPushValueNode(_parser, _stackTop, valueLoc);
}

void ExpressionStackPushLValue(struct ParseState *_parser,
                               struct ExpressionStack **_stackTop,
                               struct Value *_pushValue,
                               int Offset) {
	struct Value *valueLoc = VariableAllocValueShared(_parser, _pushValue);
	valueLoc->Val = (void *)((char *)valueLoc->Val + Offset);
	ExpressionStackPushValueNode(_parser, _stackTop, valueLoc);
}

void ExpressionStackPushDereference(struct ParseState *_parser,
                                    struct ExpressionStack **_stackTop,
                                    struct Value *_dereferenceValue) {
	struct Value *DerefVal;
	struct Value *valueLoc;
	int Offset;
	struct ValueType *DerefType;
	int DerefIsLValue;
	void *DerefDataLoc = VariableDereferencePointer(_parser, _dereferenceValue, &DerefVal, &Offset, &DerefType, &DerefIsLValue);
	if (DerefDataLoc == NULL) {
		ProgramFail(_parser, "NULL pointer dereference");
	}
	valueLoc = VariableAllocValueFromExistingData(_parser, DerefType, (union AnyValue *)DerefDataLoc, DerefIsLValue, DerefVal);
	ExpressionStackPushValueNode(_parser, _stackTop, valueLoc);
}

void ExpressionPushInt(struct ParseState *_parser,
                       struct ExpressionStack **_stackTop,
                       long _intValue) {
	struct Value *valueLoc = VariableAllocValueFromType(_parser, &IntType, FALSE, NULL, FALSE);
	valueLoc->Val->Integer = _intValue;
	ExpressionStackPushValueNode(_parser, _stackTop, valueLoc);
}

void ExpressionPushFP(struct ParseState *_parser,
                      struct ExpressionStack **_stackTop,
                      double _fPValue) {
	struct Value *valueLoc = VariableAllocValueFromType(_parser, &FPType, FALSE, NULL, FALSE);
	valueLoc->Val->FP = _fPValue;
	ExpressionStackPushValueNode(_parser, _stackTop, valueLoc);
}

/* assign to a pointer */
void ExpressionAssignToPointer(struct ParseState *_parser,
                               struct Value *_toValue,
                               struct Value *_fromValue,
                               const char *_funcName,
                               int _paramNo,
                               int _allowPointerCoercion) {
	struct ValueType *PointedToType = _toValue->Typ->FromType;
	if (    _fromValue->Typ == _toValue->Typ
	     || _fromValue->Typ == VoidPtrType
	     || (    _toValue->Typ == VoidPtrType
	          && _fromValue->Typ->Base == TypePointer) ) {
		// plain old pointer assignment
		_toValue->Val->Pointer = _fromValue->Val->Pointer;
	} else if (    _fromValue->Typ->Base == TypeArray
	            && (    PointedToType == _fromValue->Typ->FromType
	                 || _toValue->Typ == VoidPtrType) ) {
		// the form is: blah *x = array of blah
		_toValue->Val->Pointer = (void *)&_fromValue->Val->ArrayMem[0];
	} else if (    _fromValue->Typ->Base == TypePointer
	            && _fromValue->Typ->FromType->Base == TypeArray
	            && (    PointedToType == _fromValue->Typ->FromType->FromType
	                 || _toValue->Typ == VoidPtrType) ) {
		// the form is: blah *x = pointer to array of blah
		_toValue->Val->Pointer = VariableDereferencePointer(_parser, _fromValue, NULL, NULL, NULL, NULL);
	} else if (    IS_NUMERIC_COERCIBLE(_fromValue)
	            && ExpressionCoerceInteger(_fromValue) == 0){
		// null pointer assignment
		_toValue->Val->Pointer = NULL;
	} else if (_allowPointerCoercion && IS_NUMERIC_COERCIBLE(_fromValue)) {
		// assign integer to native pointer
		_toValue->Val->Pointer = (void *)(unsigned long)ExpressionCoerceUnsignedInteger(_fromValue);
	} else if (    _allowPointerCoercion
	            && _fromValue->Typ->Base == TypePointer) {
		/* assign a pointer to a pointer to a different type */
		_toValue->Val->Pointer = _fromValue->Val->Pointer;
	} else {
		AssignFail(_parser, "%t from %t", _toValue->Typ, _fromValue->Typ, 0, 0, _funcName, _paramNo); 
	}
}

/* assign any kind of value */
void ExpressionAssign(struct ParseState *_parser,
                      struct Value *_destValue,
                      struct Value *_sourceValue,
                      int _force,
                      const char *_funcName,
                      int _paramNo,
                      int _allowPointerCoercion) {
	if (    !_destValue->IsLValue
	     && !_force) {
		AssignFail(_parser, "not an lvalue", NULL, NULL, 0, 0, _funcName, _paramNo); 
	}
	if (    IS_NUMERIC_COERCIBLE(_destValue)
	     && !IS_NUMERIC_COERCIBLE_PLUS_POINTERS(_sourceValue, _allowPointerCoercion)) {
		AssignFail(_parser, "%t from %t", _destValue->Typ, _sourceValue->Typ, 0, 0, _funcName, _paramNo); 
	}
	switch (_destValue->Typ->Base) {
		case TypeInt:
			_destValue->Val->Integer = ExpressionCoerceInteger(_sourceValue);
			break;
		case TypeShort:
			_destValue->Val->ShortInteger = ExpressionCoerceInteger(_sourceValue);
			break;
		case TypeChar:
			_destValue->Val->Character = ExpressionCoerceUnsignedInteger(_sourceValue);
			break;
		case TypeLong:
			_destValue->Val->LongInteger = ExpressionCoerceInteger(_sourceValue);
			break;
		case TypeUnsignedInt:
			_destValue->Val->UnsignedInteger = ExpressionCoerceUnsignedInteger(_sourceValue);
			break;
		case TypeUnsignedShort:
			_destValue->Val->UnsignedShortInteger = ExpressionCoerceUnsignedInteger(_sourceValue);
			break;
		case TypeUnsignedLong:
			_destValue->Val->UnsignedLongInteger = ExpressionCoerceUnsignedInteger(_sourceValue);
			break;
		case TypeFP:
			if (!IS_NUMERIC_COERCIBLE_PLUS_POINTERS(_sourceValue, _allowPointerCoercion)) {
				AssignFail(_parser, "%t from %t", _destValue->Typ, _sourceValue->Typ, 0, 0, _funcName, _paramNo); 
			}
			_destValue->Val->FP = ExpressionCoerceFP(_sourceValue);
			break;
		case TypePointer:
			ExpressionAssignToPointer(_parser, _destValue, _sourceValue, _funcName, _paramNo, _allowPointerCoercion);
			break;
		case TypeArray:
			if (_destValue->Typ != _sourceValue->Typ) {
				AssignFail(_parser, "%t from %t", _destValue->Typ, _sourceValue->Typ, 0, 0, _funcName, _paramNo); 
			}
			if (_destValue->Typ->ArraySize != _sourceValue->Typ->ArraySize) {
				AssignFail(_parser, "from an array of size %d to one of size %d", NULL, NULL, _destValue->Typ->ArraySize, _sourceValue->Typ->ArraySize, _funcName, _paramNo);
			}
			memcpy((void *)_destValue->Val, (void *)_sourceValue->Val, TypeSizeValue(_destValue, FALSE));
			break;
		case TypeStruct:
		case TypeUnion:
			if (_destValue->Typ != _sourceValue->Typ) {
				AssignFail(_parser, "%t from %t", _destValue->Typ, _sourceValue->Typ, 0, 0, _funcName, _paramNo); 
			}
			memcpy((void *)_destValue->Val, (void *)_sourceValue->Val, TypeSizeValue(_sourceValue, FALSE));
			break;
		default:
			AssignFail(_parser, "%t", _destValue->Typ, NULL, 0, 0, _funcName, _paramNo); 
			break;
	}
}

/* evaluate the first half of a ternary operator x ? y : z */
void ExpressionQuestionMarkOperator(struct ParseState *_parser,
                                    struct ExpressionStack **_stackTop,
                                    struct Value *_bottomValue,
                                    struct Value *_topValue) {
	if (!IS_NUMERIC_COERCIBLE(_topValue)) {
		ProgramFail(_parser, "first argument to '?' should be a number");
	}
	if (ExpressionCoerceInteger(_topValue)) {
		/* the condition's true, return the _bottomValue */
		ExpressionStackPushValue(_parser, _stackTop, _bottomValue);
	} else {
		/* the condition's false, return void */
		ExpressionStackPushValueByType(_parser, _stackTop, &VoidType);
	}
}

/* evaluate the second half of a ternary operator x ? y : z */
void ExpressionColonOperator(struct ParseState *_parser,
                             struct ExpressionStack **_stackTop,
                             struct Value *_bottomValue,
                             struct Value *_topValue) {
	if (_topValue->Typ->Base == TypeVoid) {
		/* invoke the "else" part - return the _bottomValue */
		ExpressionStackPushValue(_parser, _stackTop, _bottomValue);
	} else {
		/* it was a "then" - return the _topValue */
		ExpressionStackPushValue(_parser, _stackTop, _topValue);
	}
}

/* evaluate a prefix operator */
void ExpressionPrefixOperator(struct ParseState *_parser,
                              struct ExpressionStack **_stackTop,
                              enum LexToken _op,
                              struct Value *_topValue) {
	struct Value *Result;
	union AnyValue *ValPtr;
	debugf("ExpressionPrefixOperator()\n");
	switch (_op) {
		case TokenAmpersand:
			if (!_topValue->IsLValue) {
				ProgramFail(_parser, "can't get the address of this");
			}
			ValPtr = _topValue->Val;
			Result = VariableAllocValueFromType(_parser, TypeGetMatching(_parser, _topValue->Typ, TypePointer, 0, StrEmpty, TRUE), FALSE, NULL, FALSE);
			Result->Val->Pointer = (void *)ValPtr;
			ExpressionStackPushValueNode(_parser, _stackTop, Result);
			break;
		case TokenAsterisk:
			ExpressionStackPushDereference(_parser, _stackTop, _topValue);
			break;
		case TokenSizeof:
			/* return the size of the argument */
			if (_topValue->Typ == &TypeType) {
				ExpressionPushInt(_parser, _stackTop, TypeSize(_topValue->Val->Typ, _topValue->Val->Typ->ArraySize, TRUE));
			} else {
				ExpressionPushInt(_parser, _stackTop, TypeSize(_topValue->Typ, _topValue->Typ->ArraySize, TRUE));
			}
			break;
		default:
			/* an arithmetic operator */
			if (_topValue->Typ == &FPType) {
				/* floating point prefix arithmetic */
				double ResultFP = 0.0;
				switch (_op) {
					case TokenPlus:
						ResultFP = _topValue->Val->FP;
						break;
					case TokenMinus:
						ResultFP = -_topValue->Val->FP;
						break;
					default:
						ProgramFail(_parser, "invalid operation");
						break;
				}
				ExpressionPushFP(_parser, _stackTop, ResultFP);
			} else if (IS_NUMERIC_COERCIBLE(_topValue)) {
				/* integer prefix arithmetic */
				long ResultInt = 0;
				long TopInt = ExpressionCoerceInteger(_topValue);
				switch (_op) {
					case TokenPlus:
						ResultInt = TopInt;
						break;
					case TokenMinus:
						ResultInt = -TopInt;
						break;
					case TokenIncrement:
						ResultInt = ExpressionAssignInt(_parser, _topValue, TopInt+1, FALSE);
						break;
					case TokenDecrement:
						ResultInt = ExpressionAssignInt(_parser, _topValue, TopInt-1, FALSE);
						break;
					case TokenUnaryNot:
						ResultInt = !TopInt;
						break;
					case TokenUnaryExor:
						ResultInt = ~TopInt;
						break;
					default:
						ProgramFail(_parser, "invalid operation");
						break;
				}
				ExpressionPushInt(_parser, _stackTop, ResultInt);
			} else if (_topValue->Typ->Base == TypePointer) {
				/* pointer prefix arithmetic */
				int Size = TypeSize(_topValue->Typ->FromType, 0, TRUE);
				struct Value *StackValue;
				void *ResultPtr;
				if (_topValue->Val->Pointer == NULL) {
					ProgramFail(_parser, "invalid use of a NULL pointer");
				}
				if (!_topValue->IsLValue) {
					ProgramFail(_parser, "can't assign to this"); 
				}
				switch (_op) {
					case TokenIncrement:
						_topValue->Val->Pointer = (void *)((char *)_topValue->Val->Pointer + Size);
						break;
					case TokenDecrement:
						_topValue->Val->Pointer = (void *)((char *)_topValue->Val->Pointer - Size);
						break;
					default:
						ProgramFail(_parser, "invalid operation");
						break;
				}
				ResultPtr = _topValue->Val->Pointer;
				StackValue = ExpressionStackPushValueByType(_parser, _stackTop, _topValue->Typ);
				StackValue->Val->Pointer = ResultPtr;
			} else {
				ProgramFail(_parser, "invalid operation");
			}
			break;
	}
}

/* evaluate a postfix operator */
void ExpressionPostfixOperator(struct ParseState *_parser,
                               struct ExpressionStack **_stackTop,
                               enum LexToken _op,
                               struct Value *_topValue) {
	debugf("ExpressionPostfixOperator()\n");
	if (IS_NUMERIC_COERCIBLE(_topValue)) {
		long ResultInt = 0;
		long TopInt = ExpressionCoerceInteger(_topValue);
		switch (_op) {
			case TokenIncrement:
				ResultInt = ExpressionAssignInt(_parser, _topValue, TopInt+1, TRUE);
				break;
			case TokenDecrement:
				ResultInt = ExpressionAssignInt(_parser, _topValue, TopInt-1, TRUE);
				break;
			case TokenRightSquareBracket:
				ProgramFail(_parser, "not supported");
				// TODO : ...
				break;
			case TokenCloseBracket:
				ProgramFail(_parser, "not supported");
				// TODO : ...
				break;
			default:
				ProgramFail(_parser, "invalid operation");
				break;
		}
		ExpressionPushInt(_parser, _stackTop, ResultInt);
	} else if (_topValue->Typ->Base == TypePointer) {
		/* pointer postfix arithmetic */
		int Size = TypeSize(_topValue->Typ->FromType, 0, TRUE);
		struct Value *StackValue;
		void *OrigPointer = _topValue->Val->Pointer;
		if (_topValue->Val->Pointer == NULL) {
			ProgramFail(_parser, "invalid use of a NULL pointer");
		}
		if (!_topValue->IsLValue) {
			ProgramFail(_parser, "can't assign to this"); 
		}
		switch (_op) {
			case TokenIncrement:
				_topValue->Val->Pointer = (void *)((char *)_topValue->Val->Pointer + Size);
				break;
			case TokenDecrement:
				_topValue->Val->Pointer = (void *)((char *)_topValue->Val->Pointer - Size);
				break;
			default:
				ProgramFail(_parser, "invalid operation");
				break;
		}
		StackValue = ExpressionStackPushValueByType(_parser, _stackTop, _topValue->Typ);
		StackValue->Val->Pointer = OrigPointer;
	} else {
		ProgramFail(_parser, "invalid operation");
	}
}

/* evaluate an infix operator */
void ExpressionInfixOperator(struct ParseState *_parser,
                             struct ExpressionStack **_stackTop,
                             enum LexToken _op,
                             struct Value *_bottomValue,
                             struct Value *_topValue) {
	long ResultInt = 0;
	struct Value *StackValue;
	void *Pointer;
	debugf("ExpressionInfixOperator()\n");
	if (    _bottomValue == NULL
	     || _topValue == NULL) {
		ProgramFail(_parser, "invalid expression");
	}
	if (_op == TokenLeftSquareBracket) {
		/* array index */
		int ArrayIndex;
		struct Value *Result = NULL;
		if (!IS_NUMERIC_COERCIBLE(_topValue)) {
			ProgramFail(_parser, "array index must be an integer");
		}
		ArrayIndex = ExpressionCoerceInteger(_topValue);
		/* make the array element result */
		switch (_bottomValue->Typ->Base) {
			case TypeArray:
				Result = VariableAllocValueFromExistingData(_parser, _bottomValue->Typ->FromType, (union AnyValue *)(&_bottomValue->Val->ArrayMem[0] + TypeSize(_bottomValue->Typ, ArrayIndex, TRUE)), _bottomValue->IsLValue, _bottomValue->LValueFrom);
				break;
			case TypePointer:
				Result = VariableAllocValueFromExistingData(_parser, _bottomValue->Typ->FromType, (union AnyValue *)((char *)_bottomValue->Val->Pointer + TypeSize(_bottomValue->Typ->FromType, 0, TRUE) * ArrayIndex), _bottomValue->IsLValue, _bottomValue->LValueFrom);
				break;
			default:
				ProgramFail(_parser, "this %t is not an array", _bottomValue->Typ);
		}
		ExpressionStackPushValueNode(_parser, _stackTop, Result);
	} else if (_op == TokenQuestionMark) {
		ExpressionQuestionMarkOperator(_parser, _stackTop, _topValue, _bottomValue);
	} else if (_op == TokenColon) {
		ExpressionColonOperator(_parser, _stackTop, _topValue, _bottomValue);
	} else if (    (_topValue->Typ == &FPType && _bottomValue->Typ == &FPType)
	            || (_topValue->Typ == &FPType && IS_NUMERIC_COERCIBLE(_bottomValue))
	            || (IS_NUMERIC_COERCIBLE(_topValue) && _bottomValue->Typ == &FPType) ) {
		/* floating point infix arithmetic */
		int ResultIsInt = FALSE;
		double ResultFP = 0.0;
		double TopFP = (_topValue->Typ == &FPType) ? _topValue->Val->FP : (double)ExpressionCoerceInteger(_topValue);
		double BottomFP = (_bottomValue->Typ == &FPType) ? _bottomValue->Val->FP : (double)ExpressionCoerceInteger(_bottomValue);
		switch (_op) {
			case TokenAssign:
				ResultFP = ExpressionAssignFP(_parser, _bottomValue, TopFP);
				break;
			case TokenAddAssign:
				ResultFP = ExpressionAssignFP(_parser, _bottomValue, BottomFP + TopFP);
				break;
			case TokenSubtractAssign:
				ResultFP = ExpressionAssignFP(_parser, _bottomValue, BottomFP - TopFP);
				break;
			case TokenMultiplyAssign:
				ResultFP = ExpressionAssignFP(_parser, _bottomValue, BottomFP * TopFP);
				break;
			case TokenDivideAssign:
				ResultFP = ExpressionAssignFP(_parser, _bottomValue, BottomFP / TopFP);
				break;
			case TokenEqual:
				ResultInt = BottomFP == TopFP;
				ResultIsInt = TRUE;
				break;
			case TokenNotEqual:
				ResultInt = BottomFP != TopFP;
				ResultIsInt = TRUE;
				break;
			case TokenLessThan:
				ResultInt = BottomFP < TopFP;
				ResultIsInt = TRUE;
				break;
			case TokenGreaterThan:
				ResultInt = BottomFP > TopFP;
				ResultIsInt = TRUE;
				break;
			case TokenLessEqual:
				ResultInt = BottomFP <= TopFP;
				ResultIsInt = TRUE;
				break;
			case TokenGreaterEqual:
				ResultInt = BottomFP >= TopFP;
				ResultIsInt = TRUE;
				break;
			case TokenPlus:
				ResultFP = BottomFP + TopFP;
				break;
			case TokenMinus:
				ResultFP = BottomFP - TopFP;
				break;
			case TokenAsterisk:
				ResultFP = BottomFP * TopFP;
				break;
			case TokenSlash:
				ResultFP = BottomFP / TopFP;
				break;
			default:
				ProgramFail(_parser, "invalid operation");
				break;
		}
		if (ResultIsInt) {
			ExpressionPushInt(_parser, _stackTop, ResultInt);
		} else {
			ExpressionPushFP(_parser, _stackTop, ResultFP);
		}
	} else if (IS_NUMERIC_COERCIBLE(_topValue) && IS_NUMERIC_COERCIBLE(_bottomValue)) {
		/* integer operation */
		long TopInt = ExpressionCoerceInteger(_topValue);
		long BottomInt = ExpressionCoerceInteger(_bottomValue);
		switch (_op) {
			case TokenAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, TopInt, FALSE);
				break;
			case TokenAddAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt + TopInt, FALSE);
				break;
			case TokenSubtractAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt - TopInt, FALSE);
				break;
			case TokenMultiplyAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt * TopInt, FALSE);
				break;
			case TokenDivideAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt / TopInt, FALSE);
				break;
#ifndef NO_MODULUS
			case TokenModulusAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt % TopInt, FALSE);
				break;
#endif
			case TokenShiftLeftAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt << TopInt, FALSE);
				break;
			case TokenShiftRightAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt >> TopInt, FALSE);
				break;
			case TokenArithmeticAndAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt & TopInt, FALSE);
				break;
			case TokenArithmeticOrAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt | TopInt, FALSE);
				break;
			case TokenArithmeticExorAssign:
				ResultInt = ExpressionAssignInt(_parser, _bottomValue, BottomInt ^ TopInt, FALSE);
				break;
			case TokenLogicalOr:
				ResultInt = BottomInt || TopInt;
				break;
			case TokenLogicalAnd:
				ResultInt = BottomInt && TopInt;
				break;
			case TokenArithmeticOr:
				ResultInt = BottomInt | TopInt;
				break;
			case TokenArithmeticExor:
				ResultInt = BottomInt ^ TopInt;
				break;
			case TokenAmpersand:
				ResultInt = BottomInt & TopInt;
				break;
			case TokenEqual:
				ResultInt = BottomInt == TopInt;
				break;
			case TokenNotEqual:
				ResultInt = BottomInt != TopInt;
				break;
			case TokenLessThan:
				ResultInt = BottomInt < TopInt;
				break;
			case TokenGreaterThan:
				ResultInt = BottomInt > TopInt;
				break;
			case TokenLessEqual:
				ResultInt = BottomInt <= TopInt;
				break;
			case TokenGreaterEqual:
				ResultInt = BottomInt >= TopInt;
				break;
			case TokenShiftLeft:
				ResultInt = BottomInt << TopInt;
				break;
			case TokenShiftRight:
				ResultInt = BottomInt >> TopInt;
				break;
			case TokenPlus:
				ResultInt = BottomInt + TopInt;
				break;
			case TokenMinus:
				ResultInt = BottomInt - TopInt;
				break;
			case TokenAsterisk:
				ResultInt = BottomInt * TopInt;
				break;
			case TokenSlash:
				ResultInt = BottomInt / TopInt;
				break;
#ifndef NO_MODULUS
			case TokenModulus:
				ResultInt = BottomInt % TopInt;
				break;
#endif
			default:
				ProgramFail(_parser, "invalid operation");
				break;
		}
		ExpressionPushInt(_parser, _stackTop, ResultInt);
	} else if (_bottomValue->Typ->Base == TypePointer && IS_NUMERIC_COERCIBLE(_topValue)) {
		/* pointer/integer infix arithmetic */
		long TopInt = ExpressionCoerceInteger(_topValue);
		if (    _op == TokenEqual
		     || _op == TokenNotEqual) {
			/* comparison to a NULL pointer */
			if (TopInt != 0) {
				ProgramFail(_parser, "invalid operation");
			}
			if (_op == TokenEqual) {
				ExpressionPushInt(_parser, _stackTop, _bottomValue->Val->Pointer == NULL);
			} else {
				ExpressionPushInt(_parser, _stackTop, _bottomValue->Val->Pointer != NULL);
			}
		} else if (    _op == TokenPlus
		            || _op == TokenMinus) {
			/* pointer arithmetic */
			int Size = TypeSize(_bottomValue->Typ->FromType, 0, TRUE);
			Pointer = _bottomValue->Val->Pointer;
			if (Pointer == NULL) {
				ProgramFail(_parser, "invalid use of a NULL pointer");
			}
			if (_op == TokenPlus) {
				Pointer = (void *)((char *)Pointer + TopInt * Size);
			} else {
				Pointer = (void *)((char *)Pointer - TopInt * Size);
			}
			StackValue = ExpressionStackPushValueByType(_parser, _stackTop, _bottomValue->Typ);
			StackValue->Val->Pointer = Pointer;
		} else if (    _op == TokenAssign
		            && TopInt == 0) {
			/* assign a NULL pointer */
			HeapUnpopStack(sizeof(struct Value));
			ExpressionAssign(_parser, _bottomValue, _topValue, FALSE, NULL, 0, FALSE);
			ExpressionStackPushValueNode(_parser, _stackTop, _bottomValue);
		} else if (    _op == TokenAddAssign
		            || _op == TokenSubtractAssign) {
			/* pointer arithmetic */
			int Size = TypeSize(_bottomValue->Typ->FromType, 0, TRUE);
			Pointer = _bottomValue->Val->Pointer;
			if (Pointer == NULL) {
				ProgramFail(_parser, "invalid use of a NULL pointer");
			}
			if (_op == TokenAddAssign) {
				Pointer = (void *)((char *)Pointer + TopInt * Size);
			} else {
				Pointer = (void *)((char *)Pointer - TopInt * Size);
			}
			HeapUnpopStack(sizeof(struct Value));
			_bottomValue->Val->Pointer = Pointer;
			ExpressionStackPushValueNode(_parser, _stackTop, _bottomValue);
		} else {
			ProgramFail(_parser, "invalid operation");
		}
	} else if (    _bottomValue->Typ->Base == TypePointer
	            && _topValue->Typ->Base == TypePointer
	            && _op != TokenAssign) {
		/* pointer/pointer operations */
		char *TopLoc = (char *)_topValue->Val->Pointer;
		char *BottomLoc = (char *)_bottomValue->Val->Pointer;
		switch (_op) {
			case TokenEqual:
				ExpressionPushInt(_parser, _stackTop, BottomLoc == TopLoc);
				break;
			case TokenNotEqual:
				ExpressionPushInt(_parser, _stackTop, BottomLoc != TopLoc);
				break;
			case TokenMinus:
				ExpressionPushInt(_parser, _stackTop, BottomLoc - TopLoc);
				break;
			default:
				ProgramFail(_parser, "invalid operation");
				break;
		}
	} else if (_op == TokenAssign) {
		/* assign a non-numeric type */
		HeapUnpopStack(sizeof(struct Value));   /* XXX - possible bug if lvalue is a temp value and takes more than sizeof(struct Value) */
		ExpressionAssign(_parser, _bottomValue, _topValue, FALSE, NULL, 0, FALSE);
		ExpressionStackPushValueNode(_parser, _stackTop, _bottomValue);
	} else if (_op == TokenCast) {
		/* cast a value to a different type */   /* XXX - possible bug if the destination type takes more than sizeof(struct Value) + sizeof(struct ValueType *) */
		struct Value *valueLoc = ExpressionStackPushValueByType(_parser, _stackTop, _bottomValue->Val->Typ);
		ExpressionAssign(_parser, valueLoc, _topValue, TRUE, NULL, 0, TRUE);
	} else {
		ProgramFail(_parser, "invalid operation");
	}
}

/* take the contents of the expression stack and compute the top until there's nothing greater than the given precedence */
void ExpressionStackCollapse(struct ParseState *_parser,
                             struct ExpressionStack **_stackTop,
                             int Precedence,
                             int *IgnorePrecedence) {
	int FoundPrecedence = Precedence;
	struct Value *topValue;
	struct Value *BottomValue;
	struct ExpressionStack *TopStackNode = *_stackTop;
	struct ExpressionStack *TopOperatorNode;
	debugf("ExpressionStackCollapse(%d):\n", Precedence);
#ifdef DEBUG_EXPRESSIONS
	ExpressionStackShow(*_stackTop);
#endif
	while (TopStackNode != NULL && TopStackNode->Next != NULL && FoundPrecedence >= Precedence) {
		/* find the top operator on the stack */
		if (TopStackNode->Order == OrderNone) {
			TopOperatorNode = TopStackNode->Next;
		} else {
			TopOperatorNode = TopStackNode;
		}
		FoundPrecedence = TopOperatorNode->Precedence;
		/* does it have a high enough precedence? */
		if (FoundPrecedence >= Precedence && TopOperatorNode != NULL) {
			/* execute this operator */
			switch (TopOperatorNode->Order) {
				case OrderPrefix:
					/* prefix evaluation */
					debugf("prefix evaluation\n");
					topValue = TopStackNode->Val;
					/* pop the value and then the prefix operator - assume they'll still be there until we're done */
					HeapPopStack(NULL, sizeof(struct ExpressionStack) + sizeof(struct Value) + TypeStackSizeValue(topValue));
					HeapPopStack(TopOperatorNode, sizeof(struct ExpressionStack));
					*_stackTop = TopOperatorNode->Next;
					/* do the prefix operation */
					if (    _parser->Mode == RunModeRun
					     && FoundPrecedence < *IgnorePrecedence) {
						/* run the operator */
						ExpressionPrefixOperator(_parser, _stackTop, TopOperatorNode->Op, topValue);
					} else {
						/* we're not running it so just return 0 */
						ExpressionPushInt(_parser, _stackTop, 0);
					}
					break;
				case OrderPostfix:
					/* postfix evaluation */
					debugf("postfix evaluation\n");
					topValue = TopStackNode->Next->Val;
					/* pop the postfix operator and then the value - assume they'll still be there until we're done */
					HeapPopStack(NULL, sizeof(struct ExpressionStack));
					HeapPopStack(topValue, sizeof(struct ExpressionStack) + sizeof(struct Value) + TypeStackSizeValue(topValue));
					*_stackTop = TopStackNode->Next->Next;
					/* do the postfix operation */
					if (    _parser->Mode == RunModeRun
					     && FoundPrecedence < *IgnorePrecedence) {
						/* run the operator */
						ExpressionPostfixOperator(_parser, _stackTop, TopOperatorNode->Op, topValue);
					} else {
						/* we're not running it so just return 0 */
						ExpressionPushInt(_parser, _stackTop, 0);
					}
					break;
				case OrderInfix:
					/* infix evaluation */
					debugf("infix evaluation\n");
					topValue = TopStackNode->Val;
					if (topValue != NULL) {
						BottomValue = TopOperatorNode->Next->Val;
						/* pop a value, the operator and another value - assume they'll still be there until we're done */
						HeapPopStack(NULL, sizeof(struct ExpressionStack) + sizeof(struct Value) + TypeStackSizeValue(topValue));
						HeapPopStack(NULL, sizeof(struct ExpressionStack));
						HeapPopStack(BottomValue, sizeof(struct ExpressionStack) + sizeof(struct Value) + TypeStackSizeValue(BottomValue));
						*_stackTop = TopOperatorNode->Next->Next;
						/* do the infix operation */
						if (_parser->Mode == RunModeRun && FoundPrecedence <= *IgnorePrecedence) {
							/* run the operator */
							ExpressionInfixOperator(_parser, _stackTop, TopOperatorNode->Op, BottomValue, topValue);
						} else {
							/* we're not running it so just return 0 */
							ExpressionPushInt(_parser, _stackTop, 0);
						}
					} else {
						FoundPrecedence = -1;
					}
					break;
				case OrderNone:
					/* this should never happen */
					assert(TopOperatorNode->Order != OrderNone);
					break;
			}
			/* if we've returned above the ignored precedence level turn ignoring off */
			if (FoundPrecedence <= *IgnorePrecedence) {
				*IgnorePrecedence = DEEP_PRECEDENCE;
			}
		}
#ifdef DEBUG_EXPRESSIONS
		ExpressionStackShow(*_stackTop);
#endif
		TopStackNode = *_stackTop;
	}
	debugf("ExpressionStackCollapse() finished\n");
#ifdef DEBUG_EXPRESSIONS
	ExpressionStackShow(*_stackTop);
#endif
}

/* push an operator on to the expression stack */
void ExpressionStackPushOperator(struct ParseState *_parser, struct ExpressionStack **_stackTop, enum OperatorOrder Order, enum LexToken Token, int Precedence) {
	struct ExpressionStack *StackNode = VariableAlloc(_parser, sizeof(struct ExpressionStack), FALSE);
	StackNode->Next = *_stackTop;
	StackNode->Order = Order;
	StackNode->Op = Token;
	StackNode->Precedence = Precedence;
	*_stackTop = StackNode;
	debugf("ExpressionStackPushOperator()\n");
#ifdef FANCY_ERROR_MESSAGES
	StackNode->Line = _parser->Line;
	StackNode->CharacterPos = _parser->CharacterPos;
#endif
#ifdef DEBUG_EXPRESSIONS
	ExpressionStackShow(*_stackTop);
#endif
}

/* do the '.' and '->' operators */
void ExpressionGetStructElement(struct ParseState *_parser, struct ExpressionStack **_stackTop, enum LexToken Token) {
	struct Value *Ident;
	/* get the identifier following the '.' or '->' */
	if (LexGetToken(_parser, &Ident, TRUE) != TokenIdentifier) {
		ProgramFail(_parser, "need an structure or union member after '%s'", (Token == TokenDot) ? "." : "->");
	}
	if (_parser->Mode == RunModeRun) {
		/* look up the struct element */
		struct Value *ParamVal = (*_stackTop)->Val;
		struct Value *StructVal = ParamVal;
		struct ValueType *StructType = ParamVal->Typ;
		char *DerefDataLoc = (char *)ParamVal->Val;
		struct Value *MemberValue;
		struct Value *Result;
		/* if we're doing '->' dereference the struct pointer first */
		if (Token == TokenArrow) {
			DerefDataLoc = VariableDereferencePointer(_parser, ParamVal, &StructVal, NULL, &StructType, NULL);
		}
		if (StructType->Base != TypeStruct && StructType->Base != TypeUnion) {
			ProgramFail(_parser, "can't use '%s' on something that's not a struct or union %s : it's a %t", (Token == TokenDot) ? "." : "->", (Token == TokenArrow) ? "pointer" : "", ParamVal->Typ);
		}
		if (!TableGet(StructType->Members, Ident->Val->Identifier, &MemberValue, NULL, NULL, NULL)) {
			ProgramFail(_parser, "doesn't have a member called '%s'", Ident->Val->Identifier);
		}
		/* pop the value - assume it'll still be there until we're done */
		HeapPopStack(ParamVal, sizeof(struct ExpressionStack) + sizeof(struct Value) + TypeStackSizeValue(StructVal));
		*_stackTop = (*_stackTop)->Next;
		/* make the result value for this member only */
		Result = VariableAllocValueFromExistingData(_parser, MemberValue->Typ, (void *)(DerefDataLoc + MemberValue->Val->Integer), TRUE, (StructVal != NULL) ? StructVal->LValueFrom : NULL);
		ExpressionStackPushValueNode(_parser, _stackTop, Result);
	}
}

/* parse an expression with operator precedence */
int ExpressionParse(struct ParseState *_parser, struct Value **Result) {
	struct Value *LexValue;
	int PrefixState = TRUE;
	int Done = FALSE;
	int BracketPrecedence = 0;
	int LocalPrecedence;
	int Precedence = 0;
	int IgnorePrecedence = DEEP_PRECEDENCE;
	struct ExpressionStack *_stackTop = NULL;
	int TernaryDepth = 0;
	debugf("ExpressionParse():\n");
	do {
		struct ParseState PreState;
		enum LexToken Token;
		ParserCopy(&PreState, _parser);
		Token = LexGetToken(_parser, &LexValue, TRUE);
		if (    (    (    (int)Token > TokenComma
		               && (int)Token <= (int)TokenOpenBracket)
		          || (    Token == TokenCloseBracket
		               && BracketPrecedence != 0) )
		     && (    Token != TokenColon
		          || TernaryDepth > 0) ) {
			/* it's an operator with precedence */
			if (PrefixState) {
				/* expect a prefix operator */
				if (OperatorPrecedence[(int)Token].PrefixPrecedence == 0) {
					ProgramFail(_parser, "operator not expected here");
				}
				LocalPrecedence = OperatorPrecedence[(int)Token].PrefixPrecedence;
				Precedence = BracketPrecedence + LocalPrecedence;
				if (Token == TokenOpenBracket) {
					/* it's either a new bracket level or a cast */
					enum LexToken BracketToken = LexGetToken(_parser, &LexValue, FALSE);
					if (    IS_TYPE_TOKEN(BracketToken)
					     && (    _stackTop == NULL
					          || _stackTop->Op != TokenSizeof) ) {
						/* it's a cast - get the new type */
						struct ValueType *CastType;
						char *CastIdentifier;
						struct Value *CastTypeValue;
						TypeParse(_parser, &CastType, &CastIdentifier, NULL);
						if (LexGetToken(_parser, &LexValue, TRUE) != TokenCloseBracket) {
							ProgramFail(_parser, "brackets not closed");
						}
						// scan and collapse the stack to the precedence of this infix cast operator, then push
						Precedence = BracketPrecedence + OperatorPrecedence[(int)TokenCast].PrefixPrecedence;
						ExpressionStackCollapse(_parser, &_stackTop, Precedence+1, &IgnorePrecedence);
						CastTypeValue = VariableAllocValueFromType(_parser, &TypeType, FALSE, NULL, FALSE);
						CastTypeValue->Val->Typ = CastType;
						ExpressionStackPushValueNode(_parser, &_stackTop, CastTypeValue);
						ExpressionStackPushOperator(_parser, &_stackTop, OrderInfix, TokenCast, Precedence);
					} else {
						/* boost the bracket operator precedence */
						BracketPrecedence += BRACKET_PRECEDENCE;
					}
				} else {
					//scan and collapse the stack to the precedence of this operator, then push
					ExpressionStackCollapse(_parser, &_stackTop, Precedence, &IgnorePrecedence);
					ExpressionStackPushOperator(_parser, &_stackTop, OrderPrefix, Token, Precedence);
				}
			} else {
				// expect an infix or postfix operator
				if (OperatorPrecedence[(int)Token].PostfixPrecedence != 0) {
					switch (Token) {
						case TokenCloseBracket:
						case TokenRightSquareBracket:
							if (BracketPrecedence == 0) {
								// assume this bracket is after the end of the expression
								ParserCopy(_parser, &PreState);
								Done = TRUE;
							} else {
								// collapse to the bracket precedence
								ExpressionStackCollapse(_parser, &_stackTop, BracketPrecedence, &IgnorePrecedence);
								BracketPrecedence -= BRACKET_PRECEDENCE;
							}
							break;
						default:
							// scan and collapse the stack to the precedence of this operator, then push
							Precedence = BracketPrecedence + OperatorPrecedence[(int)Token].PostfixPrecedence;
							ExpressionStackCollapse(_parser, &_stackTop, Precedence, &IgnorePrecedence);
							ExpressionStackPushOperator(_parser, &_stackTop, OrderPostfix, Token, Precedence);
							break;
					}
				} else if (OperatorPrecedence[(int)Token].InfixPrecedence != 0) {
					// scan and collapse the stack, then push
					Precedence = BracketPrecedence + OperatorPrecedence[(int)Token].InfixPrecedence;
					// for right to left order, only go down to the next higher precedence so we evaluate it in reverse order
					// for left to right order, collapse down to this precedence so we evaluate it in forward order
					if (IS_LEFT_TO_RIGHT(OperatorPrecedence[(int)Token].InfixPrecedence)) {
						ExpressionStackCollapse(_parser, &_stackTop, Precedence, &IgnorePrecedence);
					} else {
						ExpressionStackCollapse(_parser, &_stackTop, Precedence+1, &IgnorePrecedence);
					}
					if (    Token == TokenDot
					     || Token == TokenArrow) {
						ExpressionGetStructElement(_parser, &_stackTop, Token); /* this operator is followed by a struct element so handle it as a special case */
					} else {
						/* if it's a && or || operator we may not need to evaluate the right hand side of the expression */
						if (    (    Token == TokenLogicalOr
						          || Token == TokenLogicalAnd)
						     && IS_NUMERIC_COERCIBLE(_stackTop->Val)) {
							long LHSInt = ExpressionCoerceInteger(_stackTop->Val);
							if (    (    (    Token == TokenLogicalOr
							               && LHSInt)
							          || (    Token == TokenLogicalAnd
							               && !LHSInt) )
							     && (IgnorePrecedence > Precedence) )
								IgnorePrecedence = Precedence;
						}
						// push the operator on the stack
						ExpressionStackPushOperator(_parser, &_stackTop, OrderInfix, Token, Precedence);
						PrefixState = TRUE;
						switch (Token) {
							case TokenQuestionMark:
								TernaryDepth++;
								break;
							case TokenColon:
								TernaryDepth--;
								break;
							default:
								break;
						}
					}
					/* treat an open square bracket as an infix array index operator followed by an open bracket */
					if (Token == TokenLeftSquareBracket) {
						/* boost the bracket operator precedence, then push */
						BracketPrecedence += BRACKET_PRECEDENCE;
					}
				} else {
					ProgramFail(_parser, "operator not expected here");
				}
			}
		} else if (Token == TokenIdentifier) {
			/* it's a variable, function or a macro */
			if (!PrefixState) {
				ProgramFail(_parser, "identifier not expected here");
			}
			if (LexGetToken(_parser, NULL, FALSE) == TokenOpenBracket) {
				ExpressionParseFunctionCall(_parser, &_stackTop, LexValue->Val->Identifier, _parser->Mode == RunModeRun && Precedence < IgnorePrecedence);
			} else {
				if (_parser->Mode == RunModeRun && Precedence < IgnorePrecedence) {
					struct Value *VariableValue = NULL;
					VariableGet(_parser, LexValue->Val->Identifier, &VariableValue);
					if (VariableValue->Typ->Base == TypeMacro) {
						/* evaluate a macro as a kind of simple subroutine */
						struct ParseState Macro_parser;
						struct Value *MacroResult;
						ParserCopy(&Macro_parser, &VariableValue->Val->MacroDef.Body);
						if (VariableValue->Val->MacroDef.NumParams != 0) {
							ProgramFail(&Macro_parser, "macro arguments missing");
						}
						if (    !ExpressionParse(&Macro_parser, &MacroResult)
						     || LexGetToken(&Macro_parser, NULL, FALSE) != TokenEndOfFunction) {
							ProgramFail(&Macro_parser, "expression expected");
						}
						ExpressionStackPushValueNode(_parser, &_stackTop, MacroResult);
					} else if (VariableValue->Typ == TypeVoid) {
						ProgramFail(_parser, "a void value isn't much use here");
					} else {
						ExpressionStackPushLValue(_parser, &_stackTop, VariableValue, 0); /* it's a value variable */
					}
				} else {
					// push a dummy value
					ExpressionPushInt(_parser, &_stackTop, 0);
				}
			}
			// if we've successfully ignored the RHS turn ignoring off
			if (Precedence <= IgnorePrecedence) {
				IgnorePrecedence = DEEP_PRECEDENCE;
			}
			PrefixState = FALSE;
		} else if (    (int)Token > TokenCloseBracket
		            && (int)Token <= TokenCharacterConstant) {
			/* it's a value of some sort, push it */
			if (!PrefixState) {
				ProgramFail(_parser, "value not expected here");
			}
			PrefixState = FALSE;
			ExpressionStackPushValue(_parser, &_stackTop, LexValue);
		} else if (IS_TYPE_TOKEN(Token)) {
			/* it's a type. push it on the stack like a value. this is used in sizeof() */
			struct ValueType *Typ;
			char *Identifier;
			struct Value *TypeValue;
			if (!PrefixState) {
				ProgramFail(_parser, "type not expected here");
			}
			PrefixState = FALSE;
			ParserCopy(_parser, &PreState);
			TypeParse(_parser, &Typ, &Identifier, NULL);
			TypeValue = VariableAllocValueFromType(_parser, &TypeType, FALSE, NULL, FALSE);
			TypeValue->Val->Typ = Typ;
			ExpressionStackPushValueNode(_parser, &_stackTop, TypeValue);
		} else {
			// it isn't a token from an expression
			ParserCopy(_parser, &PreState);
			Done = TRUE;
		}
	} while (!Done);
	// check that brackets have been closed
	if (BracketPrecedence > 0) {
		ProgramFail(_parser, "brackets not closed");
	}
	// scan and collapse the stack to precedence 0
	ExpressionStackCollapse(_parser, &_stackTop, 0, &IgnorePrecedence);
	// fix up the stack and return the result if we're in run mode
	if (_stackTop != NULL) {
		// all that should be left is a single value on the stack
		if (_parser->Mode == RunModeRun) {
			if (    _stackTop->Order != OrderNone
			     || _stackTop->Next != NULL) {
				ProgramFail(_parser, "invalid expression");
			}
			*Result = _stackTop->Val;
			HeapPopStack(_stackTop, sizeof(struct ExpressionStack));
		} else {
			HeapPopStack(_stackTop->Val, sizeof(struct ExpressionStack) + sizeof(struct Value) + TypeStackSizeValue(_stackTop->Val));
		}
	}
	debugf("ExpressionParse() done\n\n");
#ifdef DEBUG_EXPRESSIONS
	ExpressionStackShow(_stackTop);
#endif
	return _stackTop != NULL;
}


/* do a parameterised macro call */
void ExpressionParseMacroCall(struct ParseState *_parser, struct ExpressionStack **_stackTop, const char *MacroName, struct MacroDef *MDef) {
	struct Value *ReturnValue = NULL;
	struct Value *Param;
	struct Value **ParamArray = NULL;
	int ArgCount;
	enum LexToken Token;
	if (_parser->Mode == RunModeRun) {
		/* create a stack frame for this macro */
		ExpressionStackPushValueByType(_parser, _stackTop, &FPType);  /* largest return type there is */
		ReturnValue = (*_stackTop)->Val;
		HeapPushStackFrame();
		ParamArray = HeapAllocStack(sizeof(struct Value *) * MDef->NumParams);	
		if (ParamArray == NULL) {
			ProgramFail(_parser, "out of memory");
		}
	} else {
		ExpressionPushInt(_parser, _stackTop, 0);
	}
	// parse arguments
	ArgCount = 0;
	do {
		if (ExpressionParse(_parser, &Param)) {
			if (_parser->Mode == RunModeRun) {
				if (ArgCount < MDef->NumParams) {
					ParamArray[ArgCount] = Param;
				} else {
					ProgramFail(_parser, "too many arguments to %s()", MacroName);
				}
			}
			ArgCount++;
			Token = LexGetToken(_parser, NULL, TRUE);
			if (    Token != TokenComma
			     && Token != TokenCloseBracket) {
				ProgramFail(_parser, "comma expected");
			}
		} else {
			/* end of argument list? */
			Token = LexGetToken(_parser, NULL, TRUE);
			if (!TokenCloseBracket) {
				ProgramFail(_parser, "bad argument");
			}
		}
	} while (Token != TokenCloseBracket);
	
	if (_parser->Mode == RunModeRun) {
		/* evaluate the macro */
		struct ParseState Macro_parser;
		int Count;
		struct Value *EvalValue;
		if (ArgCount < MDef->NumParams) {
			ProgramFail(_parser, "not enough arguments to '%s'", MacroName);
		}
		if (MDef->Body.Pos == NULL) {
			ProgramFail(_parser, "'%s' is undefined", MacroName);
		}
		ParserCopy(&Macro_parser, &MDef->Body);
		VariableStackFrameAdd(_parser, MacroName, 0);
		TopStackFrame->NumParams = ArgCount;
		TopStackFrame->ReturnValue = ReturnValue;
		for (Count = 0; Count < MDef->NumParams; Count++) {
			VariableDefine(_parser, MDef->ParamName[Count], ParamArray[Count], NULL, TRUE);
		}
		ExpressionParse(&Macro_parser, &EvalValue);
		ExpressionAssign(_parser, ReturnValue, EvalValue, TRUE, MacroName, 0, FALSE);
		VariableStackFramePop(_parser);
		HeapPopStackFrame();
	}
}

/* do a function call */
void ExpressionParseFunctionCall(struct ParseState *_parser, struct ExpressionStack **_stackTop, const char *_funcName, int _runIt) {
	struct Value *ReturnValue = NULL;
	struct Value *FuncValue;
	struct Value *Param;
	struct Value **ParamArray = NULL;
	int ArgCount;
	enum LexToken Token = LexGetToken(_parser, NULL, TRUE);	/* open bracket */
	enum RunMode OldMode = _parser->Mode;
	if (_runIt) {
		/* get the function definition */
		VariableGet(_parser, _funcName, &FuncValue);
		if (FuncValue->Typ->Base == TypeMacro) {
			/* this is actually a macro, not a function */
			ExpressionParseMacroCall(_parser, _stackTop, _funcName, &FuncValue->Val->MacroDef);
			return;
		}
		if (FuncValue->Typ->Base != TypeFunction) {
			ProgramFail(_parser, "%t is not a function - can't call", FuncValue->Typ);
		}
		ExpressionStackPushValueByType(_parser, _stackTop, FuncValue->Val->FuncDef.ReturnType);
		ReturnValue = (*_stackTop)->Val;
		HeapPushStackFrame();
		ParamArray = HeapAllocStack(sizeof(struct Value *) * FuncValue->Val->FuncDef.NumParams);
		if (ParamArray == NULL) {
			ProgramFail(_parser, "out of memory");
		}
	} else {
		ExpressionPushInt(_parser, _stackTop, 0);
		_parser->Mode = RunModeSkip;
	}
	// parse arguments
	ArgCount = 0;
	do {
		if (    _runIt
		     && ArgCount < FuncValue->Val->FuncDef.NumParams) {
			ParamArray[ArgCount] = VariableAllocValueFromType(_parser, FuncValue->Val->FuncDef.ParamType[ArgCount], FALSE, NULL, FALSE);
		}
		if (ExpressionParse(_parser, &Param)) {
			if (_runIt) {
				if (ArgCount < FuncValue->Val->FuncDef.NumParams) {
					ExpressionAssign(_parser, ParamArray[ArgCount], Param, TRUE, _funcName, ArgCount+1, FALSE);
					VariableStackPop(_parser, Param);
				} else {
					if (!FuncValue->Val->FuncDef.VarArgs) {
						ProgramFail(_parser, "too many arguments to %s()", _funcName);
					}
				}
			}
			ArgCount++;
			Token = LexGetToken(_parser, NULL, TRUE);
			if (Token != TokenComma && Token != TokenCloseBracket) {
				ProgramFail(_parser, "comma expected");
			}
		} else {
			/* end of argument list? */
			Token = LexGetToken(_parser, NULL, TRUE);
			if (!TokenCloseBracket) {
				ProgramFail(_parser, "bad argument");
			}
		}
	} while (Token != TokenCloseBracket);
	if (_runIt) {
		/* run the function */
		if (ArgCount < FuncValue->Val->FuncDef.NumParams) {
			ProgramFail(_parser, "not enough arguments to '%s'", _funcName);
		}
		if (FuncValue->Val->FuncDef.Intrinsic == NULL) {
			/* run a user-defined function */
			struct ParseState FuncParser;
			int Count;
			if (FuncValue->Val->FuncDef.Body.Pos == NULL) {
				ProgramFail(_parser, "'%s' is undefined", _funcName);
			}
			ParserCopy(&FuncParser, &FuncValue->Val->FuncDef.Body);
			VariableStackFrameAdd(_parser, _funcName, FuncValue->Val->FuncDef.Intrinsic ? FuncValue->Val->FuncDef.NumParams : 0);
			TopStackFrame->NumParams = ArgCount;
			TopStackFrame->ReturnValue = ReturnValue;
			for (Count = 0; Count < FuncValue->Val->FuncDef.NumParams; ++Count) {
				VariableDefine(_parser, FuncValue->Val->FuncDef.ParamName[Count], ParamArray[Count], NULL, TRUE);
			}
			if (ParseStatement(&FuncParser, TRUE) != ParseResultOk) {
				ProgramFail(&FuncParser, "function body expected");
			}
			if (_runIt) {
				if (    FuncParser.Mode == RunModeRun
				     && FuncValue->Val->FuncDef.ReturnType != &VoidType) {
					ProgramFail(&FuncParser, "no value returned from a function returning %t", FuncValue->Val->FuncDef.ReturnType);
				} else if (FuncParser.Mode == RunModeGoto) {
					ProgramFail(&FuncParser, "couldn't find goto label '%s'", FuncParser.SearchGotoLabel);
				}
			}
			VariableStackFramePop(_parser);
		} else {
			FuncValue->Val->FuncDef.Intrinsic(_parser, ReturnValue, ParamArray, ArgCount);
		}
		HeapPopStackFrame();
	}
	_parser->Mode = OldMode;
}

/* parse an expression */
long ExpressionParseInt(struct ParseState *_parser) {
	struct Value *Val;
	long Result = 0;
	if (!ExpressionParse(_parser, &Val)) {
		ProgramFail(_parser, "expression expected");
	}
	if (_parser->Mode == RunModeRun) {
		if (!IS_NUMERIC_COERCIBLE(Val)) {
			ProgramFail(_parser, "integer value expected instead of %t", Val->Typ);
		}
		Result = ExpressionCoerceInteger(Val);
		VariableStackPop(_parser, Val);
	}
	return Result;
}

