#ifndef TERMINAL_H
#define TERMINAL_H

void enableRawMode();
void disableRawMode();
void die(const char* s);
int editorReadKey();
int getWindowSize(int* rows, int* cols);
int getCursorPosition(int* rows, int* cols);

#endif