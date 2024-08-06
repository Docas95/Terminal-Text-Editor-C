// file input/output operations

// includes
#include "main.h"
#include "append_buffer.h"
#include "file_io.h"
#include "terminal.h"
#include "row_operations.h"
#include "output.h"
#include "input.h"

// filetypes
char* C_HL_extensions[] = {".c", ".h", ".cpp", NULL};
char* C_HL_keywords[] = {
  "switch", "if", "else", "while", "for", "break", "break", "continue",
  "return", "struct", "union", "typedef", "static", "enum", "class", "case",

  "int|", "long|", "short|", "char|", "float|", "double|", "void|", "signed|"
  "unsigned|", NULL
};

struct editorSyntax HLDB[] = {
  { 
    "c",
    C_HL_extensions,
    C_HL_keywords,
    "//","/*", "*/",
    HL_HIGHLIGHT_NUMBERS | HL_HIGHLIGHT_STRINGS
  },
};

// open new file, if file can't be found an empty file will be opened
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
	
  editorSelectSyntaxHighlight(E);

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

// convert editor rows to a singular string
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

// save editor information to a file
void editorSave(struct editorConfig* E, struct abuf* ab){
	if(E->filename == NULL){
		E->filename = editorPrompt("Save as (ESC to cancel):", NULL,ab, E);
		if(E->filename == NULL){
			editorSetStatusMessage(E, "Save aborted");
			return;
		}
	}

  editorSelectSyntaxHighlight(E);

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

// find a word in current file
void editorFind(struct editorConfig* E, struct abuf* ab){
	char* query = editorPrompt("Search (ESC to cancel):", editorFindCallBack ,ab, E);

	if(query)
		free(query);
}
void editorFindCallBack(char* query, int key, struct editorConfig* E){
	static int last_match = -1;
	static int direction = 1;

  static int saved_hl_line;
  static char* saved_hl = NULL;

  // restore original row highlight for previous match
  if (saved_hl){
    memcpy(E->rows[saved_hl_line].highlight, saved_hl, E->rows[saved_hl_line].rlen);
    free(saved_hl);
    saved_hl = NULL;
  }

  // reset variables when search is over
	if(key == '\r' || key == '\x1b'){
		last_match = -1;
		direction = 1;
		return;
  // changed search direction with arrow keys  
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
	
  // search through all rows until query is found
  for(int i = 0; i < E->filerows; i++){
		current += direction;

    // keep search within file boundaries
		if(current == -1) current = E->filerows - 1;
		else if(current == E->filerows) current = 0;

		erow* row = &E->rows[current];
		char* match = strstr(row->rstr, query);
		
    // end loop when query is found
    // place row at top of sreen
    // place cursor at query match location
    if(match){
			last_match = current;
			E->cy = current;
			E->cx = editorRxToCx(row, match - row->rstr);
			E->rowoffset = E->filerows;
      
      // saved row highlights
      saved_hl_line = E->cy;
      saved_hl = malloc(row->rlen);
      memcpy(saved_hl, row->highlight, row->rlen);
       
      // highlight match
      memset(&row->highlight[match - row->rstr], HL_MATCH, strlen(query));
			break;
		}
	}
}

// select highlight based on file type
void editorSelectSyntaxHighlight(struct editorConfig* E){
  // unsaved files have no highlight by default
  E->syntax == NULL;
  if(E->filename == NULL) return;

  // get file extension
  char* ext = strchr(E->filename, '.');
  
  // iterate through all available file syntaxes
  for(unsigned int i = 0; i < HLDB_ENTRIES; i++){
    struct editorSyntax* s = &HLDB[i]; 
    unsigned int j = 0;
    while(s->filematch[j]){
      
      // check if filetype is an extension
      int is_ext = (s->filematch[j][0] == '.');

      // compare file extension or file name
      if((is_ext && ext && !strcmp(s->filematch[j], ext)) || (!is_ext && !strcmp(s->filematch[j], E->filename))){
        E->syntax = s;

        // update syntax of all rows
        for(int k = 0; k < E->filerows; k++){
          editorUpdateSyntax(&E->rows[k], E);
        }
        return;
      }
      j++;
    }
  }  
}
