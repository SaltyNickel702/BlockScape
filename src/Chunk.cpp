#include "Chunk.h"
#include <FastNoise/FastNoiseLite.h>

Chunk Chunk::genChunk (int cx, int cy) {
	Chunk c;
	c.pos = glm::vec2(cx,cy);

	//VVV do perlin noise stuff here VVV

	int seed = 495804; // Change this int to change the seed
	float x,y,z;

	FastNoiseLite noise;  // Create noise generator
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); 
	noise.SetFrequency(0.01f);
    float noiseValue = noise.GetNoise(x, y,z);
	noise.SetSeed(seed);  
	
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 16; y++) {
			//2d noise map value
			int noiseValue = 0;

			for (int z = 0; z < noiseValue; z++) {
				c.blocks[x][y][z] = 1;
			}
		}
	}

	return c;
}