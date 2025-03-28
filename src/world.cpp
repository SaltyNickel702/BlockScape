#include "World.h"

map<int, map<int, Chunk>> World::chunks;
map<int, Block> World::blockTypes;
vector<Model*> World::models;

Chunk* World::getChunk (int x, int y) {
	int cx = x/16;
	int cy = y/16;
	return &World::chunks[cx][cy];
}
int* World::getBlock (int x, int y, int z) {
	Chunk* c = World::getChunk(x,y);
	return &c->blocks[x%16][y%16][z];
}
void World::setBlock (int x, int y, int z, int block) {
	int* blck = World::getBlock(x,y,z);
	*blck = block;
}