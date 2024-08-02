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

typedef struct erow{
	char* str;
	char* rstr;
	int len;
	int rlen;
}erow;

struct editorConfig{
	int cx;
	int cy;
	int rx;
	int screenrows;
	int screencols;
	int rowoffset;
	int coloffset;
	
	char statusmsg[100];
	time_t statusmsg_time;
	
	char* filename;
	int filerows;
	erow* rows;
	int saved;
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