#ifndef __BUFFER_H
#define __BUFFER_H

#include "glbt.h"

struct Buffer {
	GLuint id;
	int length; //length in floats, not size in bytes.
};

#endif