// text editing operations

// includes
#include "main.h"
#include "editor_operations.h"
#include "row_operations.h"

// insert char at cursor location
void editorInsertChar(char c, struct editorConfig* E){
	// create new row if needed
	if(E->cy == E->filerows) editorInsertRow(E->filerows, "", 0, E);
	editorRowInsertChar(&E->rows[E->cy], E->cx, c, E);
	E->cx++;
}

// delete char at cursor location
void editorDelChar(struct editorConfig* E){
	if(E->cy == E->filerows) return;
	if(E->cy == 0 && E->cx == 0) return;
	
	erow* row = &E->rows[E->cy];
	if(E->cx > 0){
		editorRowDelChar(row, E->cx, E);
		E->cx--;
	} else {
		E->cx = E->rows[E->cy - 1].len;
		editorRowAppendString(&E->rows[E->cy-1], row->str, row->len, E);
		editorDelRow(E->cy, E);
		E->cy--;
	}
}

// add new line to current file
void editorInsertNewLine(struct editorConfig* E){
	if(E->cx == 0){
		editorInsertRow(E->cy, "", 0, E);
	} else {
		erow* row = &E->rows[E->cy];
		editorInsertRow(E->cy + 1, &row->str[E->cx], row->len - E->cx, E);
		row = &E->rows[E->cy];
		row->len = E->cx;
		row->str[row->len] = '\0';
		editorUpdateRow(row);
	}
	E->cy++;
	E->cx = 0;
}
