#include "World.h"
#include <thread>

int World::seed = 495804;
map<int, map<int, Chunk>> World::chunks;

map<int, Block> World::blockTypes;
map<string, Shader*> World::shaders;
map<string, unsigned int*> World::textures;
vector<Model*> World::models;
vector<LObject*> World::LogicObjects;

LObject World::Camera;
LObject World::Player;

float World::Settings::FOV = 72;
float World::Settings::renderDistance = 5;


Chunk* World::getChunk (int x, int y) {
	int cx = x/16;
	int cy = y/16;
	return &chunks[cx][cy];
}
int* World::getBlock (int x, int y, int z) {
	Chunk* c = getChunk(x,y);
	return &c->blocks[x%16][y%16][z];
}
void World::setBlock (int x, int y, int z, int block) {
	int* blck = getBlock(x,y,z);
	*blck = block;
}


//World Management Stuff
void chunkLoader () {
	glm::vec2 p((int)(fmod(World::Player.pos.x,16.0f)), (int)(fmod(World::Player.pos.y,16.0f)));
	for (int x = p.x-World::Settings::renderDistance; x <= p.x+World::Settings::renderDistance; x++) {
		map<int,Chunk> cx = World::chunks[x];
		for (int z = p.y-World::Settings::renderDistance; z <= p.y+World::Settings::renderDistance; z++) {
			auto c = cx.find(z);
			if (c == cx.end()) {
				//Check if saved (not added yet)
				//Else generate new chunk
				World::chunks[x][z] = Chunk::genChunk(x,z);
			}
		}
	}
	for (auto& [key, cx] : World::chunks) { //load/unload chunks already in memory
		for (auto& [key2, cMem] : cx) {
			Chunk* c = &cMem;
			float distance = sqrtf(powf(p.x - c->pos.x,2) + powf(p.y - c->pos.y,2));
			if (distance > World::Settings::renderDistance) {
				//unload
				if (c->loaded) {
					c->loaded = false;
					delete c->mesh;
					c->mesh = new Model();
				}
			} else {
				//load
				if (!c->loaded) {
					*c->mesh = c->genMesh();
					c->mesh->shader = World::shaders["world"];
					c->mesh->textures.push_back(*World::textures["atlas"]);
					c->loaded = true;
				}
			}
		}
	}

	// auto c1 = World::chunks[0].find(0);
	// if (c1 == World::chunks[0].end()) {
	// 	World::chunks[0][0] = Chunk::genChunk(0,0);
	// }

	// Chunk* c = &World::chunks[0][0];
	// if (!c->loaded) {
	// 	*c->mesh = c->genMesh();
	// 	c->mesh->shader = World::shaders["world"];
	// 	c->mesh->textures.push_back(*World::textures["atlas"]);
	// 	c->loaded = true;
	// }
}
void worldSetup () { //called by the loading functions
	//Chunk Rendering
	LObject chunkRender;
	chunkRender.onTick = [&](){
		for (auto& [key, cx] : World::chunks) {
			for (auto& [key2, cMem] : cx) {
				Chunk* c = &cMem;
				if (c->loaded) {
					c->mesh->draw();
				};
			}
		}
	};

	//Check if player moves between chunks
	LObject chunkChecker;
	glm::vec2 lastChunk((int)(fmod(World::Player.pos.x,16.0f)), (int)(fmod(World::Player.pos.y,16.0f)));
	chunkChecker.onTick = [&](){
		glm::vec2 c((int)(fmod(World::Player.pos.x,16.0f)), (int)(fmod(World::Player.pos.y,16.0f)));
		if (!(c.x == lastChunk.x && c.y == lastChunk.y)) thread chunkThread(chunkLoader);
		lastChunk = c;
	};
}

void World::loadNew (int seed) {
	World::seed = seed;
	chunkLoader();
	Chunk spawnC = World::chunks[0][0];
	for (int y = 0; y < 64; y++) {
		if (spawnC.blocks[8][y+2][8] == 0) {
			World::Camera.pos = glm::vec3(8,y,8); //change this after everything works
			break;
		}
	}

	worldSetup();
}