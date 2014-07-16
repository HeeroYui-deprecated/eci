/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_LEX_H__
#define __ECI_LEX_H__


void LexInit();
void LexCleanup();
void* LexAnalyse(const char* _fileName, const char *_source, int _sourceLen, int *_tokenLen);
void LexInitParser(struct ParseState* _parser, const char *_sourceText, void* _tokenSource, const char* _fileName, int _runIt);
enum LexToken LexGetToken(struct ParseState* _parser, struct Value **_value, int _incPos);
enum LexToken LexRawPeekToken(struct ParseState* _parser);
void LexToEndOfLine(struct ParseState* _parser);
void* LexCopyTokens(struct ParseState* _startParser, struct ParseState* _endParser);
void LexInteractiveClear(struct ParseState* _parser);
void LexInteractiveCompleted(struct ParseState* _parser);
void LexInteractiveStatementPrompt();

#endif
