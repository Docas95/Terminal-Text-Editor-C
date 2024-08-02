#ifndef FILE_IO_H
#define FILE_IO_H

void editorOpen(char* filename, struct editorConfig* E);
char* editorRowsToString(int* buflen, struct editorConfig* E);
void editorSave(struct editorConfig* E, struct abuf* ab);


#endif