#include "glbt.h"

void fail(int error_code, const char* error_message) {
	fprintf(stderr, "%d: %s\n", error_code, error_message);
	//exit(1);
}

int check_errors() {
	GLenum err = 0;
	int err_count = 0;
	while (err = glGetError()) {
		err_count++;
		switch (err) {
		case GL_INVALID_ENUM:
			fail(err, "GL_INVALID_ENUM");
			break;
		case GL_INVALID_VALUE:
			fail(err, "GL_INVALID_VALUE");
			break;
		case GL_INVALID_OPERATION:
			fail(err, "GL_INVALID_OPERATION");
			break;
		case GL_INVALID_FRAMEBUFFER_OPERATION:
			fail(err, "GL_INVALID_FRAMEBUFFER_OPERATION");
			break;
		case GL_OUT_OF_MEMORY:
			fail(err, "GL_OUT_OF_MEMORY");
			break;
		}
	}
	return err_count;
}

struct Window {
	GLFWwindow* window;
};
static GLFWwindow* main_window = NULL;


//input shared across all windows, as it should be.
//by the way I love using enums as array indices
static int previous_input_state[INPUT_CODE_LAST] = { 0 };
static int current_input_state[INPUT_CODE_LAST] = { 0 };

#define map_key(input_state, glfw_key)\
	case glfw_key:\
		actual_key = input_state;\
		break;
		
static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	int actual_key = 0;
	switch (key) {
		map_key(KEYBOARD_0, GLFW_KEY_0);
		map_key(KEYBOARD_1, GLFW_KEY_1);
		map_key(KEYBOARD_2, GLFW_KEY_2);
		map_key(KEYBOARD_3, GLFW_KEY_3);
		map_key(KEYBOARD_4, GLFW_KEY_4);
		map_key(KEYBOARD_5, GLFW_KEY_5);
		map_key(KEYBOARD_6, GLFW_KEY_6);
		map_key(KEYBOARD_7, GLFW_KEY_7);
		map_key(KEYBOARD_8, GLFW_KEY_8);
		map_key(KEYBOARD_9, GLFW_KEY_9);
		map_key(KEYBOARD_A, GLFW_KEY_A);
		map_key(KEYBOARD_B, GLFW_KEY_B);
		map_key(KEYBOARD_C, GLFW_KEY_C);
		map_key(KEYBOARD_D, GLFW_KEY_D);
		map_key(KEYBOARD_E, GLFW_KEY_E);
		map_key(KEYBOARD_F, GLFW_KEY_F);
		map_key(KEYBOARD_G, GLFW_KEY_G);
		map_key(KEYBOARD_H, GLFW_KEY_H);
		map_key(KEYBOARD_I, GLFW_KEY_I);
		map_key(KEYBOARD_J, GLFW_KEY_J);
		map_key(KEYBOARD_K, GLFW_KEY_K);
		map_key(KEYBOARD_L, GLFW_KEY_L);
		map_key(KEYBOARD_M, GLFW_KEY_M);
		map_key(KEYBOARD_N, GLFW_KEY_N);
		map_key(KEYBOARD_O, GLFW_KEY_O);
		map_key(KEYBOARD_P, GLFW_KEY_P);
		map_key(KEYBOARD_Q, GLFW_KEY_Q);
		map_key(KEYBOARD_R, GLFW_KEY_R);
		map_key(KEYBOARD_S, GLFW_KEY_S);
		map_key(KEYBOARD_T, GLFW_KEY_T);
		map_key(KEYBOARD_U, GLFW_KEY_U);
		map_key(KEYBOARD_V, GLFW_KEY_V);
		map_key(KEYBOARD_W, GLFW_KEY_W);
		map_key(KEYBOARD_X, GLFW_KEY_X);
		map_key(KEYBOARD_Y, GLFW_KEY_Y);
		map_key(KEYBOARD_Z, GLFW_KEY_Z);
		map_key(KEYBOARD_UP, GLFW_KEY_UP);
		map_key(KEYBOARD_DOWN, GLFW_KEY_DOWN);
		map_key(KEYBOARD_LEFT, GLFW_KEY_LEFT);
		map_key(KEYBOARD_RIGHT, GLFW_KEY_RIGHT);
		map_key(KEYBOARD_SPACE, GLFW_KEY_SPACE);
		map_key(KEYBOARD_ESCAPE, GLFW_KEY_ESCAPE);
		map_key(KEYBOARD_RETURN, GLFW_KEY_ENTER);
		map_key(KEYBOARD_SHIFT, GLFW_KEY_LEFT_SHIFT);
		map_key(KEYBOARD_CONTROL, GLFW_KEY_LEFT_CONTROL);
	}
	current_input_state[actual_key] = (action == GLFW_PRESS || action == GLFW_REPEAT);
}
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	int actual_key = 0;
	switch (button) {
		map_key(MOUSE_BUTTON_LEFT, GLFW_MOUSE_BUTTON_LEFT);
		map_key(MOUSE_BUTTON_MIDDLE, GLFW_MOUSE_BUTTON_MIDDLE);
		map_key(MOUSE_BUTTON_RIGHT, GLFW_MOUSE_BUTTON_RIGHT);
	}
	current_input_state[actual_key] = (action == GLFW_PRESS);
}

struct Window* create_window(const char* title, int width, int height) {
	struct Window* w = malloc(sizeof(struct Window));
	if (!w) {
		fail(0, "Out of memory, I guess");
		return NULL;
	}
	if (!main_window) {
		glfwSetErrorCallback(fail);
		glfwInit();
	}

	glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);

	glfwWindowHint(GLFW_SAMPLES, 8);

	w->window = glfwCreateWindow(width, height, title, NULL, NULL);
	glfwSetKeyCallback(w->window, key_callback);
	glfwSetMouseButtonCallback(w->window, mouse_button_callback);
	if (!main_window) {
		main_window = w->window;
		glfwMakeContextCurrent(w->window);
	}

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fail(0, "Failed to initialize GLAD");
	}
	printf("%s\n", glGetString(GL_VERSION));
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_MULTISAMPLE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return w;
}

enum WindowStatus window_status(struct Window* w) {
	glfwPollEvents();
	if (glfwWindowShouldClose(w->window)) {
		//do automatic cleanup if window has been destroyed.
		glfwDestroyWindow(w->window);
		if (main_window == w->window) {
			main_window = NULL;
			glfwTerminate();
		}
		free(w);
		return CLOSED;
	}
	else {
		return RUNNING;
	}
}
void close_window(struct Window* w) {
	glfwSetWindowShouldClose(w->window, 1);
}

void scan_inputs() {
	//update input state
	int i;
	for (i = 0; i < INPUT_CODE_LAST; ++i) {
		previous_input_state[i] = current_input_state[i];
	}
}
void refresh(struct Window* w) {
	//flush GL, check for errors
	glfwMakeContextCurrent(w->window);
	glfwSwapBuffers(w->window);
	check_errors();
	//assume update rate is same as refresh rate if not already called manually
	scan_inputs();
}

enum InputState input_state(enum InputCode input_code) {
	if (current_input_state[input_code] && !previous_input_state[input_code]) {
		return PRESSED;
	}
	if (!current_input_state[input_code] && previous_input_state[input_code]) {
		return RELEASED;
	}
	if (current_input_state[input_code] && previous_input_state[input_code]) {
		return DOWN;
	}
	return UP;
}
void cursor_position(int* x, int* y) {
	double dx, dy;
	glfwGetCursorPos(main_window, &dx, &dy);
	if (x) {
		*x = (int)dx;
	}
	if (y) {
		*y = (int)dy;
	}
}
int cursor_x() {
	int x;
	cursor_position(&x, NULL);
	return x;
}
int cursor_y() {
	int y;
	cursor_position(NULL, &y);
	return y;
}

//same as above but relative to some window instead of just the main window.
void window_cursor_position(struct Window *w, int* x, int* y) {
	double dx, dy;
	glfwGetCursorPos(w->window, &dx, &dy);
	if (x) {
		*x = (int)dx;
	}
	if (y) {
		*y = (int)dy;
	}
}
int window_cursor_x(struct Window *w) {
	int x;
	window_cursor_position(w, &x, NULL);
	return x;
}
int window_cursor_y(struct Window *w) {
	int y;
	window_cursor_position(w, NULL, &y);
	return y;
}
