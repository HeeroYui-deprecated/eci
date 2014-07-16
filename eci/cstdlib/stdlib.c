/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

/* stdlib.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int ZeroValue = 0;

void StdlibAtof(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->FP = atof(_param[0]->Val->Pointer);
}

void StdlibAtoi(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = atoi(_param[0]->Val->Pointer);
}

void StdlibAtol(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = atol(_param[0]->Val->Pointer);
}

void StdlibStrtod(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->FP = strtod(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StdlibStrtol(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strtol(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StdlibStrtoul(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strtoul(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StdlibMalloc(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = malloc(_param[0]->Val->Integer);
}

void StdlibCalloc(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = calloc(_param[0]->Val->Integer, _param[1]->Val->Integer);
}

void StdlibRealloc(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = realloc(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void StdlibFree(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	free(_param[0]->Val->Pointer);
}

void StdlibRand(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = rand();
}

void StdlibSrand(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	srand(_param[0]->Val->Integer);
}

void StdlibAbort(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	ProgramFail(_parser, "abort");
}

void StdlibExit(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	PlatformExit(_param[0]->Val->Integer);
}

void StdlibGetenv(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = getenv(_param[0]->Val->Pointer);
}

void StdlibSystem(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = system(_param[0]->Val->Pointer);
}

void StdlibAbs(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = abs(_param[0]->Val->Integer);
}

void StdlibLabs(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = labs(_param[0]->Val->Integer);
}

/* all stdlib.h functions */
struct LibraryFunction StdlibFunctions[] =
{
	{ StdlibAtof,           "float atof(char *);" },
	{ StdlibStrtod,         "float strtod(char *,char **);" },
	{ StdlibAtoi,           "int atoi(char *);" },
	{ StdlibAtol,           "int atol(char *);" },
	{ StdlibStrtol,         "int strtol(char *,char **,int);" },
	{ StdlibStrtoul,        "int strtoul(char *,char **,int);" },
	{ StdlibMalloc,         "void *malloc(int);" },
	{ StdlibCalloc,         "void *calloc(int,int);" },
	{ StdlibRealloc,        "void *realloc(void *,int);" },
	{ StdlibFree,           "void free(void *);" },
	{ StdlibRand,           "int rand();" },
	{ StdlibSrand,          "void srand(int);" },
	{ StdlibAbort,          "void abort();" },
	{ StdlibExit,           "void exit(int);" },
	{ StdlibGetenv,         "char *getenv(char *);" },
	{ StdlibSystem,         "int system(char *);" },
	{ StdlibAbs,            "int abs(int);" },
	{ StdlibLabs,           "int labs(int);" },
	{ NULL,                 NULL }
};

/* creates various system-dependent definitions */
void StdlibSetupFunc() {
	/* define NULL, TRUE and FALSE */
	if (!VariableDefined(TableStrRegister("NULL"))) {
		VariableDefinePlatformVar(NULL, "NULL", &IntType, (union AnyValue *)&ZeroValue, FALSE);
	}
}

#endif
