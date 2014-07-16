/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_TYPE_H__
#define __ECI_TYPE_H__

void TypeInit();
void TypeCleanup();
int TypeSize(struct ValueType *_type, int _arraySize, int _compact);
int TypeSizeValue(struct Value *_value, int _compact);
int TypeStackSizeValue(struct Value *_value);
int TypeLastAccessibleOffset(struct Value *_value);
int TypeParseFront(struct ParseState *_parser, struct ValueType **_type, int *_isStatic);
void TypeParseIdentPart(struct ParseState *_parser, struct ValueType *_basicTyp, struct ValueType **_type, char **_identifier);
void TypeParse(struct ParseState *_parser, struct ValueType **_type, char **_identifier, int *_isStatic);
struct ValueType *TypeGetMatching(struct ParseState *_parser, struct ValueType *_parentType, enum BaseType _base, int _arraySize, const char *_identifier, int _allowDuplicates);
struct ValueType *TypeCreateOpaqueStruct(struct ParseState *_parser, const char *_structName, int _size);

#endif
