#include "Chunk.h"
#include <FastNoise/FastNoiseLite.h>

Chunk Chunk::genChunk (int cx, int cy) {
	Chunk c;
	c.pos = glm::vec2(cx,cy);

	int seed = 495804; // Change this int to change the seed
	float x,y,z;
	bool generateChunk = false;

	FastNoiseLite noise;  // Create noise generator
	//Fast Noise settings
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); 
	noise.SetFrequency(0.01f);
    float noiseValue = noise.GetNoise(x, y,z);
	noise.SetSeed(seed); 
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(3);
	noise.SetFractalLacunarity(2.76f); 
 
	
		// Generates terrain
		for (int x = 0; x < 16; x++) {
			for (int y = 0; y < 16; y++) {
				// Get height value and scale it to terrain height
				float heightValue = noise.GetNoise((float)(x + cx*16), (float)(y + cy*16));
				int terrainHeight = (int)((heightValue + 1) * 0.5 * (64 / 2)) + (64 / 4);
				for (int z = 0; z < 64; z++) {
					// Assign blocks based on Z level
					if (z > terrainHeight) {
						c.blocks[x][y][z] = 0;  
					} 
					else if (z == terrainHeight) {
						c.blocks[x][y][z] = 1;  
					} 
					else if (z > terrainHeight - 1) {
						c.blocks[x][y][z] = 2;  
					} 
					else if (z > 20) {
						c.blocks[x][y][z] = 3;  
					} 
					else {
						c.blocks[x][y][z] = 3;  
					}
				}
			}
		}
	}



	