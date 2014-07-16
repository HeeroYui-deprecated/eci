/**
 * @author Edouard DUPIN
 * 
 * @copyright 2014, Edouard DUPIN, all right reserved
 * 
 * @license APACHE-2 (see license file)
 */

#ifndef __ECI_TABLE_H__
#define __ECI_TABLE_H__


void TableInit();
char *TableStrRegister(const char *_str);
char *TableStrRegister2(const char *_str, int Len);
void TableInitTable(struct Table *_table, struct TableEntry **_hashTable, int _size, int _onHeap);
int TableSet(struct Table *_tbl, char *_key, struct Value *_val, const char *_declFileName, int _declLine, int _declColumn);
int TableGet(struct Table *_tbl, const char *_key, struct Value **_val, const char **_declFileName, int *_declLine, int *_declColumn);
struct Value *TableDelete(struct Table *_tbl, const char *_key);
char *TableSetIdentifier(struct Table *_tbl, const char *_ident, int _identLen);
void TableStrFree();

#endif
