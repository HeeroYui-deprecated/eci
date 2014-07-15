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

static int trueValue = 1;
static int falseValue = 0;


/* structure definitions */
const char StdboolDefs[] = "typedef int bool;";

/* creates various system-dependent definitions */
void StdboolSetupFunc() {
	/* defines */
	VariableDefinePlatformVar(NULL, "true", &IntType, (union AnyValue *)&trueValue, FALSE);
	VariableDefinePlatformVar(NULL, "false", &IntType, (union AnyValue *)&falseValue, FALSE);
	VariableDefinePlatformVar(NULL, "__bool_true_false_are_defined", &IntType, (union AnyValue *)&trueValue, FALSE);
}

#endif
