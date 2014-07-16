/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

static int ZeroValue = 0;

void StringStrcpy(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strcpy(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrncpy(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strncpy(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringStrcmp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strcmp(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrncmp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strncmp(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringStrcat(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strcat(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrncat(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strncat(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringIndex(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = index(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void StringRindex(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = rindex(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void StringStrlen(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strlen(_param[0]->Val->Pointer);
}

void StringMemset(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = memset(_param[0]->Val->Pointer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void StringMemcpy(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = memcpy(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringMemcmp(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = memcmp(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringMemmove(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = memmove(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringMemchr(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = memchr(_param[0]->Val->Pointer, _param[1]->Val->Integer, _param[2]->Val->Integer);
}

void StringStrchr(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strchr(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void StringStrrchr(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strrchr(_param[0]->Val->Pointer, _param[1]->Val->Integer);
}

void StringStrcoll(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strcoll(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrerror(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strerror(_param[0]->Val->Integer);
}

void StringStrspn(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strspn(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrcspn(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strcspn(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrpbrk(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strpbrk(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrstr(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strstr(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrtok(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strtok(_param[0]->Val->Pointer, _param[1]->Val->Pointer);
}

void StringStrxfrm(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = strxfrm(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Integer);
}

void StringStrdup(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strdup(_param[0]->Val->Pointer);
}

void StringStrtok_r(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Pointer = strtok_r(_param[0]->Val->Pointer, _param[1]->Val->Pointer, _param[2]->Val->Pointer);
}

/* all string.h functions */
struct LibraryFunction StringFunctions[] = {
	{ StringIndex,         "char *index(char *,int);" },
	{ StringRindex,        "char *rindex(char *,int);" },
	{ StringMemcpy,        "void *memcpy(void *,void *,int);" },
	{ StringMemmove,       "void *memmove(void *,void *,int);" },
	{ StringMemchr,        "void *memchr(char *,int,int);" },
	{ StringMemcmp,        "int memcmp(void *,void *,int);" },
	{ StringMemset,        "void *memset(void *,int,int);" },
	{ StringStrcat,        "char *strcat(char *,char *);" },
	{ StringStrncat,       "char *strncat(char *,char *,int);" },
	{ StringStrchr,        "char *strchr(char *,int);" },
	{ StringStrrchr,       "char *strrchr(char *,int);" },
	{ StringStrcmp,        "int strcmp(char *,char *);" },
	{ StringStrncmp,       "int strncmp(char *,char *,int);" },
	{ StringStrcoll,       "int strcoll(char *,char *);" },
	{ StringStrcpy,        "char *strcpy(char *,char *);" },
	{ StringStrncpy,       "char *strncpy(char *,char *,int);" },
	{ StringStrerror,      "char *strerror(int);" },
	{ StringStrlen,        "int strlen(char *);" },
	{ StringStrspn,        "int strspn(char *,char *);" },
	{ StringStrcspn,       "int strcspn(char *,char *);" },
	{ StringStrpbrk,       "char *strpbrk(char *,char *);" },
	{ StringStrstr,        "char *strstr(char *,char *);" },
	{ StringStrtok,        "char *strtok(char *,char *);" },
	{ StringStrxfrm,       "int strxfrm(char *,char *,int);" },
	{ StringStrdup,        "char *strdup(char *);" },
	{ StringStrtok_r,      "char *strtok_r(char *,char *,char **);" },
	{ NULL,             NULL }
};

/* creates various system-dependent definitions */
void StringSetupFunc() {
	/* define NULL */
	if (!VariableDefined(TableStrRegister("NULL"))) {
		VariableDefinePlatformVar(NULL, "NULL", &IntType, (union AnyValue *)&ZeroValue, FALSE);
	}
}

#endif /* !BUILTIN_MINI_STDLIB */
