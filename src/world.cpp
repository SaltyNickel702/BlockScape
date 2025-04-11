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
int World::Settings::renderDistance = 10;


Chunk* World::getChunk (float x, float z) { //In world coords
	int cx = (int)x/16.0f;
	int cz = (int)z/16.0f;
	return getChunkByCC(cx,cz);
}
Chunk* World::getChunkByCC (int cx, int cz) {
	map<int, Chunk>* xMap = &chunks[cx];
	auto cTest = xMap->find(cz);
	if (cTest != xMap->end()) {
		return &chunks[cx][cz];
	};
	return nullptr;
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
bool loadingChunks = false;
vector<glm::vec2> chunkMeshGenQueue;
void chunkLoader () {
	while (loadingChunks) { //so only one can run at a time

	}
	loadingChunks = true;
	int totalChunks = 0;
	glm::vec2 p((int)(World::Player.pos.x/16), (int)(World::Player.pos.z/16));
	for (int x = 0; x <= 2 * World::Settings::renderDistance; x++) {
		for (int z = 0; z <= 2 * World::Settings::renderDistance; z++) { //load block data into memory

			int cx = x - World::Settings::renderDistance + (int)p.x;
			int cz = z - World::Settings::renderDistance + (int)p.y;

			Chunk* c = World::getChunkByCC(cx,cz);
			if (c == nullptr) {
				totalChunks++;
				//Check if saved (not added yet)
	 			//Else generate new chunk
				World::chunks[cx][cz] = Chunk::genChunk(cx,cz);
			};
		}
	}

	for (auto& [key, cx] : World::chunks) {
		for (auto& [key2, cMem] : cx) {
			Chunk* c = &cMem;
			float distance = sqrtf(powf(p.x+.5 - c->pos.x,2) + powf(p.y+.5 - c->pos.y,2));
			if (distance > World::Settings::renderDistance) {
				//unload mesh
				if (c->loaded) {
					c->loaded = false;
					c->mesh.cleanData(); //frees up GPU memory
				}
			} else {
				//load mesh
				if (!c->loaded) {
					c->genMeshParam();
					chunkMeshGenQueue.push_back(c->pos);
				}
			}
		}
	}
	// cout << totalChunks << endl;
	loadingChunks = false;
}
int* lastPlayerChunk;
void worldSetup () { //called by the loading functions
	// Chunk Rendering
	LObject* chunkRender = new LObject(); //declares new object that isn't deleted after function (dynamically allocated)
	chunkRender->onTick = [&]() {
		cout << "Render" << endl;
		for (auto& [key, cx] : World::chunks) {
			for (auto& [key2, cMem] : cx) {
				Chunk* c = &cMem;
				if (c->loaded) {
					c->mesh.draw();
				}
			}
		}
	};

	lastPlayerChunk = new int[2]{(int)World::Player.pos.x/16, (int)World::Player.pos.z/16};
	LObject* chunkBlockGen = new LObject();
	chunkBlockGen->onTick = [&]() {
		cout << "Load" << endl;
		int curChunk[2] = {(int)World::Player.pos.x/16, (int)World::Player.pos.z/16};
		if (!(lastPlayerChunk[0] == curChunk[0] && lastPlayerChunk[1] == curChunk[1])) {
			// cout << "Entered Chunk: " << curChunk[0] << " " << curChunk[1] << endl;
			// cout << "Last Chunk: " << lastPlayerChunk[0] << " " << lastPlayerChunk[1] << endl;
			thread chunkLoading(chunkLoader);
			chunkLoading.detach();
		}

		lastPlayerChunk[0] = curChunk[0];
		lastPlayerChunk[1] = curChunk[1];
	};

	LObject* chunkMeshGen = new LObject();
	chunkMeshGen->onTick = [&]() {
		cout << "Mesh" << endl;
		int chunksLeft = (chunkMeshGenQueue.size() > 3 ? 3 : chunkMeshGenQueue.size());
		while (chunksLeft--) {
			glm::vec2 coords = chunkMeshGenQueue.at(0);
			Chunk* c = &World::chunks[coords.x][coords.y];
			c->genMeshGL();
			c->loaded = true;
			chunkMeshGenQueue.erase(chunkMeshGenQueue.begin());
		}
	};

	// cout << World::LogicObjects.size() << endl;
}

void World::loadNew (int seed) {
	World::seed = seed;
	chunkLoader();
	// glm::vec2 p((int)(World::Player.pos.x/16), (int)(World::Player.pos.z/16));
	// for (int x = p.x-World::Settings::renderDistance; x <= p.x+World::Settings::renderDistance; x++) {
	// 	map<int,Chunk> cx = World::chunks[x];
	// 	for (int z = p.y-World::Settings::renderDistance; z <= p.y+World::Settings::renderDistance; z++) {
	// 		auto c = cx.find(z);
	// 		if (c == cx.end()) {
	// 			//Check if saved (not added yet)
	// 			//Else generate new chunk
	// 			World::chunks[x][z] = Chunk::genChunk(x,z);
	// 		}
	// 	}
	// }
	// for (auto& [key, cx] : World::chunks) { //load/unload chunks already in memory
	// 	for (auto& [key2, cMem] : cx) {
	// 		Chunk* c = &cMem;
	// 		float distance = sqrtf(powf(p.x - c->pos.x,2) + powf(p.y - c->pos.y,2));
	// 		if (distance > World::Settings::renderDistance) {
	// 			//unload
	// 			if (c->loaded) {
	// 				c->loaded = false;
	// 				c->mesh = new Model();
	// 			}
	// 		} else {
	// 			//load
	// 			if (!c->loaded) {
	// 				*c->mesh = c->genMesh();
	// 				c->mesh->shader = World::shaders["world"];
	// 				c->mesh->textures.push_back(*World::textures["atlas"]);
	// 				c->loaded = true;
	// 			}
	// 		}
	// 	}
	// }

	Chunk spawnC = World::chunks[0][0];
	for (int y = 0; y < 128; y++) {
		if (spawnC.blocks[7][y][7] == 0) {
			World::Player.pos = glm::vec3(7.5,y,7.5); //change this after everything works
			break;
		}
	}

	worldSetup();
}