#ifndef OUTPUT_H
#define OUTPUT_H

void editorRefreshScreen(struct abuf* ab, struct editorConfig* E);
void editorDrawRows(struct abuf* ab, struct editorConfig* E);
void editorScroll(struct editorConfig* E);
void editorDrawStatusBar(struct abuf* ab, struct editorConfig* E);
void editorSetStatusMessage(struct editorConfig* E, const char* fmt, ...);
void editorDrawMessageBar(struct abuf* ab, struct editorConfig*E);
#endif