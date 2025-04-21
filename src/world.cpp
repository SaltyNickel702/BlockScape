#include "World.h"
#include "Engine.h"
#include <cstdint>

int World::seed = 495804;
map<int, map<int, Chunk>> World::chunks;

map<int, Block> World::blockTypes;
map<string, Shader*> World::shaders;
map<string, unsigned int*> World::textures;
map<string, UI::Menu*> World::menus;
vector<LObject*> World::LogicObjects;

LObject World::Camera;
LObject World::Player;
namespace World::PlayerData {
	GameMode CurrentMode = GameMode::SPECTATOR;
	bool Flying = false;
}

float World::Settings::FOV = 72;
int World::Settings::renderDistance = 20;


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
	if (*blck == block) return;
	*blck = block;
	getChunk(x,z)->modified = true;
}



//World Management Stuff
bool loadingChunks = false;
vector<glm::vec2> chunkMeshGenQueue;
vector<glm::vec2> chunkMeshDelQueue;
void chunkLoader () {
	while (loadingChunks) { //so only one can run at a time

	}
	loadingChunks = true;
	int totalChunks = 0;
	glm::vec2 p((int)(World::Player.pos.x/16), (int)(World::Player.pos.z/16));
	for (int x = 0; x <= 2 * World::Settings::renderDistance + 2; x++) {
		for (int z = 0; z <= 2 * World::Settings::renderDistance + 2; z++) { //load block data into memory

			int cx = x - World::Settings::renderDistance - 1 + (int)p.x;
			int cz = z - World::Settings::renderDistance - 1 + (int)p.y;

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
			float distance = sqrtf(powf(p.x - 0 - c->pos.x,2) + powf(p.y - 0 - c->pos.y,2));
			if (distance > World::Settings::renderDistance) {
				//unload mesh
				if (c->loaded) {
					// cout << "Cleaning" << endl;
					chunkMeshDelQueue.push_back(c->pos);
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
	// cout << "Chunk Mesh generated" << endl;
	// cout << totalChunks << endl;
	loadingChunks = false;
}
int* lastPlayerChunk;
void worldSetup () { //called by the loading functions
	Engine::addKeydownCallback(GLFW_KEY_R,[&](){
		while (World::chunks.size() > 0) {
			World::chunks.erase(World::chunks.begin());
        }
		chunkLoader();
    });

	// Chunk Rendering
	LObject* chunkRender = new LObject(); //declares new object that isn't deleted after function (dynamically allocated)
	chunkRender->onTick = [&]() {
		for (auto& [key, cx] : World::chunks) {
			for (auto& [key2, cMem] : cx) {
				Chunk* c = &cMem;
				if (c->loaded) {
					c->mesh->draw();
				}
			}
		}
	};
	chunkRender->activeStates = vector<GameState::State> {GameState::State::PLAYING,GameState::State::PAUSE};

	lastPlayerChunk = new int[2]{(int)World::Player.pos.x/16, (int)World::Player.pos.z/16};
	LObject* chunkBlockGen = new LObject();
	chunkBlockGen->onTick = [&]() {
		int curChunk[2] = {(int)World::Player.pos.x/16, (int)World::Player.pos.z/16};
		if (!(lastPlayerChunk[0] == curChunk[0] && lastPlayerChunk[1] == curChunk[1])) {
			// cout << "Entered Chunk: " << curChunk[0] << " " << curChunk[1] << endl;
			// cout << "Last Chunk: " << lastPlayerChunk[0] << " " << lastPlayerChunk[1] << endl;
			thread* chunkLoading = new thread(chunkLoader); //test later
		}

		lastPlayerChunk[0] = curChunk[0];
		lastPlayerChunk[1] = curChunk[1];
	};
	chunkBlockGen->activeStates = vector<GameState::State> {GameState::State::PLAYING}; //No need to load chunks when player is unable to move

	LObject* chunkMeshGen = new LObject();
	chunkMeshGen->onTick = [&]() {
		int chunksPerTick = 10;
		int chunksLeft = (chunkMeshGenQueue.size() > chunksPerTick ? chunksPerTick : chunkMeshGenQueue.size());
		while (chunksLeft--) {
			glm::vec2 coords = chunkMeshGenQueue.at(0);
			Chunk* c = &World::chunks[coords.x][coords.y];
			c->genMeshGL();
			c->loaded = true;
			chunkMeshGenQueue.erase(chunkMeshGenQueue.begin());
		}
		//delete chunks
		while (chunkMeshDelQueue.size() > 0) {
			glm::vec2 coords = chunkMeshDelQueue.at(0);
			Chunk* c = &World::chunks[coords.x][coords.y];
			c->loaded = false;
			c->mesh->cleanData();
			chunkMeshDelQueue.erase(chunkMeshDelQueue.begin());
		}
	};
	chunkMeshGen->activeStates = vector<GameState::State> {GameState::State::PLAYING,GameState::State::PAUSE};
}

void World::loadNew (int seed) {
	World::seed = seed;
	chunkLoader();

	Chunk spawnC = World::chunks[0][0];
	for (int y = 0; y < 128; y++) {
		if (spawnC.blocks[7][y][7] == 0) {
			World::Player.pos = glm::vec3(7.5,y,7.5); //change this after everything works
			break;
		}
	}

	worldSetup();
}
void World::saveGame (string saveFolder) {
	using namespace World::PlayerData;
	string sF = "./saves/" + saveFolder;

	if (!filesystem::exists(sF)) {
		filesystem::create_directory(sF);
	}

	
	#pragma region 
	ofstream playerBSF(sF+"/player.ini",ofstream::trunc); //erase previous contents
	vector<string> playerBS;

	string posData = "pos=" + to_string(World::Player.pos.x) + "," + to_string(World::Player.pos.y) + "," + to_string(World::Player.pos.z);
	playerBS.push_back(posData);
	
	string rotData = "rot=" + to_string(World::Player.rot.x) + "," + to_string(World::Player.rot.y);
	playerBS.push_back(rotData);

	if (playerBSF.is_open()) {
		for (int i = 0; i < playerBS.size(); i++) {
			playerBSF << playerBS.at(i);
			if (i != playerBS.size()-1) {
				playerBSF << "\n";
			}
		}
		playerBSF.close();
	}
	#pragma endregion


	#pragma region
	ofstream configBSF(sF+"/config.ini",ofstream::trunc);
	vector<string> configBS;

	string seed = "seed=" + to_string(World::seed);
	configBS.push_back(seed);

	if (configBSF.is_open()) {
		for (int i = 0; i < configBS.size(); i++) {
			configBSF << configBS.at(i);
			if (i != configBS.size()-1) {
				configBSF << "\n";
			}
		}
		configBSF.close();
	}
	#pragma endregion


	#pragma region
	ofstream worldBSF(sF + "/world.dat", ios::binary | ofstream::trunc);

	for (auto& [cx, mx] : World::chunks) {
		for (auto& [cy, cMem] : mx) {
			Chunk* c = &cMem;
			if (!c->modified) continue;

			//write coords
			uint32_t cx32 = cx;
			for (int i = 0; i < 4; ++i) {
				uint8_t byte = (cx32 >> (i * 8)) & 0xFF;
				worldBSF.write(reinterpret_cast<char*>(&byte), 1);
			}
			uint32_t cy32 = cy;
			for (int i = 0; i < 4; ++i) {
				uint8_t byte = (cy32 >> (i * 8)) & 0xFF;
				worldBSF.write(reinterpret_cast<char*>(&byte), 1);
			}
			
			for (int y = 0; y < 128; y++) {
				for (int z = 0; z < 16; z++) {
					for (int x = 0; x < 16; x++) {
						uint8_t byte = static_cast<uint8_t>(c->blocks[x][y][z] & 0xFF);
						worldBSF.write(reinterpret_cast<char*>(&byte), 1);
					}
				}
			}
		}
	}
	worldBSF.close();
	#pragma endregion
}