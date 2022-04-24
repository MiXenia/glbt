#include "glbt.h"
#include "internal/framebuffer.h"
#include "internal/texture.h"

struct Framebuffer* create_framebuffer(int color_count, struct Texture** color, struct Texture* depth) {
	struct Framebuffer* f = malloc(sizeof(struct Framebuffer));

	glGenFramebuffers(1, &f->id);
	glBindFramebuffer(GL_FRAMEBUFFER, f->id);
	f->color = malloc(sizeof(struct Texture) * color_count);
	int i;
	for (i = 0; i < color_count; ++i) {
 		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, color[i]->target, color[i]->id, 0);
		f->color[i] = *color[i];
	}
	if (depth) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->target, depth->id, 0);
		f->depth = *depth;
	}
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
	free(f->color);
	free(f);
}

void bind_framebuffer(struct Framebuffer* f) {
	glViewport(0, 0, f->color[0].width, f->color[0].height);
	glBindFramebuffer(GL_FRAMEBUFFER, f->id);
}

void clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void copy_framebuffer(struct Framebuffer* src, struct Framebuffer* dst) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->id);
	glBlitFramebuffer(0, 0, src->color[0].width, src->color[0].height, 0, 0, dst->color[0].width, dst->color[0].height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
void copy_framebuffer_region(struct Framebuffer* src, struct Framebuffer* dst,
							 int sx, int sy, int sw, int sh,
							 int dx, int dy, int dw, int dh) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->id);
	glBlitFramebuffer(sx, sy, sw, sh, dx, dy, dw, dh, GL_COLOR_BUFFER_BIT, GL_NEAREST);
}
