#ifndef ROW_OPERATIONS_H
#define ROW_OPERATIONS_H

void editorInsertRow(int at, char* s, size_t len, struct editorConfig* E);
void editorUpdateRow(erow* row);
int editorCxToRx(erow* row, int cx);
void editorRowInsertChar(erow* row, int at, char c, struct editorConfig* E);
void editorRowDelChar(erow* row, int at, struct editorConfig* E);
void editorFreeRow(erow* row);
void editorDelRow(int at, struct editorConfig* E);
void editorRowAppendString(erow* row, char*s, size_t len, struct editorConfig* E); 
#endif