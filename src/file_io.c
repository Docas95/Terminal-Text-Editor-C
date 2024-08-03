// file input/output operations

// includes
#include "main.h"
#include "append_buffer.h"
#include "file_io.h"
#include "terminal.h"
#include "row_operations.h"
#include "output.h"
#include "input.h"

void editorOpen(char* filename, struct editorConfig* E){
	free(E->filename);
	E->filename = strdup(filename);
	
	// open file
	FILE *fp = fopen(filename, "r");
	// open empty editor if file isnt found
	if(!fp){
		free(E->filename);
		E->filename = NULL;
		return;
	}
	
	char* line = NULL;
	size_t linecap = 0;
	ssize_t linelen;
	
	// copy each line, ignoring \n and \r
	while((linelen = getline(&line, &linecap, fp)) != -1){
		while(linelen > 0 && (line[linelen - 1] == '\n' || line[linelen - 1] == '\r')){
			linelen--;
		}
		
		// append line
		editorInsertRow(E->filerows, line, linelen, E);
	}
	
	free(line);
	fclose(fp);
	E->saved = 1;
}

char* editorRowsToString(int* buflen, struct editorConfig* E){
	// allocate memory for all rows
	*buflen = 0;
	for(int i = 0; i < E->filerows; i++)
		*buflen += E->rows[i].len + 1;
	
	char* buf = malloc(*buflen);
	if(buf == NULL) return NULL;
	
	// copy all rows into return var
	char* tmp = buf;
	for(int i = 0; i < E->filerows; i++){
		memcpy(tmp, E->rows[i].str, E->rows[i].len);
		tmp += E->rows[i].len;
		*tmp = '\n';
		tmp++;
	}
	
	return buf;
}

void editorSave(struct editorConfig* E, struct abuf* ab){
	if(E->filename == NULL){
		E->filename = editorPrompt("Save as (ESC to cancel):", NULL,ab, E);
		if(E->filename == NULL){
			editorSetStatusMessage(E, "Save aborted");
			return;
		}
	}
	
	// convert editor rows into a string
	int len;
	char* buf = editorRowsToString(&len, E);
	
	// open or create file and write information
	int f = open(E->filename, O_RDWR | O_CREAT, 0644);
	if(f != -1){
		if(ftruncate(f, len) != -1){
			if(write(f, buf, len) == len){
				close(f);
				free(buf);
				editorSetStatusMessage(E, "%d bytes written to disk", len);
				E->saved = 1;
				return;
			}
		}
		close(f);
	}
	free(buf);
	editorSetStatusMessage(E, "Can't save! I/O error: %s", strerror(errno));
}

void editorFind(struct editorConfig* E, struct abuf* ab){
	char* query = editorPrompt("Search (ESC to cancel):", editorFindCallBack ,ab, E);

	if(query)
		free(query);
}

void editorFindCallBack(char* query, int key, struct editorConfig* E){
	static int last_match = -1;
	static int direction = 1;

	if(key == '\r' || key == '\x1b'){
		last_match = -1;
		direction = 1;
		return;
	} else if (key == ARROW_RIGHT || key == ARROW_BOTTOM){
		direction = 1;
	} else if (key == ARROW_LEFT || key == ARROW_TOP){
		direction = -1;
	} else {
		last_match = -1;
		direction = 1;
	}

	if(last_match == -1) direction = 1;
	int current = last_match;
	for(int i = 0; i < E->filerows; i++){
		current += direction;
		if(current == -1) current = E->filerows - 1;
		else if(current == E->filerows) current = 0;

		erow* row = &E->rows[current];
		char* match = strstr(row->str, query);
		if(match){
			last_match = current;
			E->cy = current;
			E->cx = editorRxToCx(row, match - row->rstr);
			E->rowoffset = E->filerows;
			break;
		}
	}
}
