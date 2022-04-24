#ifndef __FRAMEBUFFER_H
#define __FRAMEBUFFER_H

#include "internal/texture.h"

struct Framebuffer {
	GLuint id;
	struct Texture* color;
	struct Texture depth;
};

#endif
