#include <FastNoise/FastNoiseLite.h>
#include "Chunk.h"
#include "World.h"

Chunk Chunk::genChunk (int cx, int cz) {
	Chunk c;
	c.pos = glm::vec2(cx,cz); //assign position for reference



	int seed = World::seed;

	FastNoiseLite noise;  // Create noise generator
	//Fast Noise settings
    noise.SetNoiseType(FastNoiseLite::NoiseType_OpenSimplex2); 
	noise.SetFrequency(0.01f);
	noise.SetSeed(seed); 
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(3);
	noise.SetFractalLacunarity(2.76f); 
 
	
	// Generates terrain
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
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
				else if (y >= terrainHeight - 4) {
					c.blocks[x][y][z] = 2;  
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

	int faces = 0;
	for (int x = 0; x < 16; (float)x++) {
		for (int y = 0; y < 64; (float)y++) {
			for (int z = 0; z < 16; (float)z++) {
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
					vertices.insert(vertices.end(),{(float)x+1,(float)y+0,(float)z+0,	1,0,0,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+1,(float)z+0,	1,0,0,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+1,(float)z+1,	1,0,0,	0,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+0,(float)z+1,	1,0,0,	0,1,	(float)blck->textureSide});

					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 0), static_cast<unsigned int>(faces*4 + 1), static_cast<unsigned int>(faces*4 + 2)});
					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 2), static_cast<unsigned int>(faces*4 + 3), static_cast<unsigned int>(faces*4 + 0)});
					
					faces++;
				}
				if (right == 0) {
					vertices.insert(vertices.end(),{(float)x+0,(float)y+0,(float)z+0,	-1,0,0,	0,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+0,(float)z+1,	-1,0,0,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+1,(float)z+1,	-1,0,0,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+1,(float)z+0,	-1,0,0,	0,0,	(float)blck->textureSide});

					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 0), static_cast<unsigned int>(faces*4 + 1), static_cast<unsigned int>(faces*4 + 2)});
					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 2), static_cast<unsigned int>(faces*4 + 3), static_cast<unsigned int>(faces*4 + 0)});
					
					faces++;
				}
				if (up == 0) {
					vertices.insert(vertices.end(),{(float)x+0,(float)y+1,(float)z+0,	0,1,0,	0,1,	(float)blck->textureTop});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+1,(float)z+1,	0,1,0,	1,1,	(float)blck->textureTop});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+1,(float)z+1,	0,1,0,	1,0,	(float)blck->textureTop});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+1,(float)z+0,	0,1,0,	0,0,	(float)blck->textureTop});

					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 0), static_cast<unsigned int>(faces*4 + 1), static_cast<unsigned int>(faces*4 + 2)});
					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 2), static_cast<unsigned int>(faces*4 + 3), static_cast<unsigned int>(faces*4 + 0)});
					
					faces++;
				}
				if (down == 0) {
					vertices.insert(vertices.end(),{(float)x+0,(float)y+0,(float)z+0,	0,-1,0,	1,1,	(float)blck->textureBottom});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+0,(float)z+0,	0,-1,0,	1,0,	(float)blck->textureBottom});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+0,(float)z+1,	0,-1,0,	0,0,	(float)blck->textureBottom});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+0,(float)z+1,	0,-1,0,	0,1,	(float)blck->textureBottom});

					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 0), static_cast<unsigned int>(faces*4 + 1), static_cast<unsigned int>(faces*4 + 2)});
					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 2), static_cast<unsigned int>(faces*4 + 3), static_cast<unsigned int>(faces*4 + 0)});
					
					faces++;
				}
				if (front == 0) {
					vertices.insert(vertices.end(),{(float)x+0,(float)y+0,(float)z+1,	0,0,1,	0,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+0,(float)z+1,	0,0,1,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+1,(float)z+1,	0,0,1,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+1,(float)z+1,	0,0,1,	0,0,	(float)blck->textureSide});

					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 0), static_cast<unsigned int>(faces*4 + 1), static_cast<unsigned int>(faces*4 + 2)});
					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 2), static_cast<unsigned int>(faces*4 + 3), static_cast<unsigned int>(faces*4 + 0)});
					
					faces++;
				}
				if (back == 0) {
					vertices.insert(vertices.end(),{(float)x+0,(float)y+0,(float)z+0,	0,0,-1,	1,1,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+0,(float)y+1,(float)z+0,	0,0,-1,	1,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+1,(float)z+0,	0,0,-1,	0,0,	(float)blck->textureSide});
					vertices.insert(vertices.end(),{(float)x+1,(float)y+0,(float)z+0,	0,0,-1,	0,1,	(float)blck->textureSide});

					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 0), static_cast<unsigned int>(faces*4 + 1), static_cast<unsigned int>(faces*4 + 2)});
					indices.insert(indices.end(),{static_cast<unsigned int>(faces*4 + 2), static_cast<unsigned int>(faces*4 + 3), static_cast<unsigned int>(faces*4 + 0)});
					
					faces++;
				}
			}
		}
	}

	Model c(vertices, indices, attrib);
	c.pos = 16.0f*glm::vec3(pos.x,0,pos.y);
	
	// for (int i = 0; i < vertices.size(); i++) {
	// 	cout << vertices.at(i) << " ";
	// 	if (i % 9 == 8) cout << endl;
	// }
	
	return c;
}


int* Chunk::getBlock (int x, int y, int z) {
	return &blocks[x][y][z];
}
void Chunk::setBlock (int x, int y, int z, int blockID) {
	int* b = getBlock(x,y,z);
	*b = blockID;
}