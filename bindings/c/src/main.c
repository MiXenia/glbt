#include "glbt.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cglm/cglm.h"

// resource management is outside the scope of this library, so belongs in main.
char *load_file(const char *path, int *out_length) {
  printf("Loading file %s\n", path);
  FILE *f = fopen(path, "r");
  char *src = NULL;
  if (f) {
    fseek(f, 0, SEEK_END);
    int length = ftell(f);
    rewind(f);
    src = malloc(length);
    if (!src) {
      return NULL;
    }
    length = (int)fread(src, 1, length,
                        f); // update length because ftell is not accurate.
    fclose(f);
    if (out_length) {
      *out_length = length;
    }
  } else {
    fprintf(stderr, "Failed to open file %s\n", path);
  }
  return src;
}

struct Texture *load_texture(const char *path) {
  int width, height, components;
  enum TextureFlags flags = FILTERED;
  unsigned char *data = stbi_load(path, &width, &height, &components, 0);
  if (components == 4) {
    flags |= ALPHA;
  }
  if (!data) {
    fprintf(stderr, "Unable to load %s!\n", path);
  }
  struct Texture *texture = create_texture_2d(data, width, height, flags);
  stbi_image_free(data);
  return texture;
}

int main(int argc, char *argv[]) {
  printf("Creating window.\n");
  struct Window *win = create_window("UwU", 640, 400);

  printf("Creating vertex specification.\n");
  struct VertexInput v[4] = {
      {0, "position", 1, 3},
      {1, "color", 1, 4},
      {2, "texcoord", 1, 2},
      {3, "mvp", 4, 4, INSTANCE},
  };
  struct TextureInput t[1] = {{0, "tex"}};
  int vert_len;
  char *vert_src = load_file("resources/basic.glsl.vert", &vert_len);
  int frag_len;
  char *frag_src = load_file("resources/basic.glsl.frag", &frag_len);
  struct Pipeline *p =
      create_pipeline(vert_len, vert_src, frag_len, frag_src, 4, v, 1, t);
  free(vert_src);
  free(frag_src);

  printf("Creating buffers...\n");
  struct Buffer *positions = create_buffer(4 * 3, (float[4 * 3]){
                                                      -0.5f,
                                                      +0.5f,
                                                      0.0f,
                                                      +0.5f,
                                                      +0.5f,
                                                      0.0f,
                                                      +0.5f,
                                                      -0.5f,
                                                      0.0f,
                                                      -0.5f,
                                                      -0.5f,
                                                      0.0f,
                                                  });
  struct Buffer *texcoords = create_buffer(
      4 * 2, (float[4 * 2]){0.0, 0.0, 1.0, 0.0, 1.0, 1.0, 0.0, 1.0});
  struct Buffer *colors = create_buffer(
      4 * 4, (float[4 * 4]){1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f,
                            0.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f});
  struct Buffer *indices =
      create_index_buffer(6, (unsigned int[6]){0, 1, 2, 0, 2, 3});

  struct Texture *texture = load_texture("resources/go-home.png");

  printf("binding everything.\n");
  bind_buffer(p, positions, 0);
  bind_buffer(p, colors, 1);
  bind_buffer(p, texcoords, 2);
  bind_index_buffer(p, indices);
  bind_texture(p, texture, 0);

  // 3 instances
  // one at the center
  mat4 origin = {
      1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,  0.0f,
      0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -0.5f, 1.0f,
  };
  // one slightly to the right, rotated
  mat4 right;
  glm_mat4_identity(right);
  glm_translate_x(right, 0.75f);
  glm_rotate_z(right, M_PI / 16.0, right);
  // one slightly to the left, rotated
  mat4 left;
  glm_mat4_identity(left);
  glm_translate_x(left, -0.75f);
  glm_rotate_z(left, M_PI / 4.0, left);
  // upload instance data into a single buffer
  struct Buffer *mvps = create_buffer(3 * 16, NULL);
  buffer_sub_data(mvps, sizeof(mat4), 0, 1, origin);
  buffer_sub_data(mvps, sizeof(mat4), 1, 1, left);
  buffer_sub_data(mvps, sizeof(mat4), 2, 1, right);

  bind_buffer(p, mvps, 3);

  printf("Creating framebuffers\n");

  // downscaled framebuffer
  struct Texture *color0 = create_texture_2d(NULL, 128, 128, 0);
  struct Texture *depth = create_texture_2d(NULL, 128, 128, 0 | DEPTH);
  struct Framebuffer *f = create_framebuffer(1, &color0, depth);

  // framebuffer to resolve antialiasing to before final render
  struct Texture *resolve_color = create_texture_2d(NULL, 128, 128, 0);
  struct Framebuffer *resolve = create_framebuffer(1, &resolve_color, NULL);

  printf("Setting up postprocessing pipeline\n");

  // simple postprocessing pipeline
  vert_src = load_file("resources/postprocess.glsl.vert", &vert_len);
  frag_src = load_file("resources/postprocess.glsl.frag", &frag_len);
  struct TextureInput ppt[] = {{0, "color0"}};
  struct Pipeline *pp =
      create_pipeline(vert_len, vert_src, frag_len, frag_src, 0, 0, 1, ppt);
  free(vert_src);
  free(frag_src);
  bind_texture(pp, resolve_color, 0);

  printf("Starting main loop...\n");
  while (window_status(win) == RUNNING) {
    if (input_state(KEYBOARD_ESCAPE) == PRESSED) {
      close_window(win);
    }

    if (input_state(MOUSE_BUTTON_LEFT) == PRESSED) {
      int x, y;
      cursor_position(&x, &y);
      printf("%d, %d\n", x, y);
    }

    bind_framebuffer(f);
    viewport(0, 0, 128, 128);
    clear_screen(1, 0, 1, 1);
    bind_pipeline(p);
    run_pipeline(p);

    copy_framebuffer(f, resolve);

    bind_framebuffer(NULL);
    viewport(0, 0, 640, 400);
    clear_screen(0, 0, 0, 0);
    bind_pipeline(pp);
    push_constant_int(pp, "bnw", 1); // enable black-n-white
    run_pipeline_n(pp, 3);

    refresh(win);
  }

  printf("Closing.\n");
  // cleanup
  // if you value your sanity write a resource manager
  destroy_pipeline(p);
  destroy_buffer(positions);
  destroy_buffer(colors);
  destroy_buffer(indices);
  destroy_buffer(mvps);
  destroy_texture(texture);
  destroy_texture(color0);
  destroy_texture(depth);
  destroy_framebuffer(f);
  destroy_pipeline(pp);
  destroy_texture(resolve_color);
  destroy_framebuffer(resolve);
  quit();
  return 0;
}
