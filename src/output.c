// create output to be displayed on terminal

// includes
#include "main.h"
#include "append_buffer.h"
#include "output.h"
#include "row_operations.h"

// draw on terminal
void editorRefreshScreen(struct abuf* ab, struct editorConfig* E){
	// set row and column offsets
	editorScroll(E);
	
	// reset buffer
	ab->len = 0;
	ab->b = NULL;
	
	// hide cursor
	abAppend(ab, "\x1b[?25l", 6);
	
	// place cursor at top of screen
	abAppend(ab, "\x1b[H", 3);
	
	// draw
	editorDrawRows(ab, E);
	editorDrawStatusBar(ab, E);
	editorDrawMessageBar(ab, E);
	
	// place cursor in correct place
	char buff[100];
	int len = snprintf(buff, sizeof(buff), "\x1b[%d;%dH", E->cy + 1 - E->rowoffset, E->rx + 1 - E->coloffset);
	abAppend(ab, buff, len);
	
	// show cursor
	abAppend(ab, "\x1b[?25h", 6);
	
	// print
	write(STDIN_FILENO, ab->b, ab->len);
}

// draw text rows
void editorDrawRows(struct abuf* ab, struct editorConfig* E){
	for(int i = 0; i < E->screenrows; i++){
		if(i + E->rowoffset >= E->filerows){
			if(E->filerows == 0 && i == E->screenrows / 3){
				// print text editor version
				// only if no file was opened
				char welcome[100];
				int welcomelen = snprintf(welcome, sizeof(welcome), "Text Editor Version -- %s", VERSION);
				if(welcomelen > E->screencols) welcomelen = E->screencols;
				
				// center text
				int padding = (E->screencols - welcomelen) / 2;
				if(padding){
					abAppend(ab, "~", 1);
					padding--;
				}
				while(padding--) abAppend(ab, " ", 1);
				abAppend(ab, welcome, welcomelen);
			}
			else {
				// print '~' in empty lines
				abAppend(ab, "~", 1);
			}
		} else {
			// print contents from file
			int len = E->rows[i+E->rowoffset].rlen - E->coloffset;
			if(len < 0) len = 0;
			if(len > E->screencols) len = E->screencols;
			abAppend(ab, &E->rows[i+E->rowoffset].rstr[E->coloffset], len);
		}

		// clear line
		abAppend(ab, "\x1b[K", 3);
		// change line
		abAppend(ab, "\r\n", 2);
	}
}

// set column and row offsets
void editorScroll(struct editorConfig* E){
	E->rx = 0;
	
  // get cursor position on rendered row
  if(E->cy < E->filerows) E->rx = editorCxToRx(&E->rows[E->cy], E->cx);
	else E->cx = 0;
	
	if(E->rx >= E->coloffset + E->screencols){
		E->coloffset = E->rx - E->screencols;
	} else if (E->rx < E->coloffset){
		E->coloffset = E->rx;
	} else if (E->cy >= E->rowoffset + E->screenrows){
		E->rowoffset = E->cy - E->screenrows;
	} else if (E->cy < E->rowoffset){
		E->rowoffset = E->cy;
	}
}

// draw file status bar
void editorDrawStatusBar(struct abuf* ab, struct editorConfig* E){
	// invert screen colors
	abAppend(ab, "\x1b[7m", 4);
	
	char status1[100], status2[100];
	// show file name and number of lines
	int len1 = snprintf(status1, sizeof(status1), "%.20s - %d lines %s", (E->filename) ? E->filename : "[No Name]", E->filerows, (E->saved) ? "" : "(modified)");       
	// show current line number
	int len2 = snprintf(status2, sizeof(status2), "%d/%d", E->cy+1, E->filerows);
	
	// place status1 and status2 on opposite ends of the row
	abAppend(ab, status1, len1);
	while(len1 + len2 != E->screencols && len1 < E->screencols){
		abAppend(ab, " ", 1);
		len1++;
	}
	abAppend(ab, status2, len2);
	
	// revert screen colors to normal
	abAppend(ab, "\x1b[m", 3);
	abAppend(ab, "\r\n", 2);
}

// set status msg
void editorSetStatusMessage(struct editorConfig* E, const char* fmt, ...){
	// set status msg
	va_list ap;
	va_start(ap, fmt);
	vsnprintf(E->statusmsg, sizeof(E->statusmsg), fmt, ap);
	va_end(ap);
	
	// save status creation time
	E->statusmsg_time = time(NULL);
}

// draw message bar
void editorDrawMessageBar(struct abuf* ab, struct editorConfig* E){
	// clear line
	abAppend(ab, "\x1b[K", 3);
	
	int len = strlen(E->statusmsg);
	if(len > E->screencols) len = E->screencols;

  // messages are only displayed for 5 seconds
	if(len && time(NULL) - E->statusmsg_time < 5){
		abAppend(ab, E->statusmsg, len);
	}
}
