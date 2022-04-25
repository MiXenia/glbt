#include "glbt.h"
#include "internal/texture.h"

//this is the same regardless of texture.
void destroy_texture(struct Texture* t) {
	glDeleteTextures(1, &t->id);
	free(t);
}

struct Texture* create_texture_2d(char *data, int width, int height, enum TextureFlags flags) {
	struct Texture* t = malloc(sizeof(struct Texture));
	t->flags = flags;
	t->width = width;
	t->height = height;
	t->depth = 1;

	GLenum format = GL_RGB;
	if (flags & ALPHA) {
		format = GL_RGBA;
	}
	
	glGenTextures(1, &t->id);
	GLuint previously_bound;
	if(flags & ANTIALIASED) {
		glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE, &previously_bound);
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, t->id);
		t->target = GL_TEXTURE_2D_MULTISAMPLE;
		if (flags & DEPTH) {
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_DEPTH_COMPONENT32F, width, height, GL_FALSE);
		}
		else {
			glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 8, GL_RGBA, width, height, GL_FALSE);
		}
		glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, previously_bound);
	}
	else {
		glGetIntegerv(GL_TEXTURE_BINDING_2D, &previously_bound);
		glBindTexture(GL_TEXTURE_2D, t->id);
		t->target = GL_TEXTURE_2D;
		if (flags & DEPTH) {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, data);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
			glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
		}
		else {
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			if (flags & FILTERED) {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			}
			else {
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			}
			glGenerateMipmap(GL_TEXTURE_2D);
		}
		glBindTexture(GL_TEXTURE_2D, previously_bound);
	}

	if (check_errors()) {
		fprintf(stderr, "Above errors during texture creation.\n");
	}

	return t;
}
