#include "glbt.h"
#include "internal/render.h"
#include "internal/texture.h"

struct RenderTarget* create_render_target(int color_count, struct Texture** color, struct Texture* depth) {
	struct RenderTarget* rt = malloc(sizeof(struct RenderTarget));
	rt->first = NULL;

	glGenFramebuffers(1, &rt->id);
	glBindFramebuffer(GL_FRAMEBUFFER, rt->id);
	rt->color = malloc(sizeof(struct Texture*) * color_count);
	int i;
	for (i = 0; i < color_count; ++i) {
 		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0+i, color[i]->target, color[i]->id, 0);
		rt->color[i] = color[i];
		if(!rt->first) {
			rt->first = rt->color[i];
		}
	}
	if (depth) {
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth->target, depth->id, 0);
		rt->depth = depth;
		if(!rt->first) {
			rt->first = rt->depth;
		}
	}
	GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
	if (status != GL_FRAMEBUFFER_COMPLETE) {
		fprintf(stderr, "Framebuffer is not complete: %d\n", status);
	}

	//create implicit resolve framebuffer if needed
	if(rt->first->target == GL_TEXTURE_2D_MULTISAMPLE) {
		glGenFramebuffers(1, &rt->resolve_id);
		glBindFramebuffer(GL_FRAMEBUFFER, rt->resolve_id);
		rt->resolve_image = create_texture_2d(NULL, rt->first->width, rt->first->height, (rt->first->flags & FILTERED) | (rt->first->flags & ALPHA));
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, rt->resolve_image->id, 0);
	}

	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	if (check_errors()) {
		fprintf(stderr, "Above errors during framebuffer creation.\n");
	}

	return rt;
}
void destroy_render_target(struct RenderTarget* rt) {
	glDeleteFramebuffers(1, &rt->id);
	if(rt->first->target == GL_TEXTURE_2D_MULTISAMPLE) {
		destroy_texture(rt->resolve_image);
	}
	free(rt->color);
	free(rt);
}

struct RenderTarget *current_pass = NULL;
void begin_pass(struct RenderTarget* rt) {
	glViewport(0, 0, rt->first->width, rt->first->height);
	glBindFramebuffer(GL_FRAMEBUFFER, rt->id);
}
void end_pass(struct RenderTarget* rt) {
	//perform resolve if necessary
	if(rt->first->target == GL_TEXTURE_2D_MULTISAMPLE) {
		glBindFramebuffer(GL_READ_FRAMEBUFFER, rt->id);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, rt->resolve_id);
		glBlitFramebuffer(0, 0, rt->first->width, rt->first->height, 0, 0, rt->resolve_image->width, rt->resolve_image->height, GL_COLOR_BUFFER_BIT, GL_NEAREST);
	}
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

struct Texture* render_texture(struct RenderTarget* rt) {
	if(rt->first->target == GL_TEXTURE_2D_MULTISAMPLE) {
		return rt->resolve_image;
	} else {
		return rt->first;
	}
}

void clear(float r, float g, float b, float a) {
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void copy(struct RenderTarget* src, struct RenderTarget* dst) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->id);
	glBlitFramebuffer(0, 0, src->first->width, src->first->height, 0, 0, dst->first->width, dst->first->height, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
void copy_region(struct RenderTarget* src, struct RenderTarget* dst,
							 int sx, int sy, int sw, int sh,
							 int dx, int dy, int dw, int dh) {
	glBindFramebuffer(GL_READ_FRAMEBUFFER, src->id);
	glBindFramebuffer(GL_DRAW_FRAMEBUFFER, dst->id);
	glBlitFramebuffer(sx, sy, sw, sh, dx, dy, dw, dh, GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT, GL_NEAREST);
}
