#ifndef MAIN_H
#define MAIN_H

#define _DEFAULT_SOURCE
#define _BSD_SOURCE
#define _GNU_SOURCE

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <string.h>
#include <time.h>
#include <stdarg.h>
#include <fcntl.h>

#define CTRL_KEY(k) ((k) & 0x1f) 
#define VERSION "1.0"
#define TAB_SIZE 4
#define QUIT_TIMES 3
#define HL_HIGHLIGHT_NUMBERS (1<<0)
#define HL_HIGHLIGHT_STRINGS (1<<1)
#define HLDB_ENTRIES (sizeof(HLDB) / sizeof(HLDB[0]))

struct editorSyntax{
  char* filetype;
  char** filematch;
  char** keywords;
  char* singleline_comment_start;
  char* multiline_comment_start;
  char* multiline_comment_end;
  int flags;
  int hl_open_comment;
};

// file row
typedef struct erow{
  int idx;
  char* str;
  char* rstr;
  char* highlight;
  int len;
  int rlen;
  int hl_open_comment;
}erow;

struct editorConfig{
	int cx, cy; // cursor position
	int rx; // cursor position on rendered string
	int screenrows;
	int screencols;
	int rowoffset;
	int coloffset;
	
	char statusmsg[100]; // status msg content
	time_t statusmsg_time;  // time message was sent
	
	int auto_indent; // set to 0 if auto indentation is turned off 
		
	char* filename; // name of opened file
	int filerows; // number of rows inside file 
	erow* rows; // array of file rows 
	int saved; // set to 0 if changes have been made without saving file
  struct editorSyntax* syntax; // current file syntax settings
};

enum editorKeys{
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_BOTTOM,
	ARROW_TOP,
	DEL_KEY
};

enum editorHighlight{
  HL_NORMAL = 0,
  HL_NUMBER,
  HL_MATCH,
  HL_STRING,
  HL_COMMENT,
  HL_MLCOMMENT,
  HL_KEYWORD1,
  HL_KEYWORD2
};

void init_editor();

#endif
