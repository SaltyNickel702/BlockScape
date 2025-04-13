#include <FastNoise/FastNoiseLite.h>
#include "Chunk.h"
#include "World.h"

Chunk::Chunk () : loaded(false) {
	mesh = new Model();
	mesh->shader = World::shaders["world"];
	mesh->textures.push_back(*World::textures["atlas"]);
}

Chunk Chunk::genChunk (int cx, int cz) {
	Chunk c;
	c.pos = glm::vec2(cx,cz); //assign position for reference
	c.mesh->pos = glm::vec3(cx*16,0,cz*16);


	int seed = World::seed;

	FastNoiseLite noise;  // Baseline noise map
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin); 
	noise.SetFrequency(0.015f);
	noise.SetSeed(seed); 
	noise.SetFractalType(FastNoiseLite::FractalType_FBm);
	noise.SetFractalOctaves(4.0f);
	noise.SetFractalLacunarity(2.00f);
	noise.SetFractalGain(0.58f);
	noise.SetFractalWeightedStrength(0.480f);

	FastNoiseLite domainWarp;
	domainWarp.SetDomainWarpType(FastNoiseLite::DomainWarpType_OpenSimplex2);
	domainWarp.SetSeed(seed);
	domainWarp.SetFrequency(0.115);
	domainWarp.SetDomainWarpAmp(6);

	FastNoiseLite generalHeight; //height varaition
	generalHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	generalHeight.SetSeed(seed);
	generalHeight.SetFrequency(0.003);

	FastNoiseLite addedHeight; //steepness
	addedHeight.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
	addedHeight.SetSeed(seed+1);
	addedHeight.SetFrequency(0.005);
 
	
	// Generates terrain
	for (int x = 0; x < 16; x++) {
		for (int z = 0; z < 16; z++) {
			// Get height value and scale it to terrain height
			float nX = (float)(x + cx*16);
			float nZ = (float)(z + cz*16);
			domainWarp.DomainWarp(nX,nZ);
			float heightMulti = (generalHeight.GetNoise(nX,nZ)+1)/(2);
			float heightAdd = (addedHeight.GetNoise(nX,nZ)+1)/(2);
			float noiseVal = (noise.GetNoise(nX, nZ)+1)/2;
			
			int terrainHeight = (int)((noiseVal * heightMulti*.7 + heightAdd*.3) * 127);
			for (int y = 0; y < 128; y++) {
				// Assign blocks based on Z level
				if (y > terrainHeight) {
					c.blocks[x][y][z] = 0;  
				}
				else if (heightMulti * noiseVal > powf(.67,2)) {
					//Mountains
					if (heightMulti * noiseVal > powf(.7,2)) {
						c.blocks[x][y][z] = 3;
					} else {
						c.blocks[x][y][z] = 2;
					}
				}
				else if (heightMulti * noiseVal * heightAdd < powf(.4,3)) {
					//Desert
					c.blocks[x][y][z] = 4; //currently logs, make sand
				}
				else {
					//Plains
					if (y == terrainHeight) {
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
	}

	return c;
}

void Chunk::genMeshParam() {
	Chunk* leftC = World::getChunkByCC(pos.x+1,pos.y);
	Chunk* rightC = World::getChunkByCC(pos.x-1,pos.y);
	Chunk* frontC = World::getChunkByCC(pos.x,pos.y+1);
	Chunk* backC = World::getChunkByCC(pos.x,pos.y-1);

	int faces = 0;
	for (int x = 0; x < 16; (float)x++) {
		for (int y = 0; y < 128; (float)y++) {
			for (int z = 0; z < 16; (float)z++) {
				int ths = blocks[x][y][z];
				if (ths == 0) continue;
				Block* blck = &World::blockTypes[ths]; //pointer bc more memory efficient. No new block class for each block

				int left = (x < 15 ? blocks[x+1][y][z] : ((leftC == nullptr) ? -1 : *leftC->getBlock(0,y,z))); //-1 means not present | replace -1 with other side chunk block for x and z
				int right = (x > 0 ? blocks[x-1][y][z] : ((rightC == nullptr) ? -1 : *rightC->getBlock(15,y,z)));
				
				int up = (y < 127 ? blocks[x][y+1][z] : 0); //assume air is above y:63
				int down = (y > 0 ? blocks[x][y-1][z] : 0); //assume air is below y:0

				int front = (z < 15 ? blocks[x][y][z+1] : ((frontC == nullptr) ? -1 : *frontC->getBlock(x,y,0)));
				int back = (z > 0 ? blocks[x][y][z-1] : ((backC == nullptr) ? -1 : *backC->getBlock(x,y,15)));
				
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
}
void Chunk::genMeshGL () {
	mesh->setData(vertices,indices,attrib);
}


int* Chunk::getBlock (int x, int y, int z) {
	return &blocks[x][y][z];
}
void Chunk::setBlock (int x, int y, int z, int blockID) {
	int* b = getBlock(x,y,z);
	*b = blockID;
}