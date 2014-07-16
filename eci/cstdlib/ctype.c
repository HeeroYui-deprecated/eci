/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

/* string.h library for large systems - small embedded systems use clibrary.c instead */
#include <ctype.h>
#include "../interpreter.h"

#ifndef BUILTIN_MINI_STDLIB

void StdIsalnum(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isalnum(_param[0]->Val->Integer);
}

void StdIsalpha(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isalpha(_param[0]->Val->Integer);
}

void StdIsblank(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	int ch = _param[0]->Val->Integer;
	_returnValue->Val->Integer = (ch == ' ') | (ch == '\t');
}

void StdIscntrl(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = iscntrl(_param[0]->Val->Integer);
}

void StdIsdigit(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isdigit(_param[0]->Val->Integer);
}

void StdIsgraph(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isgraph(_param[0]->Val->Integer);
}

void StdIslower(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = islower(_param[0]->Val->Integer);
}

void StdIsprint(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isprint(_param[0]->Val->Integer);
}

void StdIspunct(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = ispunct(_param[0]->Val->Integer);
}

void StdIsspace(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isspace(_param[0]->Val->Integer);
}

void StdIsupper(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isupper(_param[0]->Val->Integer);
}

void StdIsxdigit(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isxdigit(_param[0]->Val->Integer);
}

void StdTolower(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = tolower(_param[0]->Val->Integer);
}

void StdToupper(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = toupper(_param[0]->Val->Integer);
}

void StdIsascii(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = isascii(_param[0]->Val->Integer);
}

void StdToascii(struct ParseState *_parser, struct Value *_returnValue, struct Value **_param, int _numArgs) {
	_returnValue->Val->Integer = toascii(_param[0]->Val->Integer);
}

/* all string.h functions */
struct LibraryFunction StdCtypeFunctions[] = {
	{ StdIsalnum,  "int isalnum(int);" },
	{ StdIsalpha,  "int isalpha(int);" },
	{ StdIsblank,  "int isblank(int);" },
	{ StdIscntrl,  "int iscntrl(int);" },
	{ StdIsdigit,  "int isdigit(int);" },
	{ StdIsgraph,  "int isgraph(int);" },
	{ StdIslower,  "int islower(int);" },
	{ StdIsprint,  "int isprint(int);" },
	{ StdIspunct,  "int ispunct(int);" },
	{ StdIsspace,  "int isspace(int);" },
	{ StdIsupper,  "int isupper(int);" },
	{ StdIsxdigit, "int isxdigit(int);" },
	{ StdTolower,  "int tolower(int);" },
	{ StdToupper,  "int toupper(int);" },
	{ StdIsascii,  "int isascii(int);" },
	{ StdToascii,  "int toascii(int);" },
	{ NULL, NULL }
};

#endif /* !BUILTIN_MINI_STDLIB */
