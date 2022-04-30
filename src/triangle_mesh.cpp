#include "triangle_mesh.h"

void
triangle_mesh::initialize()
{
	glGenVertexArray(1, &_vertex_array);
	glGenBuffers(1, &_vertex_buffer);
	glGenBuffers(1, &_index_buffer);

	glBindVertexArray(_vertex_array);
}