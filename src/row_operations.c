// screen/file row operations

// includes
#include "main.h"
#include "row_operations.h"

// add new row at 'at' position of current file
void editorInsertRow(int at, char* s, size_t len, struct editorConfig* E){
	if(at < 0 || at > E->filerows) return;
	
	// allocate memory for +1 row
	E->rows = realloc(E->rows, sizeof(erow) * (E->filerows + 2));
	memmove(&E->rows[at+1], &E->rows[at], sizeof(erow) * (E->filerows - at)); 
  for(int i = at + 1; i < E->filerows; i++) E->rows[i].idx++;

  E->rows[at].idx = at;

	// copy new row
	E->rows[at].len = len;
	E->rows[at].str = malloc(len + 1);
	memcpy(E->rows[at].str, s, len);
	E->rows[at].str[len] = '\0';
	
	// set up render values
	E->rows[at].rlen = 0;
	E->rows[at].rstr = NULL;
  E->rows[at].highlight = NULL;
  E->rows[at].hl_open_comment = 0;

	// update row render
	editorUpdateRow(&(E->rows[at]), E);
	
	// increment total row number
	E->filerows++;
	E->saved = 0;
}

// set up row render string
void editorUpdateRow(erow* row, struct editorConfig* E){
	
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
  
  // update row syntax highlight
  editorUpdateSyntax(row, E);
}

// convert cursor raw row position to cursor render row position
int editorCxToRx(erow* row, int cx){
	int rx = 0;
	for(int i = 0; i < cx; i++){
		if(row->str[i] == '\t') rx += TAB_SIZE-1;
		rx++;
	}
	return rx;
}

// convert cursor render row position to cursor raw row position
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

// insert new char at 'at' position of row 'row'
void editorRowInsertChar(erow* row, int at, char c, struct editorConfig* E){
	if(at < 0 || at > row->len) at = row->len;
	
	// allocate memory for new char
	row->str = realloc(row->str, row->len + 2);
	
	// move all chars after 'at' to the right
	memmove(&row->str[at+1], &row->str[at], row->len - at + 1);
	row->len++;
	row->str[at] = c;
	
	editorUpdateRow(row, E);
	E->saved = 0;
}

// delete char at 'at' position of row 'row'
void editorRowDelChar(erow* row, int at, struct editorConfig* E){
	if(at < 0 || at > row->len) return;	
	// move chars after 'at' to the left
	memmove(&row->str[at], &row->str[at+1], row->len - at);
	// remove extra memory
	row->str = realloc(row->str, row->len-1);
	row->len--;
	editorUpdateRow(row, E);
	E->saved = 0;
}

// free memory allocated for a row
void editorFreeRow(erow* row){
	free(row->str);
	free(row->rstr);
  	free(row->highlight);
}

// delete row at 'at' postion of current file
void editorDelRow(int at, struct editorConfig* E){
	if(at < 0 || at >= E->filerows) return;

	editorFreeRow(&E->rows[at]);
	
	if(at != E->filerows-1)
		memmove(&E->rows[at], &E->rows[at+1], sizeof(erow) * (E->filerows - at + 1));
  
  for(int i = at; i < E->filerows - 1; i++) E->rows[i].idx--;
	
  E->filerows--;
	E->saved = 0;
}

// append string to end of row
void editorRowAppendString(erow* row, char*s, size_t len, struct editorConfig* E){
	// allocate memory for appended string
	row->str = realloc(row->str, row->len + len + 2);
	
	// copy string to end of row
	memcpy(&row->str[row->len], s, len);
	row->len += len;
	row->str[row->len] = '\0';
	
	editorUpdateRow(row, E);
	E->saved = 0;
}

// set indentation to a row based on previous row
void editorRowSetIndentation(erow* row, struct editorConfig* E){
	if(row->idx == 0) return;

	int i = 0;
	erow* prevrow = &E->rows[row->idx-1];
	while(isspace(prevrow->str[i])){
		editorRowInsertChar(row, i, prevrow->str[i], E);
		i++;
	}
}

// update row syntax highlighting
void editorUpdateSyntax(erow* row, struct editorConfig* E){
  row->highlight = malloc(row->rlen);
  memset(row->highlight, HL_NORMAL, row->rlen);

  if(E->syntax == NULL) return;

  // previous char was separator char
  int prev_sep = 1;
  // char is inside a string
  int in_string = 0;
  // char is inside a comment
  int in_comment = (row->idx > 0 && E->rows[row->idx - 1].hl_open_comment);

  char* scs = E->syntax->singleline_comment_start;
  char* mcs = E->syntax->multiline_comment_start;
  char* mce = E->syntax->multiline_comment_end;

  int scs_len = scs ? strlen(scs) : 0;
  int mcs_len = mcs ? strlen(mcs) : 0;
  int mce_len = mce ? strlen(mce) : 0;

  char** keywords = E->syntax->keywords;

  int i = 0;
  while(i < row->rlen){
    char c = row->rstr[i];
    unsigned char prev_hl = (i > 0) ? row->highlight[i - 1] : HL_NORMAL;

    // highlight single line comments
    if(scs_len && !in_string && !in_comment){
      if(!strncmp(&row->rstr[i], scs, scs_len)){
        memset(&row->highlight[i], HL_COMMENT, row->rlen - i);
        break;
      } 
    }

   // highlight multiline comments
   if(mcs && mce && !in_string){
      if(in_comment){
        row->highlight[i] = HL_MLCOMMENT;
        if(!strncmp(&row->rstr[i], mce, mce_len)){
          memset(&row->highlight[i], HL_MLCOMMENT, mce_len);
          prev_sep = 1;
          in_comment = 0;
          i += mce_len;
          continue;
        }
        i++;
        continue;
      } else {
        if (!strncmp(&row->rstr[i], mcs, mcs_len)){
          memset(&row->highlight[i], HL_MLCOMMENT, mcs_len);
          in_comment = 1;
          i += mcs_len;
          continue;
        }
      }
    }

   // highlight strings
   if(E->syntax->flags & HL_HIGHLIGHT_STRINGS){
      if(in_string){
        row->highlight[i] = HL_STRING;
        if(c == '\\' && i + 1 < row->rlen){
          row->highlight[i+1] = HL_STRING;
          i+=2;
          continue;
        }

        if(c == '"' || c == '\'') in_string = 0; 
        i++;
        prev_sep = 1;
        continue;
      } else {
        if(c == '"' || c == '\''){
          in_string = 1;
          row->highlight[i] = HL_STRING;
          i++;
          continue;
        }
      }
    } 

    // highlight digits
    if(E->syntax->flags & HL_HIGHLIGHT_NUMBERS){
      if((isdigit(c) && (prev_sep || prev_hl == HL_NUMBER)) || (c == '.' && prev_hl == HL_NUMBER)){
        row->highlight[i] = HL_NUMBER;
        i++;
        prev_sep = 0;
        continue;
      }
    }

    // highlight special keywords
    if(prev_sep){
      int j;
      for(j = 0; keywords[j]; j++){
        int klen = strlen(keywords[j]);
        int kw2 = keywords[j][klen-1] == '|';
        if (kw2) klen--;
        
        if(!strncmp(keywords[j], &row->rstr[i], klen) && is_separator(row->rstr[i + klen])){
          memset(&row->highlight[i], kw2 ? HL_KEYWORD2 : HL_KEYWORD1, klen);
          i += klen;
          break;
        }
      }
      if(keywords[j] != NULL){
        prev_sep = 0;
        continue;
      }
    }

    prev_sep = is_separator(c);
    i++;
  }

  int changed = (row->hl_open_comment != in_comment);
  row->hl_open_comment = in_comment;
  if(changed && row->idx < E->filerows - 1){
    editorUpdateSyntax(&E->rows[row->idx + 1], E);
  }
}

int editorSyntaxToColor(int hl){
  switch(hl){
    case HL_STRING: return 35;
    case HL_NUMBER: return 31;
    case HL_MATCH: return 34;
    case HL_MLCOMMENT:
    case HL_COMMENT: return 36;
    case HL_KEYWORD1: return 33;
    case HL_KEYWORD2: return 32;
    default: return 37;
  }
}

int is_separator(int c){
  return isspace(c) || c == '\0' || strchr(",.()+-/~%<>[];", c) != NULL;
}
