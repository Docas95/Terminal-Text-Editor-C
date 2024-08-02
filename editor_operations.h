#ifndef EDITOR_OPERATIONS_H
#define EDITOR_OPERATIONS_H

void editorInsertChar(char c, struct editorConfig* E);
void editorDelChar(struct editorConfig* E);
void editorInsertNewLine(struct editorConfig* E);
#endif