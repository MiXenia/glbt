#ifndef __TEXTURE_H
#define __TEXTURE_H

#include "glbt.h"

struct Texture {
	GLuint id;
	GLenum target;
	enum TextureFlags flags;
	int width, height, depth;
};

#endif
