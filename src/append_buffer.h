#ifndef APPEND_BUFFER_H
#define APPEND_BUFFER_H

struct abuf{
	char* b;
	int len;
};

void abAppend(struct abuf* ab, char* s, int len);

#endif
