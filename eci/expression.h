/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_EXPRESSION_H__
#define __ECI_EXPRESSION_H__

int ExpressionParse(struct ParseState *_parser, struct Value **_result);
long ExpressionParseInt(struct ParseState *_parser);
void ExpressionAssign(struct ParseState *_parser, struct Value *_destValue, struct Value *_sourceValue, int _force, const char *_funcName, int _paramNo, int _allowPointerCoercion);
long ExpressionCoerceInteger(struct Value *_value);
unsigned long ExpressionCoerceUnsignedInteger(struct Value *_value);
double ExpressionCoerceFP(struct Value *_value);

#endif
