#include "glbt.h"
#include "internal/texture.h"

struct Framebuffer {
	GLuint id;
	struct Texture* color[8];
	struct Texture* depth;
};

struct Framebuffer* create_framebuffer(int color_count, struct Texture* color[8], struct Texture* depth) {
	struct Framebuffer* f = malloc(sizeof(struct Framebuffer));

	glGenFramebuffers(1, &f->id);
	glBindFramebuffer(GL_FRAMEBUFFER, f->id);
	int i;
	for (i = 0; i < color_count; ++i) {
 		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, color[i]->target, color[i]->id, 0);
		f->color[i] = color[i];
	}
	if (depth) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->target, depth->id, 0);
	}
	f->depth = depth;
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Framebuffer is not complete: %d\n", status);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (check_errors()) {
		fprintf(stderr, "Above errors during framebuffer creation.\n");
	}
	return f;
}
void destroy_framebuffer(struct Framebuffer* f) {
	glDeleteFramebuffers(1, &f->id);
	free(f);
}
void bind_framebuffer(struct Framebuffer* f) {
	if (f) {
		glBindFramebuffer(GL_FRAMEBUFFER, f->id);
	}
	else {
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	}
}

void copy_framebuffer(struct Framebuffer* src, struct Framebuffer* dst) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->id);
	glBlitFramebuffer(0, 0, src->color[0]->width, src->color[0]->height, 0, 0, dst->color[0]->width, dst->color[0]->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void copy_framebuffer_to_screen(struct Framebuffer* src, int x, int y, int width, int height) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
	glBlitFramebuffer(0, 0, src->color[0]->width, src->color[0]->height, x, y, width, height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}

void clear_screen(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void viewport(int x, int y, int w, int h) {
	glViewport(x, y, w, h);
}