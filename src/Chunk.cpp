#include "Chunk.h"

Chunk Chunk::genChunk (int cx, int cy) {
	Chunk c;
	c.pos = glm::vec2(cx,cy);

	//VVV do perlin noise stuff here VVV

	return c;
}