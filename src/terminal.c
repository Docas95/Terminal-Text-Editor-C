// terminal operations

// includes
#include "main.h"
#include "terminal.h"

// global variables
struct termios orig_termios; // original terminal configuration

// put terminal in raw input mode
void enableRawMode()
{
	// get terminal attributes
	if(tcgetattr(STDIN_FILENO, &orig_termios) == 1){
		die("tcgetattr");
	}
	
	// disable raw mode on program exit
	atexit(disableRawMode);
	
	// activate raw mode
	struct termios raw = orig_termios;
	raw.c_lflag &= ~(ECHO | ICANON | ISIG | IEXTEN); // local flags
	raw.c_iflag &= ~(IXON | ICRNL | BRKINT | INPCK | ISTRIP); // input flags
	raw.c_oflag &= ~(OPOST); // output flags
	raw.c_cflag |= ~(CS8); // control flags
	
  // make read() return even without receiving input
	raw.c_cc[VMIN] = 0; // min input for read() to return
	raw.c_cc[VTIME] = 1; // min time for read() to return

  // update terminal attributes
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw) == 1)
		die("tcsetattr");
}

// put terminal in cooked input mode
void disableRawMode(){
	if(tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios) == 1)
		die("tcsetattr");
}

void die(const char* s){
	// refresh screen
	write(STDOUT_FILENO, "\x1b[2J", 4); // clear screen
	write(STDOUT_FILENO, "\x1b[H", 3); // place cursor at default position
	
	perror(s);
	exit(1);
}

// wait to read a char from the terminal and return it
int editorReadKey(){
	int nread;
	char c;
	
	// read char from terminal
	while((nread = read(STDIN_FILENO, &c, 1)) != 1){
		if(nread == -1) die("read");
	}
	
	// handle special control characters
	if(c == '\x1b'){
		char seq[3];
		
		if(read(STDIN_FILENO, &seq[0], 1) != 1) return '\x1b';
		if(read(STDIN_FILENO, &seq[1], 1) != 1) return '\x1b';
		
		if(seq[0] == '['){
			if(seq[1] > '0' && seq[1] < '9'){
				if(read(STDIN_FILENO, &seq[2], 1) != 1) return '\x1b';
				if(seq[2] == '~' && seq[1] == '3') return DEL_KEY;
			} else {
				switch(seq[1]){
					case 'A':
						return ARROW_TOP;
					case 'B':
						return ARROW_BOTTOM;
					case 'C':
						return ARROW_RIGHT;
					case 'D':
						return ARROW_LEFT;
				}
			}
		}
		
		return '\x1b';
	}
	
	return c;
}

// get size of terminal window
int getWindowSize(int* rows, int* cols){
	struct winsize ws;
	
	// get window size
	if(ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws) == -1 || ws.ws_col == 0 || ws.ws_row == 0){
		// in case ioctl() does not work (older terminals, etc)	
		// place cursor at bottom right of screen and get its position
		if(write(STDOUT_FILENO, "\x1b[999B\x1b[999C", 12) != 12) return -1;
		return getCursorPosition(rows, cols);
	}
	else{
		*rows = ws.ws_row;
		*cols = ws.ws_col;
	}
	
	return 0;
}

// gets cursor position, places x coord in cols and y coord in rows
// returns 1 if it fails
int getCursorPosition(int* rows, int* cols){
	char buff[32];
	
	// get cursor position
	if(write(STDOUT_FILENO, "\x1b[6n", 4) != 4) return -1;
	
	// response format from terminal "\x1b[row;colR"
	unsigned int i;
	for(i = 0; i < sizeof(buff); i++){
		if(read(STDIN_FILENO, &buff[i], 1) != 1) return -1;
		if(buff[i] == 'R') break;
	}
	buff[i] = '\0';
	if(buff[0] != '\x1b' || buff[1] != '[') return -1;
	if(sscanf(&buff[2], "%d;%d", rows, cols) != 2) return -1;
	return 0;
}
