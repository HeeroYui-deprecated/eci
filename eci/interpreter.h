/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_INTERPRETER_H__
#define __ECI_INTERPRETER_H__

#include "platform.h"


/* handy definitions */
#ifndef TRUE
#define TRUE 1
#define FALSE 0
#endif

#ifndef NULL
#define NULL 0
#endif

#ifndef min
#define min(x,y) (((x)<(y))?(x):(y))
#endif

#define MEM_ALIGN(x) (((x) + sizeof(ALIGN_TYPE) - 1) & ~(sizeof(ALIGN_TYPE)-1))

#define GETS_BUF_MAX 256

/* small processors use a simplified FILE * for stdio, otherwise use the system FILE * */
#ifdef BUILTIN_MINI_STDLIB
typedef struct OutputStream IOFILE;
#else
typedef FILE IOFILE;
#endif

/* coercion of numeric types to other numeric types */
#define IS_FP(v) ((v)->Typ->Base == TypeFP)
#define FP_VAL(v) ((v)->Val->FP)

#define IS_POINTER_COERCIBLE(v, ap) ((ap) ? ((v)->Typ->Base == TypePointer) : 0)
#define POINTER_COERCE(v) ((int)(v)->Val->Pointer)

#define IS_INTEGER_NUMERIC_TYPE(t) ((t)->Base >= TypeInt && (t)->Base <= TypeUnsignedLong)
#define IS_INTEGER_NUMERIC(v) IS_INTEGER_NUMERIC_TYPE((v)->Typ)
#define IS_NUMERIC_COERCIBLE(v) (IS_INTEGER_NUMERIC(v) || IS_FP(v))
#define IS_NUMERIC_COERCIBLE_PLUS_POINTERS(v,ap) (IS_NUMERIC_COERCIBLE(v) || IS_POINTER_COERCIBLE(v,ap))


struct Table;

/* lexical tokens */
enum LexToken {
	/* 0x00 */ TokenNone, 
	/* 0x01 */ TokenComma,
	/* 0x02 */ TokenAssign, TokenAddAssign, TokenSubtractAssign, TokenMultiplyAssign, TokenDivideAssign, TokenModulusAssign,
	/* 0x08 */ TokenShiftLeftAssign, TokenShiftRightAssign, TokenArithmeticAndAssign, TokenArithmeticOrAssign, TokenArithmeticExorAssign,
	/* 0x0d */ TokenQuestionMark, TokenColon, 
	/* 0x0f */ TokenLogicalOr, 
	/* 0x10 */ TokenLogicalAnd, 
	/* 0x11 */ TokenArithmeticOr, 
	/* 0x12 */ TokenArithmeticExor, 
	/* 0x13 */ TokenAmpersand, 
	/* 0x14 */ TokenEqual, TokenNotEqual, 
	/* 0x16 */ TokenLessThan, TokenGreaterThan, TokenLessEqual, TokenGreaterEqual,
	/* 0x1a */ TokenShiftLeft, TokenShiftRight, 
	/* 0x1c */ TokenPlus, TokenMinus, 
	/* 0x1e */ TokenAsterisk, TokenSlash, TokenModulus,
	/* 0x21 */ TokenIncrement, TokenDecrement, TokenUnaryNot, TokenUnaryExor, TokenSizeof, TokenCast,
	/* 0x27 */ TokenLeftSquareBracket, TokenRightSquareBracket, TokenDot, TokenArrow, 
	/* 0x2b */ TokenOpenBracket, TokenCloseBracket,
	/* 0x2d */ TokenIdentifier, TokenIntegerConstant, TokenFPConstant, TokenStringConstant, TokenCharacterConstant,
	/* 0x32 */ TokenSemicolon, TokenEllipsis,
	/* 0x34 */ TokenLeftBrace, TokenRightBrace,
	/* 0x36 */ TokenIntType, TokenCharType, TokenFloatType, TokenDoubleType, TokenVoidType, TokenEnumType,
	/* 0x3c */ TokenLongType, TokenSignedType, TokenShortType, TokenStaticType, TokenAutoType, TokenRegisterType, TokenExternType, TokenStructType, TokenUnionType, TokenUnsignedType, TokenTypedef,
	/* 0x46 */ TokenContinue, TokenDo, TokenElse, TokenFor, TokenGoto, TokenIf, TokenWhile, TokenBreak, TokenSwitch, TokenCase, TokenDefault, TokenReturn,
	/* 0x52 */ TokenHashDefine, TokenHashInclude, TokenHashIf, TokenHashIfdef, TokenHashIfndef, TokenHashElse, TokenHashEndif,
	/* 0x59 */ TokenNew, TokenDelete,
	/* 0x5b */ TokenOpenMacroBracket,
	/* 0x5c */ TokenEOF, TokenEndOfLine, TokenEndOfFunction
};

/* used in dynamic memory allocation */
struct AllocNode {
	unsigned int Size;
	struct AllocNode *NextFree;
};

/* whether we're running or skipping code */
enum RunMode {
	RunModeRun, //!< we're running code as we parse it 
	RunModeSkip, //!< skipping code, not running 
	RunModeReturn, //!< returning from a function 
	RunModeCaseSearch, //!< searching for a case label 
	RunModeBreak, //!< breaking out of a switch/while/do 
	RunModeContinue, //!< as above but repeat the loop 
	RunModeGoto //!< searching for a goto label 
};

/* parser state - has all this detail so we can parse nested files */
struct ParseState {
	const unsigned char *Pos;
	const char *FileName;
	short int Line;
	short int CharacterPos;
	enum RunMode Mode; //!< whether to skip or run code 
	int SearchLabel; //!< what case label we're searching for 
	const char *SearchGotoLabel; //!< what goto label we're searching for 
	short int HashIfLevel;
	short int HashIfEvaluateToLevel;
	const char *SourceText;
};

/* values */
enum BaseType {
	TypeVoid, //!< no type 
	TypeInt, //!< integer 
	TypeShort, //!< short integer 
	TypeChar, //!< a single character (unsigned) 
	TypeLong, //!< long integer 
	TypeUnsignedInt, //!< unsigned integer 
	TypeUnsignedShort, //!< unsigned short integer 
	TypeUnsignedLong, //!< unsigned long integer 
	TypeFP, //!< floating point 
	TypeFunction, //!< a function 
	TypeMacro, //!< a macro 
	TypePointer, //!< a pointer 
	TypeArray, //!< an array of a sub-type 
	TypeStruct, //!< aggregate type 
	TypeUnion, //!< merged type 
	TypeEnum, //!< enumerated integer type 
	TypeGotoLabel, //!< a label we can "goto" 
	Type_Type //!< a type for storing types 
};

/* data type */
struct ValueType {
	enum BaseType Base; //!< what kind of type this is 
	int ArraySize; //!< the size of an array type 
	int Sizeof; //!< the storage required 
	int AlignBytes; //!< the alignment boundary of this type 
	const char *Identifier; //!< the name of a struct or union 
	struct ValueType *FromType; //!< the type we're derived from (or NULL) 
	struct ValueType *DerivedTypeList;  //!< first in a list of types derived from this one 
	struct ValueType *Next; //!< next item in the derived type list 
	struct Table *Members; //!< members of a struct or union 
	int OnHeap; //!< true if allocated on the heap 
	int StaticQualifier; //!< true if it's a static 
};

/* function definition */
struct FuncDef {
	struct ValueType *ReturnType; //!< the return value type 
	int NumParams; //!< the number of parameters 
	int VarArgs; //!< has a variable number of arguments after the explicitly specified ones 
	struct ValueType **ParamType; //!< array of parameter types 
	char **ParamName; //!< array of parameter names 
	void (*Intrinsic)(); //!< intrinsic call address or NULL 
	struct ParseState Body; //!< lexical tokens of the function body if not intrinsic 
};

/* macro definition */
struct MacroDef {
	int NumParams; //!< the number of parameters 
	char **ParamName; //!< array of parameter names 
	struct ParseState Body; //!< lexical tokens of the function body if not intrinsic 
};

/* values */
union AnyValue {
	unsigned char Character;
	short ShortInteger;
	int Integer;
	long LongInteger;
	unsigned short UnsignedShortInteger;
	unsigned int UnsignedInteger;
	unsigned long UnsignedLongInteger;
	char *Identifier;
	char ArrayMem[2]; //!< placeholder for where the data starts, doesn't point to it 
	struct ValueType *Typ;
	struct FuncDef FuncDef;
	struct MacroDef MacroDef;
	double FP;
	void *Pointer; //!< unsafe native pointers 
};

struct Value
{
	struct ValueType *Typ; //!< the type of this value 
	union AnyValue *Val; //!< pointer to the AnyValue which holds the actual content 
	struct Value *LValueFrom; //!< if an LValue, this is a Value our LValue is contained within (or NULL) 
	char ValOnHeap; //!< the AnyValue is on the heap (but this Value is on the stack) 
	char ValOnStack; //!< the AnyValue is on the stack along with this Value 
	char IsLValue; //!< is modifiable and is allocated somewhere we can usefully modify it 
};

/* hash table data structure */
struct TableEntry {
	struct TableEntry *Next; //!< next item in this hash chain 
	const char *DeclFileName; //!< where the variable was declared 
	unsigned short DeclLine;
	unsigned short DeclColumn;
	union TableEntryPayload {
		struct ValueEntry {
			char *Key; //!< points to the shared string table 
			struct Value *Val; //!< the value we're storing 
		} v; //!< used for tables of values 
		char Key[1]; //!< dummy size - used for the shared string table 
	} p;
};

struct Table {
	short Size;
	short OnHeap;
	struct TableEntry **HashTable;
};

/* stack frame for function calls */
struct StackFrame {
	struct ParseState ReturnParser; //!< how we got here 
	const char *FuncName; //!< the name of the function we're in 
	struct Value *ReturnValue; //!< copy the return value here 
	struct Value **Parameter; //!< array of parameter values 
	int NumParams; //!< the number of parameters 
	struct Table LocalTable; //!< the local variables and parameters 
	struct TableEntry *LocalHashTable[LOCAL_TABLE_SIZE];
	struct StackFrame *PreviousStackFrame;  //!< the next lower stack frame 
};

/* lexer state */
enum LexMode {
	LexModeNormal,
	LexModeHashInclude,
	LexModeHashDefine,
	LexModeHashDefineSpace,
	LexModeHashDefineSpaceIdent
};

struct LexState {
	const char *Pos;
	const char *End;
	const char *FileName;
	int Line;
	int CharacterPos;
	const char *SourceText;
	enum LexMode Mode;
	int EmitExtraNewlines;
};

/* library function definition */
struct LibraryFunction {
	void (*Func)(struct ParseState *Parser, struct Value *, struct Value **, int);
	const char *Prototype;
};

/* output stream-type specific state information */
union OutputStreamInfo {
	struct StringOutputStream {
		struct ParseState *Parser;
		char *WritePos;
	} Str;
};

/* stream-specific method for writing characters to the console */
typedef void CharWriter(unsigned char, union OutputStreamInfo *);

/* used when writing output to a string - eg. sprintf() */
struct OutputStream {
	CharWriter *Putch;
	union OutputStreamInfo i;
};

/* possible results of parsing a statement */
enum ParseResult {
	ParseResultEOF,
	ParseResultError,
	ParseResultOk
};

/* globals */
extern void *HeapStackTop;
extern struct Table GlobalTable;
extern struct StackFrame *TopStackFrame;
extern struct ValueType UberType;
extern struct ValueType IntType;
extern struct ValueType CharType;
extern struct ValueType FPType;
extern struct ValueType VoidType;
extern struct ValueType TypeType;
extern struct ValueType FunctionType;
extern struct ValueType MacroType;
extern struct ValueType GotoLabelType;
extern struct ValueType *CharPtrType;
extern struct ValueType *CharPtrPtrType;
extern struct ValueType *CharArrayType;
extern struct ValueType *VoidPtrType;
extern char *StrEmpty;
extern struct PointerValue NULLPointer;
extern struct LibraryFunction CLibrary[];
extern struct LibraryFunction PlatformLibrary[];
extern IOFILE *CStdOut;

#include <eci/lex.h>
#include <eci/parse.h>
#include <eci/expression.h>

#include <eci/heap.h>

#include <eci/table.h>
#include <eci/type.h>
#include <eci/variable.h>
#include <eci/include.h>
#include <eci/clibrary.h>
#include <eci/platform.h>
#include <eci/cstdlib/ctype.h>
#include <eci/cstdlib/errno.h>
#include <eci/cstdlib/math.h>
#include <eci/cstdlib/stdbool.h>
#include <eci/cstdlib/stdio.h>
#include <eci/cstdlib/stdlib.h>
#include <eci/cstdlib/string.h>
#include <eci/cstdlib/time.h>
#include <eci/cstdlib/unistd.h>


// TODO : Move this ... platform.h:

/* the following are defined in picoc.h:
 * void PicocCallMain(int argc, char **argv);
 * int PicocPlatformSetExitPoint();
 * void PicocInitialise(int StackSize);
 * void PicocCleanup();
 * void PicocPlatformScanFile(const char *FileName);
 * extern int PicocExitValue; */
void ProgramFail(struct ParseState *_parser, const char *_message, ...);
void AssignFail(struct ParseState *_parser, const char *_mormat, struct ValueType *_type1, struct ValueType *_type2, int _num1, int _num2, const char *_funcName, int _paramNo);
void LexFail(struct LexState *_lexer, const char *_message, ...);
void PlatformCleanup();
char *PlatformGetLine(char *_buf, int _maxLen, const char *_prompt);
int PlatformGetCharacter();
void PlatformPutc(unsigned char _outCh, union OutputStreamInfo *);
void PlatformErrorPrefix(struct ParseState *_parser);
void PlatformPrintf(const char *_format, ...);
void PlatformVPrintf(const char *_format, va_list _args);
void PlatformExit(int _exitVal);
char *PlatformMakeTempName(char *_tempNameBuffer);
void PlatformLibraryInit();


#endif
