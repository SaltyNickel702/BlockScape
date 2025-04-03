#include <FastNoise/FastNoiseLite.h>
#include "Chunk.h"
#include "World.h"

Chunk Chunk::genChunk (int cx, int cz) {
	Chunk c;
	c.pos = glm::vec2(cx,cz); //assign position for reference



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
		for (int z = 0; z < 16; y++) {
			// Get height value and scale it to terrain height
			float heightValue = noise.GetNoise((float)(x + cx*16), (float)(z + cz*16));
			int terrainHeight = (int)((heightValue + 1) * 0.5 * (64 / 2)) + (64 / 4);
			for (int y = 0; y < 64; y++) {
				// Assign blocks based on Z level
				if (y > terrainHeight) {
					c.blocks[x][y][z] = 0;  
				} 
				else if (y == terrainHeight) {
					c.blocks[x][y][z] = 1;  
				} 
				else if (y > terrainHeight - 1) {
					c.blocks[x][y][z] = 2;  
				} 
				else if (y > 20) {
					c.blocks[x][y][z] = 3;  
				} 
				else {
					c.blocks[x][y][z] = 3;  
				}
			}
		}
	}

	return c;
}

Model Chunk::genMesh() {
	vector<float> vertices;
	vector<unsigned int> indices;
	vector<unsigned int> attrib {3,3,2,1}; //pos, normal, uv, texture ID

	float faces = 0;
	for (int x = 0; x < 16; x++) {
		for (int y = 0; y < 64; y++) {
			for (int z = 0; z < 16; z++) {
				int ths = blocks[x][y][z];
				if (ths == 0) continue;
				Block* blck = &World::blockTypes[ths]; //pointer bc more memory efficient. No new block class for each block

				int left = (x < 15 ? blocks[x+1][y][z] : -1); //-1 means not present | replace -1 with other side chunk block for x and z
				int right = (x > 0 ? blocks[x-1][y][z] : -1);

				int up = (y < 63 ? blocks[x][y+1][z] : -1);
				int down = (y > 0 ? blocks[x][y-1][z] : -1);

				int front = (z < 15 ? blocks[x][y][z+1] : -1);
				int back = (z > 0 ? blocks[x][y][z-1] : -1);

				if (left == 0) {
					vertices.insert(vertices.end(),{x+1.0f,y+0.0f,z+0.0f,	1,0,0,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+0.0f,z+1.0f,	1,0,0,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+1.0f,z+1.0f,	1,0,0,	0,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+1.0f,z+0.0f,	1,0,0,	0,0,	(float)blck->textureSide});

					indices.push_back((float)faces*4.0f + 0.0f, (float)faces*4.0f + 1.0f, (float)faces*4.0f + 2.0f);
					indices.push_back((float)faces*4.0f + 2.0f, (float)faces*4.0f + 3.0f, (float)faces*4.0f + 0.0f);
					
					faces++;
				}
				if (right == 0) {
					vertices.insert(vertices.end(),{x-1.0f,y+0.0f,z+0.0f,	-1,0,0,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x-1.0f,y+0.0f,z+1.0f,	-1,0,0,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x-1.0f,y+1.0f,z+1.0f,	-1,0,0,	0,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x-1.0f,y+1.0f,z+0.0f,	-1,0,0,	0,0,	(float)blck->textureSide});

					indices.push_back((float)faces*4.0f + 0.0f, (float)faces*4.0f + 1.0f, (float)faces*4.0f + 2.0f);
					indices.push_back((float)faces*4.0f + 2.0f, (float)faces*4.0f + 3.0f, (float)faces*4.0f + 0.0f);
					
					faces++;
				}
				if (up == 0) {
					vertices.insert(vertices.end(),{x+0.0f,y+1.0f,z+0.0f,	0,1,0,	1,0,	(float)blck->textureTop});
					vertices.insert(vertices.end(),{x+1.0f,y+1.0f,z+0.0f,	0,1,0,	1,1,	(float)blck->textureTop});
					vertices.insert(vertices.end(),{x+1.0f,y+1.0f,z+1.0f,	0,1,0,	0,1,	(float)blck->textureTop});
					vertices.insert(vertices.end(),{x+0.0f,y+1.0f,z+1.0f,	0,1,0,	0,0,	(float)blck->textureTop});

					indices.insert(indices.end(),{(float)faces*4.0f + 0.0f, (float)faces*4.0f + 1.0f, (float)faces*4.0f + 2.0f});
					indices.insert(indices.end(),{(float)faces*4.0f + 2.0f, (float)faces*4.0f + 3.0f, (float)faces*4.0f + 0.0f});
					
					faces++;
				}
				if (down == 0) {
					vertices.insert(vertices.end(),{x+0.0f,y-1.0f,z+0.0f,	0,-1,0,	1,0,	(float)blck->textureBottom});
					vertices.insert(vertices.end(),{x+1.0f,y-1.0f,z+0.0f,	0,-1,0,	1,1,	(float)blck->textureBottom});
					vertices.insert(vertices.end(),{x+1.0f,y-1.0f,z+1.0f,	0,-1,0,	0,1,	(float)blck->textureBottom});
					vertices.insert(vertices.end(),{x+0.0f,y-1.0f,z+1.0f,	0,-1,0,	0,0,	(float)blck->textureBottom});

					indices.insert(indices.end(),{(float)faces*4.0f + 0.0f, (float)faces*4.0f + 1.0f, (float)faces*4.0f + 2.0f});
					indices.insert(indices.end(),{(float)faces*4.0f + 2.0f, (float)faces*4.0f + 3.0f, (float)faces*4.0f + 0.0f});
					
					faces++;
				}
				if (front == 0) {
					vertices.insert(vertices.end(),{x+0.0f,y+0.0f,z+1.0f,	0,0,1,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+0.0f,z+1.0f,	0,0,1,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+1.0f,z+1.0f,	0,0,1,	0,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+0.0f,y+1.0f,z+1.0f,	0,0,1,	0,0,	(float)blck->textureSide});

					indices.insert(indices.end(),{(float)faces*4.0f + 0.0f, (float)faces*4.0f + 1.0f, (float)faces*4.0f + 2.0f});
					indices.insert(indices.end(),{(float)faces*4.0f + 2.0f, (float)faces*4.0f + 3.0f, (float)faces*4.0f + 0.0f});
					
					faces++;
				}
				if (back == 0) {
					vertices.insert(vertices.end(),{x+0.0f,y+0.0f,z-1.0f,	0,0,-1,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+0.0f,z-1.0f,	0,0,-1,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+1.0f,y+1.0f,z-1.0f,	0,0,-1,	0,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{x+0.0f,y+1.0f,z-1.0f,	0,0,-1,	0,0,	(float)blck->textureSide});

					indices.insert(indices.end(),{(float)faces*4.0f + 0.0f, (float)faces*4.0f + 1.0f, (float)faces*4.0f + 2.0f});
					indices.insert(indices.end(),{(float)faces*4.0f + 2.0f, (float)faces*4.0f + 3.0f, (float)faces*4.0f + 0.0f});
					
					faces++;
				}
			}
		}
	}

	Model c(&vertices, &indices, &attrib);
	c.pos = pos;
	
	mesh = &c;
	return c;
}


int* Chunk::getBlock (int x, int y, int z) {
	return &blocks[x][y][z];
}
void Chunk::setBlock (int x, int y, int z, int blockID) {
	int* b = getBlock(x,y,z);
	*b = blockID;
}