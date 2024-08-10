// main program code

// includes
#include "main.h"
#include "append_buffer.h"
#include "terminal.h"
#include "input.h"
#include "output.h"
#include "file_io.h"
#include "row_operations.h"

// global vars
struct editorConfig E; // text editor info
struct abuf ab; // screen content

int main(int argc, char* argv[]){
	// set up inital text edior config
  init_editor();
  
  // put terminal in raw input mode
  enableRawMode();
	
  // open a text file
	if(argc > 1){
		editorOpen(argv[1], &E);
	}
	
	editorSetStatusMessage(&E, "HELP: Ctrl-Q = quit | Ctrl-S = save | Ctrl-F = find | Ctrl - N = auto-indentation toggle");
	
	while(1){
    // draw on terminal
		editorRefreshScreen(&ab, &E);
	  // process user input
    editorProcessKeypress(&E, &ab);
	}
	
	return 0;
}

void init_editor(){
	E.cx = 0;
	E.cy = 0;
	E.rx = 0;
	E.filerows = 0;
	E.filename = NULL;
	E.saved = 1;
	E.rows = NULL;
	E.rowoffset = 0;
	E.coloffset = 0;
	E.statusmsg[0] = '\0';
	E.statusmsg_time = 0;
	E.auto_indent = 1;
  E.syntax = NULL;
	if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowsSize");
	// save space for status bar and message bar
  E.screenrows-=2;
	// save space for line numbering
  E.screencols-=4;
}
