#ifndef ROW_OPERATIONS_H
#define ROW_OPERATIONS_H

void editorInsertRow(int at, char* s, size_t len, struct editorConfig* E);
void editorUpdateRow(erow* row, struct editorConfig* E);
int editorCxToRx(erow* row, int cx);
int editorRxToCx(erow* row, int rx);
void editorRowInsertChar(erow* row, int at, char c, struct editorConfig* E);
void editorRowDelChar(erow* row, int at, struct editorConfig* E);
void editorFreeRow(erow* row);
void editorDelRow(int at, struct editorConfig* E);
void editorRowAppendString(erow* row, char*s, size_t len, struct editorConfig* E); 
void editorUpdateSyntax(erow* row, struct editorConfig* E);
int editorSyntaxToColor(int hl);
int is_separator(int c);
void editorRowSetIndentation(erow* row, struct editorConfig* E);
#endif
