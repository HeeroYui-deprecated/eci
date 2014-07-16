/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_PARSE_H__
#define __ECI_PARSE_H__


/* the following are defined in picoc.h:
 * void PicocParse(const char *FileName, const char *Source, int SourceLen, int RunIt, int CleanupNow, int CleanupSource);
 * void PicocParseInteractive(); */
enum ParseResult ParseStatement(struct ParseState *_parser, int _checkTrailingSemicolon);
struct Value *ParseFunctionDefinition(struct ParseState *_parser, struct ValueType *_returnType, char *_identifier);
void ParseCleanup();
void ParserCopyPos(struct ParseState *_to, struct ParseState *_from);
void ParserCopy(struct ParseState *_to, struct ParseState *_from);

#endif
