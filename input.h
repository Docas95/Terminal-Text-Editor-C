#ifndef INPUT_H
#define INPUT_H

void editorProcessKeypress(struct editorConfig* E, struct abuf* ab);
void editorMoveCursor(int key, struct editorConfig* E);
char* editorPrompt(char* prompt, struct abuf* ab, struct editorConfig* E);

#endif