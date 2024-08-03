// screen/file row operations

// includes
#include "main.h"
#include "row_operations.h"

void editorInsertRow(int at, char* s, size_t len, struct editorConfig* E){
	if(at < 0 || at > E->filerows) return;
	
	// allocate memory for +1 row
	E->rows = realloc(E->rows, sizeof(erow) * (E->filerows + 1));
	memmove(&E->rows[at+1], &E->rows[at], sizeof(erow) * (E->filerows - at)); 
	
	// copy new row
	E->rows[at].len = len;
	E->rows[at].str = malloc(len + 1);
	memcpy(E->rows[at].str, s, len);
	E->rows[at].str[len] = '\0';
	
	// set up render values
	E->rows[at].rlen = 0;
	E->rows[at].rstr = NULL;
	
	// update row render
	editorUpdateRow(&(E->rows[at]));
	
	// increment total row number
	E->filerows++;
	E->saved = 0;
}

void editorUpdateRow(erow* row){
	
	// count special characters
	// tabs
	int ntabs = 0;
	for(int i = 0; i < row->len; i++)
		if(row->str[i] == '\t') ntabs++;
	
	// allocate memory for row render 
	free(row->rstr);
	row->rstr = malloc(row->len + ntabs * (TAB_SIZE - 1) + 1);
	
	int index = 0;
	for(int i = 0; i < row->len; i++){
		if(row->str[i] == '\t'){
			for(int j = 0; j < TAB_SIZE; j++)
				row->rstr[index++] = ' ';
		} else {
			row->rstr[index++] = row->str[i];
		}
	}
	row->rstr[index] = '\0';
	row->rlen = index;
}

int editorCxToRx(erow* row, int cx){
	int rx = 0;
	for(int i = 0; i < cx; i++){
		if(row->str[i] == '\t') rx += TAB_SIZE-1;
		rx++;
	}
	return rx;
}

int editorRxToCx(erow* row, int rx){
	int cur_rx = 0;
	int cx;

	for(cx = 0; cx < row->len; cx++){
		if(row->str[cx] == '\t')
			cur_rx += TAB_SIZE - 1;
		cur_rx++;
		if(cur_rx > rx) return cx;
	}
	return cx;
}

void editorRowInsertChar(erow* row, int at, char c, struct editorConfig* E){
	if(at < 0 || at > row->len) at = row->len;
	
	// allocate memory for new char
	row->str = realloc(row->str, row->len + 2);
	
	// move all chars after 'at' to the right
	memmove(&row->str[at+1], &row->str[at], row->len - at + 1);
	row->len++;
	row->str[at] = c;
	
	editorUpdateRow(row);
	E->saved = 0;
}

void editorRowDelChar(erow* row, int at, struct editorConfig* E){
	if(at < 0 || at > row->len) return;	
	// move chars after 'at' to the left
	memmove(&row->str[at], &row->str[at+1], row->len - at);
	// remove extra memory
	row->str = realloc(row->str, row->len-1);
	row->len--;
	editorUpdateRow(row);
	E->saved = 0;
}

void editorFreeRow(erow* row){
	free(row->str);
	free(row->rstr);
}

void editorDelRow(int at, struct editorConfig* E){
	if(at < 0 || at >= E->filerows) return;

	editorFreeRow(&E->rows[at]);
	
	if(at != E->filerows-1)
		memmove(&E->rows[at], &E->rows[at+1], sizeof(erow) * (E->filerows - at + 1));
	
	E->filerows--;
	E->saved = 0;
}

void editorRowAppendString(erow* row, char*s, size_t len, struct editorConfig* E){
	// allocate memory for appended string
	row->str = realloc(row->str, row->len + len + 2);
	
	// copy string to end of row
	memcpy(&row->str[row->len], s, len);
	row->len += len;
	row->str[row->len] = '\0';
	
	editorUpdateRow(row);
	E->saved = 0;
}
