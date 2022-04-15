#ifndef __TEXTURE_H
#define __TEXTURE_H

struct Texture {
	GLuint id;
	GLenum target;
	int width, height, depth;
};

#endif