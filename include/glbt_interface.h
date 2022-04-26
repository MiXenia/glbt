void fail(int error_code, const char* error_message);
int check_errors();

struct Pipeline;
struct Buffer;
struct Texture;
enum VertexRate {
	VERTEX,
	INSTANCE,
};
struct VertexInput {
	int location;
	const char* name;
	int rows;
	int size; //number of floats i.e. 4 for vec4
	enum VertexRate rate;
	int stride;
	int offset;
	struct Buffer* buffer; //hehe
};

struct TextureInput {
	int location;
	const char* name;
	struct Texture* texture; //hehe
};

struct Pipeline* create_pipeline(int vs_len, char* vs, int fs_len, char* fs, int attribute_count, struct VertexInput* v, int texture_count, struct TextureInput* t);
void destroy_pipeline(struct Pipeline* p);
void bind_pipeline(struct Pipeline* p); //or NULL
void run_pipeline(struct Pipeline* p);
void run_pipeline_n(struct Pipeline* p, int n);

struct Buffer* create_buffer(int length, float* data);
struct Buffer* create_index_buffer(int length, unsigned int* indices);
void destroy_buffer(struct Buffer* b);
void buffer_data(struct Buffer* b, int length, float* data); //it's always floats.
void buffer_sub_data(struct Buffer* b, size_t element_size, int index, int num_elements, float* elements);
void buffer_indices(struct Buffer* b, int length, unsigned int* indices); //it's always uints.
void bind_buffer(struct Pipeline* p, struct Buffer* b, int location);
void bind_buffer_name(struct Pipeline* p, struct Buffer* b, const char* name); //might fail.
void bind_index_buffer(struct Pipeline* p, struct Buffer* b);

enum TextureFlags {
	FILTERED = 1 << 0,
	ANTIALIASED = 1 << 1,
	DEPTH = 1 << 2,
	ALPHA = 1 << 3,
};
struct Texture* create_texture_2d(char* data, int width, int height, enum TextureFlags flags); //or data= NULL to use as render target :3
void destroy_texture(struct Texture* t);
void bind_texture(struct Pipeline* p, struct Texture* t, int location);
void bind_texture_name(struct Pipeline* p, struct Texture* t, const char* name);

struct RenderTarget;
struct RenderTarget* create_render_target(int color_count, struct Texture** color, struct Texture* depth);
void destroy_render_target(struct RenderTarget* rt);
struct Texture* render_texture(struct RenderTarget* rt);
void begin_pass(struct RenderTarget* rt);
void end_pass(struct RenderTarget* rt);
void clear(float r, float g, float b, float a);
void copy(struct RenderTarget* src, struct RenderTarget* dst);
void copy_region(struct RenderTarget* src, struct RenderTarget* dst,
				 int sx, int sy, int sw, int sh,
				 int dx, int dy, int dw, int dh);

struct Window;
struct Window* create_window(const char* title, int width, int height);
enum WindowStatus { CLOSED, RUNNING };
enum WindowStatus window_status(struct Window* w);
void refresh(struct Window* w);
void close_window(struct Window* w);
struct RenderTarget *screen(struct Window* w);
void window_cursor_position(struct Window* w, int* x, int* y);
int window_cursor_x(struct Window* w);
int window_cursor_y(struct Window* w);
void cursor_position(int* x, int* y);
int cursor_x();
int cursor_y();

enum InputCode {
	KEYBOARD_0,
	KEYBOARD_1,
	KEYBOARD_2,
	KEYBOARD_3,
	KEYBOARD_4,
	KEYBOARD_5,
	KEYBOARD_6,
	KEYBOARD_7,
	KEYBOARD_8,
	KEYBOARD_9,
	KEYBOARD_A,
	KEYBOARD_B,
	KEYBOARD_C,
	KEYBOARD_D,
	KEYBOARD_E,
	KEYBOARD_F,
	KEYBOARD_G,
	KEYBOARD_H,
	KEYBOARD_I,
	KEYBOARD_J,
	KEYBOARD_K,
	KEYBOARD_L,
	KEYBOARD_M,
	KEYBOARD_N,
	KEYBOARD_O,
	KEYBOARD_P,
	KEYBOARD_Q,
	KEYBOARD_R,
	KEYBOARD_S,
	KEYBOARD_T,
	KEYBOARD_U,
	KEYBOARD_V,
	KEYBOARD_W,
	KEYBOARD_X,
	KEYBOARD_Y,
	KEYBOARD_Z,
	KEYBOARD_UP,
	KEYBOARD_DOWN,
	KEYBOARD_LEFT,
	KEYBOARD_RIGHT,
	KEYBOARD_SPACE,
	KEYBOARD_ESCAPE,
	KEYBOARD_RETURN,
	KEYBOARD_SHIFT,
	KEYBOARD_CONTROL,
	MOUSE_BUTTON_LEFT,
	MOUSE_BUTTON_RIGHT,
	MOUSE_BUTTON_MIDDLE,
	INPUT_CODE_LAST,
};
enum InputState { UP, DOWN, PRESSED, RELEASED };
enum InputState input_state(enum InputCode code);
