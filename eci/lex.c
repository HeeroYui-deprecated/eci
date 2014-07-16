/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#include "interpreter.h"

#define isCidstart(c) (isalpha(c) || (c)=='_' || (c)=='#')
#define isCident(c) (isalnum(c) || (c)=='_')

#define IS_HEX_ALPHA_DIGIT(c) (((c) >= 'a' && (c) <= 'f') || ((c) >= 'A' && (c) <= 'F'))
#define IS_BASE_DIGIT(c,b) (((c) >= '0' && (c) < '0' + (((b)<10)?(b):10)) || (((b) > 10) ? IS_HEX_ALPHA_DIGIT(c) : FALSE))
#define GET_BASE_DIGIT(c) (((c) <= '9') ? ((c) - '0') : (((c) <= 'F') ? ((c) - 'A' + 10) : ((c) - 'a' + 10)))

#define NEXTIS(c,x,y) { if (NextChar == (c)) { LEXER_INC(_lexer); GotToken = (x); } else GotToken = (y); }
#define NEXTIS3(c,x,d,y,z) { if (NextChar == (c)) { LEXER_INC(_lexer); GotToken = (x); } else NEXTIS(d,y,z) }
#define NEXTIS4(c,x,d,y,e,z,a) { if (NextChar == (c)) { LEXER_INC(_lexer); GotToken = (x); } else NEXTIS3(d,y,e,z,a) }
#define NEXTIS3PLUS(c,x,d,y,e,z,a) { if (NextChar == (c)) { LEXER_INC(_lexer); GotToken = (x); } else if (NextChar == (d)) { if (_lexer->Pos[1] == (e)) { LEXER_INCN(_lexer, 2); GotToken = (z); } else { LEXER_INC(_lexer); GotToken = (y); } } else GotToken = (a); }
#define NEXTISEXACTLY3(c,d,y,z) { if (NextChar == (c) && _lexer->Pos[1] == (d)) { LEXER_INCN(_lexer, 2); GotToken = (y); } else GotToken = (z); }

#define LEXER_INC(l) ( (l)->Pos++, (l)->CharacterPos++ )
#define LEXER_INCN(l, n) ( (l)->Pos+=(n), (l)->CharacterPos+=(n) )
#define TOKEN_DATA_OFFSET 2

#define MAX_CHAR_VALUE 255	  /* maximum value which can be represented by a "char" data type */

static union AnyValue LexAnyValue;
static struct Value LexValue = {
	TypeVoid,
	&LexAnyValue,
	FALSE,
	FALSE
};

struct ReservedWord {
	const char *Word;
	enum LexToken Token;
	const char *SharedWord; /* word stored in shared string space */
};

static struct ReservedWord ReservedWords[] = {
	{ "#define", TokenHashDefine, NULL },
	{ "#else", TokenHashElse, NULL },
	{ "#endif", TokenHashEndif, NULL },
	{ "#if", TokenHashIf, NULL },
	{ "#ifdef", TokenHashIfdef, NULL },
	{ "#ifndef", TokenHashIfndef, NULL },
	{ "#include", TokenHashInclude, NULL },
	{ "auto", TokenAutoType, NULL },
	{ "break", TokenBreak, NULL },
	{ "case", TokenCase, NULL },
	{ "char", TokenCharType, NULL },
	{ "continue", TokenContinue, NULL },
	{ "default", TokenDefault, NULL },
	{ "delete", TokenDelete, NULL },
	{ "do", TokenDo, NULL },
	{ "double", TokenDoubleType, NULL },
	{ "else", TokenElse, NULL },
	{ "enum", TokenEnumType, NULL },
	{ "extern", TokenExternType, NULL },
	{ "float", TokenFloatType, NULL },
	{ "for", TokenFor, NULL },
	{ "goto", TokenGoto, NULL },
	{ "if", TokenIf, NULL },
	{ "int", TokenIntType, NULL },
	{ "long", TokenLongType, NULL },
	{ "new", TokenNew, NULL },
	{ "register", TokenRegisterType, NULL },
	{ "return", TokenReturn, NULL },
	{ "short", TokenShortType, NULL },
	{ "signed", TokenSignedType, NULL },
	{ "sizeof", TokenSizeof, NULL },
	{ "static", TokenStaticType, NULL },
	{ "struct", TokenStructType, NULL },
	{ "switch", TokenSwitch, NULL },
	{ "typedef", TokenTypedef, NULL },
	{ "union", TokenUnionType, NULL },
	{ "unsigned", TokenUnsignedType, NULL },
	{ "void", TokenVoidType, NULL },
	{ "while", TokenWhile, NULL }
};

/* linked list of tokens used in interactive mode */
struct TokenLine {
	struct TokenLine *Next;
	unsigned char *Tokens;
	int NumBytes;
};

static struct TokenLine *InteractiveHead = NULL;
static struct TokenLine *InteractiveTail = NULL;
static struct TokenLine *InteractiveCurrentLine = NULL;
static int LexUseStatementPrompt = FALSE;


/* initialise the lexer */
void LexInit() {
	int Count;
	for (Count = 0; Count < sizeof(ReservedWords) / sizeof(struct ReservedWord); Count++) {
		ReservedWords[Count].SharedWord = TableStrRegister(ReservedWords[Count].Word);
	}
}

/* deallocate */
void LexCleanup() {
	LexInteractiveClear(NULL);
}

/* check if a word is a reserved word - used while scanning */
enum LexToken LexCheckReservedWord(const char *_word) {
	int Count;
	for (Count = 0; Count < sizeof(ReservedWords) / sizeof(struct ReservedWord); Count++) {
		if (_word == ReservedWords[Count].SharedWord) {
			return ReservedWords[Count].Token;
		}
	}
	return TokenNone;
}

/* get a numeric literal - used while scanning */
enum LexToken LexGetNumber(struct LexState *_lexer, struct Value *_value) {
	int Result = 0;
	int Base = 10;
	enum LexToken ResultToken;
	double FPResult;
	double FPDiv;
	if (*_lexer->Pos == '0') {
		/* a binary, octal or hex literal */
		LEXER_INC(_lexer);
		if (_lexer->Pos != _lexer->End) {
			if (    *_lexer->Pos == 'x'
			     || *_lexer->Pos == 'X') {
				Base = 16;
				LEXER_INC(_lexer);
			} else if (*_lexer->Pos == 'b' || *_lexer->Pos == 'B') {
				Base = 2;
				LEXER_INC(_lexer);
			} else if (*_lexer->Pos != '.') {
				Base = 8;
			}
		}
	}
	/* get the value */
	for (; _lexer->Pos != _lexer->End && IS_BASE_DIGIT(*_lexer->Pos, Base); LEXER_INC(_lexer)) {
		Result = Result * Base + GET_BASE_DIGIT(*_lexer->Pos);
	}
	if (Result >= 0 && Result <= MAX_CHAR_VALUE) {
		_value->Typ = &CharType;
		_value->Val->Character = Result;
		ResultToken = TokenCharacterConstant;
	} else {
		_value->Typ = &IntType;
		_value->Val->Integer = Result;
		ResultToken = TokenIntegerConstant;
	}
	if (_lexer->Pos == _lexer->End) {
		return ResultToken;
	}
	if (    *_lexer->Pos == 'l'
	     || *_lexer->Pos == 'L') {
		LEXER_INC(_lexer);
		return ResultToken;
	}
	if (    _lexer->Pos == _lexer->End
	     || *_lexer->Pos != '.') {
		return ResultToken;
	}
	_value->Typ = &FPType;
	LEXER_INC(_lexer);
	for (FPDiv = 1.0/Base, FPResult = (double)Result; _lexer->Pos != _lexer->End && IS_BASE_DIGIT(*_lexer->Pos, Base); LEXER_INC(_lexer), FPDiv /= (double)Base) {
		FPResult += GET_BASE_DIGIT(*_lexer->Pos) * FPDiv;
	}
	
	if (    _lexer->Pos != _lexer->End
	     && (    *_lexer->Pos == 'e'
	          || *_lexer->Pos == 'E') ) {
		double ExponentMultiplier = 1.0;
		LEXER_INC(_lexer);
		if (_lexer->Pos != _lexer->End && *_lexer->Pos == '-') {
			ExponentMultiplier = -1.0;
			LEXER_INC(_lexer);
		}
		for (Result = 0; _lexer->Pos != _lexer->End && IS_BASE_DIGIT(*_lexer->Pos, Base); LEXER_INC(_lexer)) {
			Result = Result * (double)Base + GET_BASE_DIGIT(*_lexer->Pos);
		}
		FPResult *= pow((double)Base, (double)Result * ExponentMultiplier);
	}
	_value->Val->FP = FPResult;
	return TokenFPConstant;
}

/* get a reserved word or identifier - used while scanning */
enum LexToken LexGetWord(struct LexState *_lexer, struct Value *_value) {
	const char *StartPos = _lexer->Pos;
	enum LexToken Token;
	do {
		LEXER_INC(_lexer);
	} while (    _lexer->Pos != _lexer->End
	          && isCident((int)*_lexer->Pos));
	_value->Typ = NULL;
	_value->Val->Identifier = TableStrRegister2(StartPos, _lexer->Pos - StartPos);
	Token = LexCheckReservedWord(_value->Val->Identifier);
	switch (Token) {
		case TokenHashInclude: _lexer->Mode = LexModeHashInclude; break;
		case TokenHashDefine: _lexer->Mode = LexModeHashDefine; break;
		default: break;
	}
	if (Token != TokenNone) {
		return Token;
	}
	if (_lexer->Mode == LexModeHashDefineSpace) {
		_lexer->Mode = LexModeHashDefineSpaceIdent;
	}
	return TokenIdentifier;
}

/* unescape a character from an octal character constant */
unsigned char LexUnEscapeCharacterConstant(const char** _from, const char* _end, unsigned char _firstChar, int _base) {
	unsigned char total = GET_BASE_DIGIT(_firstChar);
	int CCount;
	for (CCount = 0; IS_BASE_DIGIT(**_from, _base) && CCount < 2; CCount++, (*_from)++) {
		total = total * _base + GET_BASE_DIGIT(**_from);
	}
	return total;
}

/* unescape a character from a string or character constant */
unsigned char LexUnEscapeCharacter(const char **_from, const char *_end) {
	unsigned char ThisChar;
	while (    *_from != _end
	        && **_from == '\\'
	        && &(*_from)[1] != _end
	        && (*_from)[1] == '\n' ) {
		(*_from) += 2;	   /* skip escaped end of lines with LF line termination */
	}
	while (    *_from != _end
	        && **_from == '\\'
	        && &(*_from)[1] != _end
	        && &(*_from)[2] != _end
	        && (*_from)[1] == '\r'
	        && (*_from)[2] == '\n') {
		(*_from) += 3;	   /* skip escaped end of lines with CR/LF line termination */
	}
	if (*_from == _end) {
		return '\\';
	}
	if (**_from == '\\') {
		/* it's escaped */
		(*_from)++;
		if (*_from == _end) {
			return '\\';
		}
		ThisChar = *(*_from)++;
		switch (ThisChar) {
			case '\\':
				return '\\'; 
			case '\'':
				return '\'';
			case '"':
				return '"';
			case 'a':
				return '\a';
			case 'b':
				return '\b';
			case 'f':
				return '\f';
			case 'n':
				return '\n';
			case 'r':
				return '\r';
			case 't':
				return '\t';
			case 'v':
				return '\v';
			case '0':
			case '1':
			case '2':
			case '3':
				return LexUnEscapeCharacterConstant(_from, _end, ThisChar, 8);
			case 'x':
				return LexUnEscapeCharacterConstant(_from, _end, '0', 16);
			default:
				return ThisChar;
		}
	} else {
		return *(*_from)++;
	}
}

/* get a string constant - used while scanning */
enum LexToken LexGetStringConstant(struct LexState *_lexer, struct Value *_value, char _endChar) {
	int Escape = FALSE;
	const char *StartPos = _lexer->Pos;
	const char *EndPos;
	char *EscBuf;
	char *EscBufPos;
	char *RegString;
	struct Value *ArrayValue;
	
	while (    _lexer->Pos != _lexer->End
	        && (    *_lexer->Pos != _endChar
	             || Escape)) {
		/* find the end */
		if (Escape) {
			if (    *_lexer->Pos == '\r'
			     && _lexer->Pos+1 != _lexer->End) {
				_lexer->Pos++;
			}
			if (    *_lexer->Pos == '\n'
			     && _lexer->Pos+1 != _lexer->End) {
				_lexer->Line++;
				_lexer->Pos++;
				_lexer->CharacterPos = 0;
				_lexer->EmitExtraNewlines++;
			}
			Escape = FALSE;
		} else if (*_lexer->Pos == '\\') {
			Escape = TRUE;
		}
		LEXER_INC(_lexer);
	}
	EndPos = _lexer->Pos;
	EscBuf = HeapAllocStack(EndPos - StartPos);
	if (EscBuf == NULL) {
		LexFail(_lexer, "out of memory");
	}
	for (EscBufPos = EscBuf, _lexer->Pos = StartPos; _lexer->Pos != EndPos;) {
		*EscBufPos++ = LexUnEscapeCharacter(&_lexer->Pos, EndPos);
	}
	/* try to find an existing copy of this string literal */
	RegString = TableStrRegister2(EscBuf, EscBufPos - EscBuf);
	HeapPopStack(EscBuf, EndPos - StartPos);
	ArrayValue = VariableStringLiteralGet(RegString);
	if (ArrayValue == NULL) {
		/* create and store this string literal */
		ArrayValue = VariableAllocValueAndData(NULL, 0, FALSE, NULL, TRUE);
		ArrayValue->Typ = CharArrayType;
		ArrayValue->Val = (union AnyValue *)RegString;
		VariableStringLiteralDefine(RegString, ArrayValue);
	}
	/* create the the pointer for this char* */
	_value->Typ = CharPtrType;
	_value->Val->Pointer = RegString;
	if (*_lexer->Pos == _endChar) {
		LEXER_INC(_lexer);
	}
	return TokenStringConstant;
}

/* get a character constant - used while scanning */
enum LexToken LexGetCharacterConstant(struct LexState *_lexer, struct Value *_value) {
	_value->Typ = &CharType;
	_value->Val->Character = LexUnEscapeCharacter(&_lexer->Pos, _lexer->End);
	if (_lexer->Pos != _lexer->End && *_lexer->Pos != '\'') {
		LexFail(_lexer, "expected \"'\""); // "
	}
	LEXER_INC(_lexer);
	return TokenCharacterConstant;
}

/* skip a comment - used while scanning */
void LexSkipComment(struct LexState *_lexer, char _nextChar, enum LexToken *_returnToken) {
	if (_nextChar == '*') {
		/* conventional C comment */
		while (_lexer->Pos != _lexer->End && (*(_lexer->Pos-1) != '*' || *_lexer->Pos != '/')) {
			if (*_lexer->Pos == '\n') {
				_lexer->EmitExtraNewlines++;
			}
			LEXER_INC(_lexer);
		}
		if (_lexer->Pos != _lexer->End) {
			LEXER_INC(_lexer);
		}
		_lexer->Mode = LexModeNormal;
	} else {
		/* C++ style comment */
		while (    _lexer->Pos != _lexer->End
		        && *_lexer->Pos != '\n') {
			LEXER_INC(_lexer);
		}
	}
}

/* get a single token from the source - used while scanning */
enum LexToken LexScanGetToken(struct LexState *_lexer, struct Value **_value) {
	char ThisChar;
	char NextChar;
	enum LexToken GotToken = TokenNone;
	/* handle cases line multi-line comments or string constants which mess up the line count */
	if (_lexer->EmitExtraNewlines > 0) {
		_lexer->EmitExtraNewlines--;
		return TokenEndOfLine;
	}
	/* scan for a token */
	do {
		*_value = &LexValue;
		while (    _lexer->Pos != _lexer->End
		        && isspace((int)*_lexer->Pos)) {
			if (*_lexer->Pos == '\n') {
				_lexer->Line++;
				_lexer->Pos++;
				_lexer->Mode = LexModeNormal;
				_lexer->CharacterPos = 0;
				return TokenEndOfLine;
			} else if (    _lexer->Mode == LexModeHashDefine
			            || _lexer->Mode == LexModeHashDefineSpace) {
				_lexer->Mode = LexModeHashDefineSpace;
			} else if (_lexer->Mode == LexModeHashDefineSpaceIdent) {
				_lexer->Mode = LexModeNormal;
			}
			LEXER_INC(_lexer);
		}
		if (    _lexer->Pos == _lexer->End
		     || *_lexer->Pos == '\0') {
			return TokenEOF;
		}
		ThisChar = *_lexer->Pos;
		if (isCidstart((int)ThisChar)) {
			return LexGetWord(_lexer, *_value);
		}
		if (isdigit((int)ThisChar)) {
			return LexGetNumber(_lexer, *_value);
		}
		NextChar = (_lexer->Pos+1 != _lexer->End) ? *(_lexer->Pos+1) : 0;
		LEXER_INC(_lexer);
		switch (ThisChar) {
			case '"':
				GotToken = LexGetStringConstant(_lexer, *_value, '"');
				break;
			case '\'': //'
				GotToken = LexGetCharacterConstant(_lexer, *_value);
				break;
			case '(':
				if (_lexer->Mode == LexModeHashDefineSpaceIdent) {
					GotToken = TokenOpenMacroBracket;
				} else {
					GotToken = TokenOpenBracket;
				}
				_lexer->Mode = LexModeNormal;
				break;
			case ')': 
				GotToken = TokenCloseBracket;
				break;
			case '=':
				NEXTIS('=', TokenEqual, TokenAssign);
				break;
			case '+':
				NEXTIS3('=', TokenAddAssign, '+', TokenIncrement, TokenPlus);
				break;
			case '-':
				NEXTIS4('=', TokenSubtractAssign, '>', TokenArrow, '-', TokenDecrement, TokenMinus);
				break;
			case '*':
				NEXTIS('=', TokenMultiplyAssign, TokenAsterisk);
				break;
			case '/':
				if (    NextChar == '/'
				     || NextChar == '*') {
					LEXER_INC(_lexer);
					LexSkipComment(_lexer, NextChar, &GotToken);
				} else {
					NEXTIS('=', TokenDivideAssign, TokenSlash);
				}
				break;
			case '%':
				NEXTIS('=', TokenModulusAssign, TokenModulus);
				break;
			case '<':
				if (_lexer->Mode == LexModeHashInclude) {
					GotToken = LexGetStringConstant(_lexer, *_value, '>');
				} else {
					NEXTIS3PLUS('=', TokenLessEqual, '<', TokenShiftLeft, '=', TokenShiftLeftAssign, TokenLessThan);
				}
				break;
			case '>':
				NEXTIS3PLUS('=', TokenGreaterEqual, '>', TokenShiftRight, '=', TokenShiftRightAssign, TokenGreaterThan);
				break;
			case ';':
				GotToken = TokenSemicolon;
				break;
			case '&':
				NEXTIS3('=', TokenArithmeticAndAssign, '&', TokenLogicalAnd, TokenAmpersand);
				break;
			case '|':
				NEXTIS3('=', TokenArithmeticOrAssign, '|', TokenLogicalOr, TokenArithmeticOr);
				break;
			case '{':
				GotToken = TokenLeftBrace;
				break;
			case '}':
				GotToken = TokenRightBrace;
				break;
			case '[':
				GotToken = TokenLeftSquareBracket;
				break;
			case ']':
				GotToken = TokenRightSquareBracket;
				break;
			case '!':
				NEXTIS('=', TokenNotEqual, TokenUnaryNot);
				break;
			case '^':
				NEXTIS('=', TokenArithmeticExorAssign, TokenArithmeticExor);
				break;
			case '~':
				GotToken = TokenUnaryExor;
				break;
			case ',':
				GotToken = TokenComma;
				break;
			case '.':
				NEXTISEXACTLY3('.', '.', TokenEllipsis, TokenDot);
				break;
			case '?':
				GotToken = TokenQuestionMark;
				break;
			case ':':
				GotToken = TokenColon;
				break;
			default:
				LexFail(_lexer, "illegal character '%c'", ThisChar);
				break;
		}
	} while (GotToken == TokenNone);
	return GotToken;
}

/* what size value goes with each token */
int LexTokenSize(enum LexToken _token) {
	switch (_token) {
		case TokenIdentifier:
		case TokenStringConstant:
			return sizeof(char *);
		case TokenIntegerConstant:
			return sizeof(int);
		case TokenCharacterConstant:
			return sizeof(unsigned char);
		case TokenFPConstant:
			return sizeof(double);
		default:
			return 0;
	}
}

/* produce tokens from the lexer and return a heap buffer with the result - used for scanning */
void *LexTokenise(struct LexState *_lexer, int *_tokenLen) {
	enum LexToken Token;
	void *HeapMem;
	struct Value *GotValue;
	int MemUsed = 0;
	int ValueSize;
	int ReserveSpace = (_lexer->End - _lexer->Pos) * 4 + 16; 
	void *TokenSpace = HeapAllocStack(ReserveSpace);
	char *TokenPos = (char *)TokenSpace;
	int LastCharacterPos = 0;
	if (TokenSpace == NULL) {
		LexFail(_lexer, "out of memory");
	}
	do {
		/* store the token at the end of the stack area */
		Token = LexScanGetToken(_lexer, &GotValue);
#ifdef DEBUG_LEXER
		printf("Token: %02x\n", Token);
#endif
		*(unsigned char *)TokenPos = Token;
		TokenPos++;
		MemUsed++;
		*(unsigned char *)TokenPos = (unsigned char)LastCharacterPos;
		TokenPos++;
		MemUsed++;
		ValueSize = LexTokenSize(Token);
		if (ValueSize > 0) {
			/* store a value as well */
			memcpy((void *)TokenPos, (void *)GotValue->Val, ValueSize);
			TokenPos += ValueSize;
			MemUsed += ValueSize;
		}
		LastCharacterPos = _lexer->CharacterPos;
	} while (Token != TokenEOF);
	HeapMem = HeapAllocMem(MemUsed);
	if (HeapMem == NULL) {
		LexFail(_lexer, "out of memory");
	}
	assert(ReserveSpace >= MemUsed);
	memcpy(HeapMem, TokenSpace, MemUsed);
	HeapPopStack(TokenSpace, ReserveSpace);
#ifdef DEBUG_LEXER
	{
		int Count;
		printf("Tokens: ");
		for (Count = 0; Count < MemUsed; Count++) {
			printf("%02x ", *((unsigned char *)HeapMem+Count));
		}
		printf("\n");
	}
#endif
	if (_tokenLen) {
		*_tokenLen = MemUsed;
	}
	return HeapMem;
}

/* lexically analyse some source text */
void *LexAnalyse(const char* _fileName, const char* _source, int _sourceLen, int *_tokenLen) {
	struct LexState Lexer;
	Lexer.Pos = _source;
	Lexer.End = _source + _sourceLen;
	Lexer.Line = 1;
	Lexer.FileName = _fileName;
	Lexer.Mode = LexModeNormal;
	Lexer.EmitExtraNewlines = 0;
	Lexer.CharacterPos = 1;
	Lexer.SourceText = _source;
	return LexTokenise(&Lexer, _tokenLen);
}

/* prepare to parse a pre-tokenised buffer */
void LexInitParser(struct ParseState* _parser, const char* _sourceText, void* _tokenSource, const char* _fileName, int _runIt){
	_parser->Pos = _tokenSource;
	_parser->Line = 1;
	_parser->FileName = _fileName;
	_parser->Mode = _runIt ? RunModeRun : RunModeSkip;
	_parser->SearchLabel = 0;
	_parser->HashIfLevel = 0;
	_parser->HashIfEvaluateToLevel = 0;
	_parser->CharacterPos = 0;
	_parser->SourceText = _sourceText;
}

/* get the next token, without pre-processing */
enum LexToken LexGetRawToken(struct ParseState *_parser, struct Value **_value, int _incPos) {
	enum LexToken Token = TokenNone;
	int ValueSize;
	char *Prompt = NULL;
	do {
		/* get the next token */
		if (    _parser->Pos == NULL
		     && InteractiveHead != NULL) {
			_parser->Pos = InteractiveHead->Tokens;
		}
		if (    _parser->FileName != StrEmpty
		     || InteractiveHead != NULL) {
			/* skip leading newlines */
			while ((Token = (enum LexToken)*(unsigned char *)_parser->Pos) == TokenEndOfLine) {
				_parser->Line++;
				_parser->Pos += TOKEN_DATA_OFFSET;
			}
		}
		if (    _parser->FileName == StrEmpty
		     && (    InteractiveHead == NULL
		          || Token == TokenEOF) ) {
			/* we're at the end of an interactive input token list */
			char LineBuffer[LINEBUFFER_MAX];
			void *LineTokens;
			int LineBytes;
			struct TokenLine *LineNode;
			if (    InteractiveHead == NULL
			     || (unsigned char *)_parser->Pos == &InteractiveTail->Tokens[InteractiveTail->NumBytes-TOKEN_DATA_OFFSET]) {
				/* get interactive input */
				if (LexUseStatementPrompt) {
					Prompt = INTERACTIVE_PROMPT_STATEMENT;
					LexUseStatementPrompt = FALSE;
				} else {
					Prompt = INTERACTIVE_PROMPT_LINE;
				}
				if (PlatformGetLine(&LineBuffer[0], LINEBUFFER_MAX, Prompt) == NULL) {
					return TokenEOF;
				}
				/* put the new line at the end of the linked list of interactive lines */
				LineTokens = LexAnalyse(StrEmpty, &LineBuffer[0], strlen(LineBuffer), &LineBytes);
				LineNode = VariableAlloc(_parser, sizeof(struct TokenLine), TRUE);
				LineNode->Tokens = LineTokens;
				LineNode->NumBytes = LineBytes;
				if (InteractiveHead == NULL) {
					/* start a new list */
					InteractiveHead = LineNode;
					_parser->Line = 1;
					_parser->CharacterPos = 0;
				} else {
					InteractiveTail->Next = LineNode;
				}
				InteractiveTail = LineNode;
				InteractiveCurrentLine = LineNode;
				_parser->Pos = LineTokens;
			} else {
				/* go to the next token line */
				if (_parser->Pos != &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET]) {
					/* scan for the line */
					for (InteractiveCurrentLine = InteractiveHead; _parser->Pos != &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET]; InteractiveCurrentLine = InteractiveCurrentLine->Next) {
						assert(InteractiveCurrentLine->Next != NULL);
					}
				}
				assert(InteractiveCurrentLine != NULL);
				InteractiveCurrentLine = InteractiveCurrentLine->Next;
				assert(InteractiveCurrentLine != NULL);
				_parser->Pos = InteractiveCurrentLine->Tokens;
			}
			Token = (enum LexToken)*(unsigned char *)_parser->Pos;
		}
	} while (    (    _parser->FileName == StrEmpty
	               && Token == TokenEOF)
	          || Token == TokenEndOfLine);
	_parser->CharacterPos = *((unsigned char *)_parser->Pos + 1);
	ValueSize = LexTokenSize(Token);
	if (ValueSize > 0) {
		/* this token requires a value - unpack it */
		if (_value != NULL) {
			switch (Token) {
				case TokenStringConstant:
					LexValue.Typ = CharPtrType;
					break;
				case TokenIdentifier:
					LexValue.Typ = NULL;
					break;
				case TokenIntegerConstant:
					LexValue.Typ = &IntType;
					break;
				case TokenCharacterConstant:
					LexValue.Typ = &CharType;
					break;
				case TokenFPConstant:
					LexValue.Typ = &FPType;
					break;
				default:
					break;
			}
			memcpy((void *)LexValue.Val, (void *)((char *)_parser->Pos + TOKEN_DATA_OFFSET), ValueSize);
			LexValue.ValOnHeap = FALSE;
			LexValue.ValOnStack = FALSE;
			LexValue.IsLValue = FALSE;
			LexValue.LValueFrom = NULL;
			*_value = &LexValue;
		}
		if (_incPos) {
			_parser->Pos += ValueSize + TOKEN_DATA_OFFSET;
		}
	} else {
		if (_incPos && Token != TokenEOF) {
			_parser->Pos += TOKEN_DATA_OFFSET;
		}
	}
#ifdef DEBUG_LEXER
	printf("Got token=%02x inc=%d pos=%d\n", Token, _incPos, _parser->CharacterPos);
#endif
	assert(Token >= TokenNone && Token <= TokenEndOfFunction);
	return Token;
}

/* correct the token position depending if we already incremented the position */
void LexHashIncPos(struct ParseState *_parser, int _incPos) {
	if (!_incPos) {
		LexGetRawToken(_parser, NULL, TRUE);
	}
}

/* handle a #ifdef directive */
void LexHashIfdef(struct ParseState *_parser, int _ifNot) {
	/* get symbol to check */
	struct Value *IdentValue;
	struct Value *SavedValue;
	int IsDefined;
	enum LexToken Token = LexGetRawToken(_parser, &IdentValue, TRUE);
	if (Token != TokenIdentifier) {
		ProgramFail(_parser, "identifier expected");
	}
	/* is the identifier defined? */
	IsDefined = TableGet(&GlobalTable, IdentValue->Val->Identifier, &SavedValue, NULL, NULL, NULL);
	if (    _parser->HashIfEvaluateToLevel == _parser->HashIfLevel
	     && (   (    IsDefined
	              && !_ifNot)
	         || (    !IsDefined
	               && _ifNot) ) ) {
		/* #if is active, evaluate to this new level */
		_parser->HashIfEvaluateToLevel++;
	}
	_parser->HashIfLevel++;
}

/* handle a #if directive */
void LexHashIf(struct ParseState *_parser) {
	/* get symbol to check */
	struct Value *IdentValue;
	struct Value *SavedValue;
	struct ParseState MacroParser;
	enum LexToken Token = LexGetRawToken(_parser, &IdentValue, TRUE);
	if (Token == TokenIdentifier) {
		/* look up a value from a macro definition */
		if (!TableGet(&GlobalTable, IdentValue->Val->Identifier, &SavedValue, NULL, NULL, NULL)) {
			ProgramFail(_parser, "'%s' is undefined", IdentValue->Val->Identifier);
		}
		if (SavedValue->Typ->Base != TypeMacro) {
			ProgramFail(_parser, "value expected");
		}
		ParserCopy(&MacroParser, &SavedValue->Val->MacroDef.Body);
		Token = LexGetRawToken(&MacroParser, &IdentValue, TRUE);
	}
	if (Token != TokenCharacterConstant) {
		ProgramFail(_parser, "value expected");
	}
	/* is the identifier defined? */
	if (_parser->HashIfEvaluateToLevel == _parser->HashIfLevel && IdentValue->Val->Character) {
		/* #if is active, evaluate to this new level */
		_parser->HashIfEvaluateToLevel++;
	}
	_parser->HashIfLevel++;
}

/* handle a #else directive */
void LexHashElse(struct ParseState *_parser) {
	if (_parser->HashIfEvaluateToLevel == _parser->HashIfLevel - 1) {
		_parser->HashIfEvaluateToLevel++;	 /* #if was not active, make this next section active */
	} else if (_parser->HashIfEvaluateToLevel == _parser->HashIfLevel) {
		/* #if was active, now go inactive */
		if (_parser->HashIfLevel == 0) {
			ProgramFail(_parser, "#else without #if");
		}
		_parser->HashIfEvaluateToLevel--;
	}
}

/* handle a #endif directive */
void LexHashEndif(struct ParseState *_parser) {
	if (_parser->HashIfLevel == 0) {
		ProgramFail(_parser, "#endif without #if");
	}
	_parser->HashIfLevel--;
	if (_parser->HashIfEvaluateToLevel > _parser->HashIfLevel) {
		_parser->HashIfEvaluateToLevel = _parser->HashIfLevel;
	}
}

/* get the next token given a parser state, pre-processing as we go */
enum LexToken LexGetToken(struct ParseState *_parser, struct Value **_value, int _incPos) {
	enum LexToken Token;
	int TryNextToken;
	/* implements the pre-processor #if commands */
	do {
		int WasPreProcToken = TRUE;
		Token = LexGetRawToken(_parser, _value, _incPos);
		switch (Token) {
			case TokenHashIfdef:
				LexHashIncPos(_parser, _incPos);
				LexHashIfdef(_parser, FALSE);
				break;
			case TokenHashIfndef:
				LexHashIncPos(_parser, _incPos);
				LexHashIfdef(_parser, TRUE);
				break;
			case TokenHashIf:
				LexHashIncPos(_parser, _incPos);
				LexHashIf(_parser);
				break;
			case TokenHashElse:
				LexHashIncPos(_parser, _incPos);
				LexHashElse(_parser);
				break;
			case TokenHashEndif:
				LexHashIncPos(_parser, _incPos);
				LexHashEndif(_parser);
				break;
			default:
				WasPreProcToken = FALSE;
				break;
		}
		/* if we're going to reject this token, increment the token pointer to the next one */
		TryNextToken = (_parser->HashIfEvaluateToLevel < _parser->HashIfLevel && Token != TokenEOF) || WasPreProcToken;
		if (!_incPos && TryNextToken) {
			LexGetRawToken(_parser, NULL, TRUE);
		}
	} while (TryNextToken);
	return Token;
}

/* take a quick peek at the next token, skipping any pre-processing */
enum LexToken LexRawPeekToken(struct ParseState *_parser) {
	return (enum LexToken)*(unsigned char *)_parser->Pos;
}

/* find the end of the line */
void LexToEndOfLine(struct ParseState *_parser) {
	while (TRUE) {
		enum LexToken Token = (enum LexToken)*(unsigned char *)_parser->Pos;
		if (    Token == TokenEndOfLine
		     || Token == TokenEOF) {
			return;
		} else {
			LexGetRawToken(_parser, NULL, TRUE);
		}
	}
}

/* copy the tokens from StartParser to EndParser into new memory, removing TokenEOFs and terminate with a TokenEndOfFunction */
void *LexCopyTokens(struct ParseState* _startParser, struct ParseState* _endParser) {
	int MemSize = 0;
	int CopySize;
	unsigned char *Pos = (unsigned char *)_startParser->Pos;
	unsigned char *NewTokens;
	unsigned char *NewTokenPos;
	struct TokenLine *ILine;
	if (InteractiveHead == NULL) {
		/* non-interactive mode - copy the tokens */
		MemSize = _endParser->Pos - _startParser->Pos;
		NewTokens = VariableAlloc(_startParser, MemSize + TOKEN_DATA_OFFSET, TRUE);
		memcpy(NewTokens, (void *)_startParser->Pos, MemSize);
	} else {
		/* we're in interactive mode - add up line by line */
		for (InteractiveCurrentLine = InteractiveHead; InteractiveCurrentLine != NULL && (Pos < &InteractiveCurrentLine->Tokens[0] || Pos >= &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes]); InteractiveCurrentLine = InteractiveCurrentLine->Next)
		{} /* find the line we just counted */
		
		if (_endParser->Pos >= _startParser->Pos && _endParser->Pos < &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes]) {
			/* all on a single line */
			MemSize = _endParser->Pos - _startParser->Pos;
			NewTokens = VariableAlloc(_startParser, MemSize + TOKEN_DATA_OFFSET, TRUE);
			memcpy(NewTokens, (void *)_startParser->Pos, MemSize);
		} else {
			/* it's spread across multiple lines */
			MemSize = &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET] - Pos;
			for (ILine = InteractiveCurrentLine->Next; ILine != NULL && (_endParser->Pos < &ILine->Tokens[0] || _endParser->Pos >= &ILine->Tokens[ILine->NumBytes]); ILine = ILine->Next) {
				MemSize += ILine->NumBytes - TOKEN_DATA_OFFSET;
			}
			assert(ILine != NULL);
			MemSize += _endParser->Pos - &ILine->Tokens[0];
			NewTokens = VariableAlloc(_startParser, MemSize + TOKEN_DATA_OFFSET, TRUE);
			CopySize = &InteractiveCurrentLine->Tokens[InteractiveCurrentLine->NumBytes-TOKEN_DATA_OFFSET] - Pos;
			memcpy(NewTokens, Pos, CopySize);
			NewTokenPos = NewTokens + CopySize;
			for (ILine = InteractiveCurrentLine->Next; ILine != NULL && (_endParser->Pos < &ILine->Tokens[0] || _endParser->Pos >= &ILine->Tokens[ILine->NumBytes]); ILine = ILine->Next) {
				memcpy(NewTokenPos, &ILine->Tokens[0], ILine->NumBytes - TOKEN_DATA_OFFSET);
				NewTokenPos += ILine->NumBytes-TOKEN_DATA_OFFSET;
			}
			assert(ILine != NULL);
			memcpy(NewTokenPos, &ILine->Tokens[0], _endParser->Pos - &ILine->Tokens[0]);
		}
	}
	NewTokens[MemSize] = (unsigned char)TokenEndOfFunction;
	return NewTokens;
}

/* indicate that we've completed up to this point in the interactive input and free expired tokens */
void LexInteractiveClear(struct ParseState *_parser) {
	while (InteractiveHead != NULL) {
		struct TokenLine *NextLine = InteractiveHead->Next;
		HeapFreeMem(InteractiveHead->Tokens);
		HeapFreeMem(InteractiveHead);
		InteractiveHead = NextLine;
	}
	if (_parser != NULL) {
		_parser->Pos = NULL;
	}
	InteractiveTail = NULL;
}

/* indicate that we've completed up to this point in the interactive input and free expired tokens */
void LexInteractiveCompleted(struct ParseState *_parser) {
	while (InteractiveHead != NULL && !(_parser->Pos >= &InteractiveHead->Tokens[0] && _parser->Pos < &InteractiveHead->Tokens[InteractiveHead->NumBytes])) {
		/* this token line is no longer needed - free it */
		struct TokenLine *NextLine = InteractiveHead->Next;
		HeapFreeMem(InteractiveHead->Tokens);
		HeapFreeMem(InteractiveHead);
		InteractiveHead = NextLine;
		if (InteractiveHead == NULL) {
			// we've emptied the list
			_parser->Pos = NULL;
			InteractiveTail = NULL;
		}
	}
}

/* the next time we prompt, make it the full statement prompt */
void LexInteractiveStatementPrompt() {
	LexUseStatementPrompt = TRUE;
}
