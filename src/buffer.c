#include "internal/buffer.h"

struct Buffer* create_buffer(int length, float* data) {
	struct Buffer* b = malloc(sizeof(struct Buffer)); //malloc just for one uint?
	glGenBuffers(1, &b->id);
	b->length = length;
	glBindBuffer(GL_ARRAY_BUFFER, b->id);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), data, GL_STATIC_DRAW);
	return b;
}
struct Buffer* create_index_buffer(int length, unsigned int* indices) {
	struct Buffer* b = malloc(sizeof(struct Buffer));
	glGenBuffers(1, &b->id);
	b->length = length;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(GLuint), indices, GL_STATIC_DRAW);
	return b;
}
void destroy_buffer(struct Buffer* b) {
	glDeleteBuffers(1, &b->id);
	free(b);
}
void buffer_data(struct Buffer* b, int length, float* data) {
	b->length = length;
	glBindBuffer(GL_ARRAY_BUFFER, b->id);
	glBufferData(GL_ARRAY_BUFFER, length * sizeof(GLfloat), data, GL_DYNAMIC_DRAW);
}
void buffer_sub_data(struct Buffer* b, size_t element_size, int index, int num_elements, float* elements) {
	glBindBuffer(GL_ARRAY_BUFFER, b->id);
	glBufferSubData(GL_ARRAY_BUFFER, index * element_size, num_elements * element_size, elements);
}
void buffer_indices(struct Buffer* b, int length, unsigned int* indices) {
	b->length = length;
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, b->id);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, length * sizeof(GLuint), indices, GL_DYNAMIC_DRAW);
}