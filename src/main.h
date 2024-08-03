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

// text file row
typedef struct erow{
	char* str; // raw data
	char* rstr; // data to be rendered on screen
	int len;
	int rlen;
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
	
	char* filename; // name of opened file
	int filerows; // number of rows inside file 
	erow* rows; // array of file rows 
	int saved; // set to 0 if changes have been made without saving file
};

enum editorKeys{
	BACKSPACE = 127,
	ARROW_LEFT = 1000,
	ARROW_RIGHT,
	ARROW_BOTTOM,
	ARROW_TOP,
	DEL_KEY
};

void init_editor();

#endif
