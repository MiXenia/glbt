#include "glbt.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "cglm/cglm.h"

#include <math.h>
#include <time.h>

#define WIDTH 1600
#define HEIGHT 900
#define NUM_SPRITES 200000
struct Sprite {
    vec3 position;
    vec3 velocity;
    float rotation;
    float rvelocity;
};
void update_sprite(int i, struct Sprite *sprites, mat4* mats) {
    struct Sprite* s = &sprites[i];
    s->position[0] += s->velocity[0];
    s->position[1] += s->velocity[1];
    if (s->position[0] > WIDTH || s->position[0] < 0) {
        s->velocity[0] = -s->velocity[0];
    }
    if (s->position[1] > HEIGHT || s->position[1] < 0) {
        s->velocity[1] = -s->velocity[1];
    }
    s->rotation += s->rvelocity;
    if (s->rotation > M_PI * 2.0f) {
        s->rotation -= M_PI * 2.0f;
    }
    //printf("%f\t%f\t%f\t%f\t\n", s->position[0], s->position[1], s->velocity[0], s->velocity[1]);
    glm_mat4_identity(mats[i]);
    vec3 pos = {-1.0f + s->position[0] / ((float)WIDTH / 2.0f), -1.0f + s->position[1] / ((float)HEIGHT / 2.0f), 0};
    glm_translate(mats[i], pos);
    glm_scale(mats[i], (vec3) { 0.1f, 0.1f, 0.1f });
    glm_rotate_z(mats[i], s->rotation, mats[i]);
}

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
    length = (int)fread(src, 1, length, f); // update length because ftell is not accurate.
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
  enum TextureFlags flags = 0;
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
  struct Window *win = create_window("UwU", WIDTH, HEIGHT);

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
  struct Pipeline *p = create_pipeline(vert_len, vert_src, frag_len, frag_src, 4, v, 1, t);
  free(vert_src);
  free(frag_src);

  printf("Creating buffers...\n");
  struct Buffer *positions = create_buffer(4 * 3, (float[4 * 3]){
    -0.5f, +0.5f, 0.0f,
    +0.5f, +0.5f, 0.0f,
    +0.5f, -0.5f, 0.0f,
    -0.5f, -0.5f, 0.0f,
  });
  struct Buffer *texcoords = create_buffer(4 * 2, (float[4 * 2]){
    0.0, 0.0,
    1.0, 0.0,
    1.0, 1.0,
    0.0, 1.0
  });
  struct Buffer *colors = create_buffer(4 * 4, (float[4 * 4]){
    1.0f, 0.0f, 0.0f, 1.0f,
    0.0f, 1.0f, 0.0f, 1.0f,
    0.0f, 0.0f, 1.0f, 1.0f,
    1.0f, 1.0f, 1.0f, 0.0f
  });
  struct Buffer *indices = create_index_buffer(6, (unsigned int[6]){0, 1, 2, 0, 2, 3});

  struct Texture *texture = load_texture("resources/bunny.png");

  printf("binding everything.\n");
  bind_buffer(p, positions, 0);
  bind_buffer(p, colors, 1);
  bind_buffer(p, texcoords, 2);
  bind_index_buffer(p, indices);
  bind_texture(p, texture, 0);
  glDisable(GL_DEPTH_TEST);

  srand(time(NULL));
  //create our instance data amnd upload into a single buffer
  struct Buffer *mvps = create_buffer(NUM_SPRITES * 16, NULL);
  mat4* mats = malloc(sizeof(mat4) * NUM_SPRITES);
  struct Sprite* sprites = malloc(sizeof(struct Sprite) * NUM_SPRITES);
  int i;
  for (i = 0; i < NUM_SPRITES; ++i) {
      glm_mat4_identity(mats[i]);
      sprites[i].position[0] = ((float)rand() / (float)RAND_MAX) * 640.0f;
      sprites[i].position[1] = ((float)rand() / (float)RAND_MAX) * 400.0f;
      sprites[i].velocity[0] = ((float)rand() / (float)RAND_MAX) * 16.0f;
      sprites[i].velocity[1] = ((float)rand() / (float)RAND_MAX) * 16.0f;
      sprites[i].rotation = ((float)rand() / (float)RAND_MAX) * 2.0f * M_PI;
      sprites[i].rvelocity = ((float)rand() / (float)RAND_MAX) * 0.1f;
  }

  bind_buffer(p, mvps, 3);

  printf("Creating framebuffers\n");

  // downscaled framebuffer
  struct Texture *color0 = create_texture_2d(NULL, WIDTH, HEIGHT, 0);
  struct Texture *depth = create_texture_2d(NULL, WIDTH, HEIGHT, 0 | DEPTH);
  struct RenderTarget *rt = create_render_target(1, &color0, depth);

  printf("Setting up postprocessing pipeline\n");

  // simple postprocessing pipeline
  vert_src = load_file("resources/postprocess.glsl.vert", &vert_len);
  frag_src = load_file("resources/postprocess.glsl.frag", &frag_len);
  struct TextureInput ppt[] = {{0, "color0"}};
  struct Pipeline *pp = create_pipeline(vert_len, vert_src, frag_len, frag_src, 0, 0, 1, ppt);
  free(vert_src);
  free(frag_src);
  bind_texture(pp, render_texture(rt), 0);

  printf("Starting main loop...\n");
  double then = glfwGetTime();
  int frames = 0;
  char window_title[4];
  const char *uwu = "UwU";
  memcpy(window_title, uwu, 4);
  while (window_status(win) == RUNNING) {
    if (input_state(KEYBOARD_ESCAPE) == PRESSED) {
      close_window(win);
    }

    if (input_state(MOUSE_BUTTON_LEFT) == PRESSED) {
      int x, y;
      cursor_position(&x, &y);
      printf("%d, %d\n", x, y);
    }

    for (i = 0; i < NUM_SPRITES; ++i) {
        update_sprite(i, sprites, mats);
    }

    begin_pass(rt);
    clear(1, 0, 1, 1);
    buffer_sub_data(mvps, sizeof(mat4), 0, NUM_SPRITES, &mats[0][0]);
    //buffer_data(mvps, NUM_SPRITES * 4 * 4, &mats[0][0]);
    bind_pipeline(p);
    run_pipeline(p);
    end_pass(rt);

    begin_pass(screen(win));
    clear(0, 0, 0, 0);
    bind_pipeline(pp);
    push_constant_int(pp, "bnw", 1); // enable black-n-white
    run_pipeline_n(pp, 3);
    end_pass(screen(win));

    refresh(win);

    frames++;
    double now = glfwGetTime();
    if (now - then >= 1.0) {
        sprintf(window_title, "%d.2", frames);
        set_window_title(win, window_title);
        frames = 0;
        then = now;
    }

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
  destroy_render_target(rt);
  destroy_pipeline(pp);
  quit();
  return 0;
}
