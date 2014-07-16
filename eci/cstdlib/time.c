/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <time.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int CLOCKS_PER_SECValue = CLOCKS_PER_SEC;

#ifdef CLK_PER_SEC
static int CLK_PER_SECValue = CLK_PER_SEC;
#endif

#ifdef CLK_TCK
static int CLK_TCKValue = CLK_TCK;
#endif

void StdAsctime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = asctime(_param[0]->Val->Pointer);
}

void StdClock(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = clock();
}

void StdCtime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = ctime(_param[0]->Val->Pointer);
}

void StdDifftime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->FP = difftime((time_t)_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void StdGmtime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = gmtime(_param[0]->Val->Pointer);
}

void StdGmtime_r(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = gmtime_r(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StdLocaltime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = localtime(_param[0]->Val->Pointer);
}

void StdMktime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = mktime(_param[0]->Val->Pointer);
}

void StdTime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = time(_param[0]->Val->Pointer);
}

void StdStrftime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strftime(_param[0]->Val->Pointer, _param[1]->Val->Integer, _param[2]->Val->Pointer, _param[3]->Val->Pointer);
}

void StdStrptime(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	extern char *strptime(const char *s, const char *format, struct tm *tm);
	_returnValue->Val->Pointer = strptime(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Pointer);
}

void StdTimegm(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = timegm(_param[0]->Val->Pointer);
}

/* handy structure definitions */
const char StdTimeDefs[] = "\
typedef int time_t; \
typedef int clock_t;\
";

/* all string.h functions */
struct LibraryFunction StdTimeFunctions[] = {
	{ StdAsctime,       "char *asctime(struct tm *);" },
	{ StdClock,         "time_t clock();" },
	{ StdCtime,         "char *ctime(int *);" },
	{ StdDifftime,      "double difftime(int, int);" },
	{ StdGmtime,        "struct tm *gmtime(int *);" },
	{ StdGmtime_r,      "struct tm *gmtime_r(int *, struct tm *);" },
	{ StdLocaltime,     "struct tm *localtime(int *);" },
	{ StdMktime,        "int mktime(struct tm *ptm);" },
	{ StdTime,          "int time(int *);" },
	{ StdStrftime,      "int strftime(char *, int, char *, struct tm *);" },
	{ StdStrptime,      "char *strptime(char *, char *, struct tm *);" },
	{ StdTimegm,        "int timegm(struct tm *);" },
	{ NULL,             NULL }
};


/* creates various system-dependent definitions */
void StdTimeSetupFunc() {
	/* make a "struct tm" which is the same size as a native tm structure */
	TypeCreateOpaqueStruct(NULL, TableStrRegister("tm"), sizeof(struct tm));
	/* define CLK_PER_SEC etc. */
	VariableDefinePlatformVar(NULL, "CLOCKS_PER_SEC", &IntType, (union AnyValue *)&CLOCKS_PER_SECValue, FALSE);
#ifdef CLK_PER_SEC
	VariableDefinePlatformVar(NULL, "CLK_PER_SEC", &IntType, (union AnyValue *)&CLK_PER_SECValue, FALSE);
#endif
#ifdef CLK_TCK
	VariableDefinePlatformVar(NULL, "CLK_TCK", &IntType, (union AnyValue *)&CLK_TCKValue, FALSE);
#endif
}

#endif
