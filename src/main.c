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
	init_editor();
	enableRawMode();
	
	if(argc > 1){
		editorOpen(argv[1], &E);
	}
	
	editorSetStatusMessage(&E, "HELP: Ctrl-Q = quit | Ctrl-S = save | Ctrl-F = Find");
	
	while(1){
		editorRefreshScreen(&ab, &E);
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
	if(getWindowSize(&E.screenrows, &E.screencols) == -1) die("getWindowsSize");
	E.screenrows-=2;
}