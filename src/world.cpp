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
int World::Settings::renderDistance = 2;


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
			if (c == nullptr || true) { 
				totalChunks++;
				//Check if saved (not added yet)
	 			//Else generate new chunk
				World::chunks[cx][cz] = Chunk::genChunk(cx,cz);
			};
		}
	}

	// for (auto& [key, cx] : World::chunks) {
	// 	for (auto& [key2, cMem] : cx) {
	// 		Chunk* c = &cMem;
	// 		float distance = sqrtf(powf(p.x+.5 - c->pos.x,2) + powf(p.y+.5 - c->pos.y,2));
	// 		if (distance > World::Settings::renderDistance) {
	// 			//unload mesh
	// 			if (c->loaded) {
	// 				c->loaded = false;
	// 				c->mesh.cleanData(); //frees up GPU memory
	// 			}
	// 		} else {
	// 			//load mesh
	// 			if (!c->loaded) {
	// 				c->genMeshParam();
	// 				chunkMeshGenQueue.push_back(c->pos);
	// 			}
	// 		}
	// 	}
	// }
	cout << totalChunks << endl;
	loadingChunks = false;
}
void worldSetup () { //called by the loading functions
	// Chunk Rendering
	LObject chunkRender;
	chunkRender.onTick = [&]() {
		cout << "renderer" << endl;
	};
	// chunkRender.onTick = [&](){
	// 	for (auto& [key, cx] : World::chunks) {
	// 		for (auto& [key2, cMem] : cx) {
	// 			Chunk* c = &cMem;
	// 			if (c->loaded) {
	// 				c->mesh.draw();
	// 			};
	// 		}
	// 	}
	// };

	// //Check if player moves between chunks
	LObject chunkChecker;
	chunkChecker.onTick = [&]() {
		cout << "checker" << endl;
	};
	// // glm::vec2 lastChunk((int)(World::Player.pos.x/16), (int)(World::Player.pos.z/16));
	// chunkChecker.onTick = [&](){
	// 	// glm::vec2 c((int)(World::Player.pos.x/16), (int)(World::Player.pos.z/16));
	// 	// if (!(c.x == lastChunk.x && c.y == lastChunk.y)) {
	// 	// 	cout << "Entered Chunk: " << c.x << " " << c.y << endl;
	// 	// 	thread t(chunkLoader);
	// 	// 	t.detach();
	// 	// };
	// // 	lastChunk = c;
	// // 	//generate meshes
	// // 	// int checksPerTick = (chunkMeshGenQueue.size() >= 3 ? 3 : chunkMeshGenQueue.size()); //adjust how many chunks can be loaded per tick
	// // 	// while (checksPerTick--) {
	// // 	// 	glm::vec2 p = chunkMeshGenQueue.at(0);
	// // 	// 	Chunk* c = World::getChunkByCC(p.x,p.y);
	// // 	// 	c->genMeshGL();
	// // 	// 	c->loaded = true;
	// // 	// 	chunkMeshGenQueue.erase(chunkMeshGenQueue.begin());
	// // 	// }
	// };

	cout << World::LogicObjects.size() << endl;
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