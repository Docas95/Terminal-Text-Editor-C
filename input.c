// handle user input

// includes
#include "main.h"
#include "append_buffer.h"
#include "input.h"
#include "terminal.h"
#include "editor_operations.h"
#include "file_io.h"
#include "output.h"

void editorProcessKeypress(struct editorConfig* E, struct abuf* ab){
	static int quit = QUIT_TIMES;
	
	int c = editorReadKey();
	
	switch(c){
		case '\r':
			editorInsertNewLine(E);
			break;
		
		case CTRL_KEY('q'):
			quit--;
			if(!E->saved && quit > 0){
				editorSetStatusMessage(E, "WARNING!! File has unsaved changes. Press Ctrl-Q %d more times to quit.", quit);
				return;
			} else {
				// refresh screen
				write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
				write(STDOUT_FILENO, "\x1b[H", 3); // place cursor at default position
				
				exit(0);
			}
			break;
		
		case CTRL_KEY('s'):
			editorSave(E, ab);
			break;
		
		case BACKSPACE:
		case CTRL_KEY('h'):
		case DEL_KEY:
			if(c == DEL_KEY) editorMoveCursor(ARROW_RIGHT, E);
			editorDelChar(E);
			break;
		
		case ARROW_TOP:
		case ARROW_BOTTOM:
		case ARROW_LEFT:
		case ARROW_RIGHT:
			editorMoveCursor(c, E);
			break;
		
		case CTRL_KEY('l'):
		case '\x1b':
			break;
		
		default:
			editorInsertChar(c, E);
			break;
	}
	
	quit = QUIT_TIMES;
}

void editorMoveCursor(int key, struct editorConfig* E){
	// get current row
	erow* currow = (E->cy >= E->filerows) ? NULL : &E->rows[E->cy];
	
	switch(key){
		case ARROW_TOP:
			if(E->cy!=0) E->cy--;
			break;
		case ARROW_BOTTOM:
			if(E->cy < E->filerows) E->cy++;
			break;
		case ARROW_LEFT:
			if(E->cx != 0) E->cx--;
			// place cursor at the end of previous line
			else if(E->cy > 0){
				E->cy--;
				E->cx = E->rows[E->cy].len;
			}
			break;
		case ARROW_RIGHT:
			if(currow && E->cx < currow->len) E->cx++;
			// place cursor at the start of next line
			else if(currow && E->cy < E->filerows){
				E->cy++;
				E->cx = 0;
			}
			break;
	}
	
	// get new current row
	currow = (E->cy >= E->filerows) ? NULL : &E->rows[E->cy];
	if (currow && E->cx > currow->len) E->cx = currow->len;
}

char* editorPrompt(char* prompt, struct abuf* ab, struct editorConfig* E){
	size_t bufsize = 128;
	char* buf = malloc(bufsize);
	if(buf == NULL) return NULL;
	
	size_t buflen = 0;
	buf[0] = '\0';
	
	while(1){
		editorSetStatusMessage(E, "%s %s" ,prompt, buf);
		editorRefreshScreen(ab, E);
		
		int c = editorReadKey();
		if (c == DEL_KEY || c == CTRL_KEY('h') || c == BACKSPACE){
			if(buflen != 0) buf[--buflen] = '\0';
		} else if(c == '\x1b'){
			editorSetStatusMessage(E, "");
			free(buf);
			return NULL;
		} else if(c == '\r'){
			if(buflen != 0){
				editorSetStatusMessage(E, "");
				return buf;
			}
		} else if (!iscntrl(c) && c < 128) {
			if(buflen == bufsize - 1){
				bufsize *= 2;
				buf = realloc(buf, bufsize);
			}
			buf[buflen++] = c;
			buf[buflen] = '\0';
		}
	}
}