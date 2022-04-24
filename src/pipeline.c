#include "glbt.h"
#include "internal/buffer.h"
#include "internal/texture.h"
#include <stdio.h>
#include <string.h>


struct FixedFunctionState {
  int depth_test;
  int alpha_blending;
//what kinds of alpha blending...?
  int alpha_src;
  int alpha_dst;
  int culling;
  int render_style;//polygons lines points
};

struct PipelineOutput {
  int color_attachments; //n
  int depth_attachment; //yes/no
  struct Framebuffer *target; //might very well be NULL.
};

struct Pipeline {
  GLuint vao;
  GLuint program_id;
  int vertex_input_size;
  struct VertexInput *vertex_inputs;
  int texture_input_size;
  struct TextureInput *texture_inputs;
  struct Buffer *index_buffer;
  int count;
  int index_count;
  int instance_count;
};

struct Pipeline *create_pipeline(int vs_len, char *vs, int fs_len, char *fs,
                                 int attribute_count, struct VertexInput *v,
                                 int texture_count, struct TextureInput *t) {
  struct Pipeline *p = malloc(sizeof(struct Pipeline));
  p->index_buffer = NULL;
  p->count = 0;
  p->index_count = 0;
  p->instance_count = 0;

  // store input bindings
  p->vertex_input_size = attribute_count;
  p->vertex_inputs = malloc(attribute_count * sizeof(struct VertexInput));
  memcpy(p->vertex_inputs, v, attribute_count * sizeof(struct VertexInput));
  p->texture_input_size = texture_count;
  p->texture_inputs = malloc(texture_count * sizeof(struct TextureInput));
  memcpy(p->texture_inputs, t, texture_count * sizeof(struct TextureInput));

  // gen the VAO & load attributes
  glGenVertexArrays(1, &p->vao);
  glBindVertexArray(p->vao);
  int i, r;
  for (i = 0; i < p->vertex_input_size; ++i) {
    if (p->vertex_inputs[i].stride == 0) {
      p->vertex_inputs[i].stride = p->vertex_inputs[i].rows * p->vertex_inputs[i].size * sizeof(GLfloat);
    }
    for (r = 0; r < p->vertex_inputs[i].rows; r++) {
      glEnableVertexAttribArray(p->vertex_inputs[i].location + r);
      if (p->vertex_inputs[i].rate == INSTANCE) {
        glVertexAttribDivisor(p->vertex_inputs[i].location + r, 1);
      }
    }
  }
  if (check_errors()) {
    fprintf(stderr, "Above errors during pipeline creation.\n");
  }

  // load program.
  int success;
  char error_log[4096];

  GLuint vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vs, &vs_len);
  glCompileShader(vert);

  glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert, 4096, NULL, error_log);
    fprintf(stderr, "Error in vertex shader: %s", error_log);
  }

  GLuint frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &fs, &fs_len);
  glCompileShader(frag);

  glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag, 4096, NULL, error_log);
    fprintf(stderr, "Error in fragment shader: %s\n", error_log);
  }

  p->program_id = glCreateProgram();
  glAttachShader(p->program_id, vert);
  glAttachShader(p->program_id, frag);
  glLinkProgram(p->program_id);

  glGetProgramiv(p->program_id, GL_LINK_STATUS, &success);
  if (!success) {
    glGetProgramInfoLog(p->program_id, 4096, NULL, error_log);
    fprintf(stderr, "Error in shader program: %s\n", error_log);
  }

  glDeleteShader(vert);
  glDeleteShader(frag);

  glUseProgram(p->program_id);

  if (check_errors()) {
    fprintf(stderr, "Above errors building shader program.\n");
  }

  return p;
}
void destroy_pipeline(struct Pipeline *p) {
  glDeleteProgram(p->program_id);
  glDeleteVertexArrays(1, &p->vao);
  free(p);
}

void bind_buffer(struct Pipeline *p, struct Buffer *b, int location) {
  glBindVertexArray(p->vao);
  struct VertexInput *v = NULL;
  int i;
  for (i = 0; i < p->vertex_input_size; ++i) {
    if (p->vertex_inputs[i].location == location) {
      v = &p->vertex_inputs[i];
      break;
    }
  }
  if (!v) {
    fprintf(stderr, "Pipeline does not have an attribute at location %d.\n", location);
    return;
  }
  v->buffer = b;
  for (i = 0; i < v->rows; i++) {
    glBindBuffer(GL_ARRAY_BUFFER, v->buffer->id);
    int offset = v->offset + i * v->size * sizeof(GLfloat);
    // fprintf(stderr, "%s: %d, %d, %d, %d\n", v->name, v->location + i, v->size, v->stride, offset);
    glVertexAttribPointer(v->location + i, v->size, GL_FLOAT, GL_FALSE, v->stride, (void *)(offset));
  }
  if (v->rate == INSTANCE) {
    p->instance_count = b->length / (v->size * v->rows);
  } else {
    p->count = b->length / (v->size * v->rows);
  }
  if (check_errors()) {
    fprintf(stderr, "Above errors while binding buffer location %d.\n", location);
  }
}
void bind_buffer_name(struct Pipeline *p, struct Buffer *b, const char *name) {
  int i, loc = -1;
  for (i = 0; i < p->vertex_input_size; ++i) {
    if (strncmp(p->vertex_inputs[i].name, name, 32) == 0) {
      loc = p->vertex_inputs[i].location;
      break;
    }
  }
  if (loc != -1) {
    bind_buffer(p, b, loc);
  } else {
    fprintf(stderr, "Pipeline does not have an attribute named %s.\n", name);
  }
}
void bind_index_buffer(struct Pipeline *p, struct Buffer *b) {
  glBindVertexArray(p->vao);
  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->id);
  p->index_buffer = b;
  p->index_count = b->length;
}

void bind_texture(struct Pipeline *p, struct Texture *t, int location) {
  struct TextureInput *ti = NULL;
  int i;
  for (i = 0; i < p->texture_input_size; ++i) {
    if (p->texture_inputs[i].location == location) {
      ti = &p->texture_inputs[i];
      break;
    }
  }
  if (!ti) {
    fprintf(stderr, "Pipeline does not have a texture at location %d.\n", location);
    return;
  }
  ti->texture = t;
  glActiveTexture(GL_TEXTURE0 + ti->location);
  glBindTexture(ti->texture->target, ti->texture->id);
}
void bind_texture_name(struct Pipeline *p, struct Texture *t,
                       const char *name) {
  int i, loc = -1;
  for (i = 0; i < p->texture_input_size; ++i) {
    if (strncmp(p->texture_inputs[i].name, name, 32) == 0) {
      loc = p->texture_inputs[i].location;
      break;
    }
  }
  if (loc != -1) {
    bind_texture(p, t, loc);
  } else {
    fprintf(stderr, "Pipeline does not have a texture named %s.\n", name);
  }
}

void bind_pipeline(struct Pipeline *p) {
  int v, t;
  if (p) {
    // printf("vao id: %d\n", p->vao);
    // printf("program id: %d\n", p->program_id);
    glBindVertexArray(p->vao);
    glUseProgram(p->program_id);
    for (v = 0; v < p->vertex_input_size; ++v) {
      if (p->vertex_inputs[v].buffer) {
        struct VertexInput *vi = &p->vertex_inputs[v];
        int i;
        for (i = 0; i < vi->rows; i++) {
          glBindBuffer(GL_ARRAY_BUFFER, vi->buffer->id);
          int offset = vi->offset + i * vi->size * sizeof(GLfloat);
          glVertexAttribPointer(vi->location + i, vi->size, GL_FLOAT, GL_FALSE, vi->stride, (void *)(offset));
        }
      }
    }
    for (t = 0; t < p->texture_input_size; ++t) {
      if (p->texture_inputs[t].texture) {
        struct TextureInput *ti = &p->texture_inputs[t];
        glActiveTexture(GL_TEXTURE0 + ti->location);
        glBindTexture(ti->texture->target, ti->texture->id);
        glUniform1i(glGetUniformLocation(p->program_id, ti->name), ti->location); //required for <4.2
      }
    }
    if (p->index_buffer) {
      bind_index_buffer(p, p->index_buffer);
    }
  } else {
    glBindVertexArray(0);
    glUseProgram(0);
  }
}

void run_pipeline(struct Pipeline *p) {
  if (p->instance_count) {
    if (p->index_count) {
      glDrawElementsInstanced(GL_TRIANGLES, p->index_count, GL_UNSIGNED_INT, 0, p->instance_count);
    } else {
      glDrawArraysInstanced(GL_TRIANGLES, 0, p->count, p->instance_count);
    }
  } else {
    if (p->index_count) {
      glDrawElements(GL_TRIANGLES, p->index_count, GL_UNSIGNED_INT, 0);
    } else {
      glDrawArrays(GL_TRIANGLES, 0, p->count);
    }
  }
}

void run_pipeline_n(struct Pipeline *p, int n) {
  glDrawArrays(GL_TRIANGLES, 0, n);
}

void push_constant_float(struct Pipeline *p, const char *name, float t) {
  glUniform1f(glGetUniformLocation(p->program_id, name), t);
}
void push_constant_vec2(struct Pipeline *p, const char *name, float t[2]) {
  glUniform2fv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_vec3(struct Pipeline *p, const char *name, float t[3]) {
  glUniform3fv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_vec4(struct Pipeline *p, const char *name, float t[4]) {
  glUniform4fv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_mat2(struct Pipeline *p, const char *name, float t[2 * 2]) {
  glUniformMatrix2fv(glGetUniformLocation(p->program_id, name), 1, GL_FALSE, t);
}
void push_constant_mat3(struct Pipeline *p, const char *name, float t[3 * 3]) {
  glUniformMatrix3fv(glGetUniformLocation(p->program_id, name), 1, GL_FALSE, t);
}
void push_constant_mat4(struct Pipeline *p, const char *name, float t[4 * 4]) {
  glUniformMatrix4fv(glGetUniformLocation(p->program_id, name), 1, GL_FALSE, t);
}
void push_constant_int(struct Pipeline *p, const char *name, int t) {
  glUniform1i(glGetUniformLocation(p->program_id, name), t);
}
void push_constant_ivec2(struct Pipeline *p, const char *name, int t[2]) {
  glUniform2iv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_ivec3(struct Pipeline *p, const char *name, int t[3]) {
  glUniform3iv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_ivec4(struct Pipeline *p, const char *name, int t[4]) {
  glUniform4iv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_uint(struct Pipeline *p, const char *name, unsigned int t) {
  glUniform1ui(glGetUniformLocation(p->program_id, name), t);
}
void push_constant_uvec2(struct Pipeline *p, const char *name, unsigned int t[2]) {
  glUniform2uiv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_uvec3(struct Pipeline *p, const char *name, unsigned int t[3]) {
  glUniform3uiv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_uvec4(struct Pipeline *p, const char *name, unsigned int t[4]) {
  glUniform4uiv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_double(struct Pipeline *p, const char *name, double t) {
  glUniform1d(glGetUniformLocation(p->program_id, name), t);
}
void push_constant_dvec2(struct Pipeline *p, const char *name, double t[2]) {
  glUniform2dv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_dvec3(struct Pipeline *p, const char *name, double t[3]) {
  glUniform3dv(glGetUniformLocation(p->program_id, name), 1, t);
}
void push_constant_dvec4(struct Pipeline *p, const char *name, double t[4]) {
  glUniform4dv(glGetUniformLocation(p->program_id, name), 1, t);
}
