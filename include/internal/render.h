#ifndef __RENDER_H
#define __RENDER_H

#include "internal/texture.h"

struct RenderTarget {
	GLuint id;
	struct Texture** color;
	struct Texture* depth;
	//for implicit multisample resolve
	GLuint resolve_id;
	struct Texture* resolve_image;
	//for metadata, could be color or depth
	struct Texture* first;
};

#endif
