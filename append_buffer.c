// append buffer operations

// includes
#include "main.h"
#include "append_buffer.h"

void abAppend(struct abuf* ab, char* s, int len){
	// allocate memory for new string
	ab->b = realloc(ab->b, ab->len + len);
	
	// add new string
	memcpy(&ab->b[ab->len], s, len);
	ab->len += len;
}