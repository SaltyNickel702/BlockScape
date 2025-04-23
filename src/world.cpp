#include "World.h"
#include "Engine.h"
#include <cstdint>

int World::seed;
string World::saveName;
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
	glm::vec3 Pdim = {0.7f,1.8f,0.7f};
	bool isColliding(float x, float y, float z) {
		float width = Pdim.x / 2.0f;
		float depth = Pdim.z / 2.0f;
		float adjustedY = y + (Pdim.y);
		/*if (Engine::keyDown(GLFW_KEY_F)){
		//cout << y << endl;
		//cout << adjustedY << endl;
		}*/
		// Check 8 corners of playerss bounding box
		float checkPoints[][3] = {
			{x - width, y + 0.4f, z - depth},          // bottom corners
			{x + width, y + 0.4f, z - depth},
			{x - width, y + 0.4f, z + depth},
			{x + width, y + 0.4f, z + depth},
			{x - width, adjustedY + 0.4f, z - depth},  // top corners
			{x + width, adjustedY + 0.4f, z - depth},
			{x - width, adjustedY + 0.4f, z + depth},
			{x + width, adjustedY + 0.4f, z + depth},
			//{x, (adjustedY + 0.4f) /2.0f, z}, // Center point. decided its not really necessary.
		};
		for (float (&point)[3] : checkPoints) {
			float blockX = point[0];
			float blockY = point[1];
			float blockZ = point[2];

			int* block = World::getBlock(blockX, blockY, blockZ);
			if (block != nullptr && *block != 0) {  // If block exists and is not air
				return true;
			}
		}

		return false;
	}
}

float World::Settings::FOV = 72;
int World::Settings::renderDistance = 20;


Chunk* World::getChunk (float x, float z) { //In world coords
	int cx = floor((x >= 0 ? x : x-1)/16.0f);
	int cz = floor((z >= 0 ? z : z-1)/16.0f);
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
int* World::getBlock (float fx, float fy, float fz) {
	int x = (int)(fx);
	int y = (int)(fy);
	int z = (int)(fz);

	Chunk* c = getChunk(x,z);
	int nx = (x >= 0 ? x % 16 : x % 16 + 15);
	int nz = (z >= 0 ? z % 16 : z % 16 + 15);
	return &c->blocks[nx][y][nz];
}
void World::setBlock (float fx, float fy, float fz, int block) {
	int x = (int)(fx);
	int y = (int)(fy);
	int z = (int)(fz);

	int* blck = getBlock(x,y,z);
	if (*blck == block) return;
	*blck = block;
	
	//Update Current Chunk
	Chunk* c = getChunk(x,z);
	c->modified = true; //only for updated chunk
	c->genMeshParam();
	chunkMeshGenQueue.push_back(c->pos);

	//Update side chunks
	vector<glm::vec2> cs;
	if ((x < 0 && x % 16 + 15 == 0) || x % 16 == 0) {
		//chunk toward -x
		cs.push_back(glm::vec2(x-1,z));
	} else if ((x < 0 && x % 16 + 15 == 15) || x % 16 == 15) {
		//chunk toward +x
		cs.push_back(glm::vec2(x+1,z));
	}
	if ((z < 0 && z % 16 + 15 == 0) || z % 16 == 0) {
		//chunk toward -z
		cs.push_back(glm::vec2(x,z-1));
	} else if ((z < 0 && z % 16 + 15 == 15) || z % 16 == 15) {
		//chunk toward +z
		cs.push_back(glm::vec2(x,z+1));
	}
	for (glm::vec2 v : cs) {
		Chunk* c = getChunk(v.x,v.y);
		c->genMeshParam();
		chunkMeshGenQueue.push_back(c->pos);
	}
}



//World Management Stuff
bool loadingChunks = false;
vector<glm::vec2> World::chunkMeshGenQueue;
vector<glm::vec2> World::chunkMeshDelQueue;
void chunkLoader () {
	while (loadingChunks) { //so only one can run at a time

	}
	loadingChunks = true;
	glm::vec2 p((int)(World::Player.pos.x/16), (int)(World::Player.pos.z/16));
	for (int x = 0; x <= 2 * World::Settings::renderDistance + 2; x++) {
		for (int z = 0; z <= 2 * World::Settings::renderDistance + 2; z++) { //load block data into memory

			int cx = x - World::Settings::renderDistance - 1 + (int)p.x;
			int cz = z - World::Settings::renderDistance - 1 + (int)p.y;

			Chunk* c = World::getChunkByCC(cx,cz);
			if (c == nullptr) { //chunk not loaded into memory
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
					World::chunkMeshDelQueue.push_back(c->pos);
				}
			} else {
				//load mesh
				if (!c->loaded && c->blocksLoaded) {
					c->genMeshParam();
					World::chunkMeshGenQueue.push_back(c->pos);
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
					// cout << key << " " << key2 << endl;
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
		int chunksLeft = (World::chunkMeshGenQueue.size() > chunksPerTick ? chunksPerTick : World::chunkMeshGenQueue.size());
		while (chunksLeft--) {
			glm::vec2 coords = World::chunkMeshGenQueue.at(0);
			Chunk* c = &World::chunks[coords.x][coords.y];
			if (!c->meshReady) {
				World::chunkMeshGenQueue.push_back(coords);
			} else {
				c->genMeshGL();
				c->loaded = true;
			}
			World::chunkMeshGenQueue.erase(World::chunkMeshGenQueue.begin());
		}
		//delete chunks
		while (World::chunkMeshDelQueue.size() > 0) {
			glm::vec2 coords = World::chunkMeshDelQueue.at(0);
			Chunk* c = &World::chunks[coords.x][coords.y];
			c->loaded = false;
			c->mesh->cleanData();
			World::chunkMeshDelQueue.erase(World::chunkMeshDelQueue.begin());
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
map<string, string> readIniFile (string fileRel) {
	ifstream ini (fileRel);
	map<string, string> out;
	if (ini.is_open()) {
		string line;
		while (getline(ini,line)) {
			string id;
			bool foundID = false;
			string contents;
			for (int i = 0; i < line.size(); i++) {
				char c = line.at(i);
				if (!foundID) {
					if (c == '=') {
						foundID = true;
					} else {
						id.push_back(c);
					}
				} else {
					contents.push_back(c);
				}
			}
			out[id] = contents;
		}
	}
	ini.close();
	return out;
}
int World::loadFromSave (string saveFolder) {
	World::saveName = saveFolder;
	using namespace World::PlayerData;
	string sF = "./saves/" + saveFolder;

	if (!filesystem::exists(sF)) {
		return -1;
	}

	#pragma region 
	map<string, string> config = readIniFile(sF + "/config.ini");
	for (auto& [id, content] : config) {
		if (id == "seed") {
			try {
				int o = std::stoi(content);
				World::seed = o;
			} catch (const std::invalid_argument& e) {
			} catch (const std::out_of_range& e) {
			}
		}
	}
	#pragma endregion


	#pragma region 
	map<string, string> playerDat = readIniFile(sF + "/player.ini");
	for (auto& [id, content] : playerDat) {
		// cout << id << "=" << content << endl;
		if (id == "pos") {
			try {
				string strs[3];
				int i = 0;
				for (char c : content) {
					if (c == ',') {
						if (i < 2) i++;
					} else {
						strs[i].push_back(c);
					}
				}
				
				float pos[3];
				for (int i = 0; i < 3; i++) {
					pos[i] = stof(strs[i]);
				}
				World::Player.pos.x = pos[0];
				World::Player.pos.y = pos[1];
				World::Player.pos.z = pos[2];
			} catch (const std::invalid_argument& e) {
			} catch (const std::out_of_range& e) {
			}
		} else {
			try {
				string strs[2];
				int i = 0;
				for (char c : content) {
					if (c == ',') {
						if (i < 1) i++;
					} else {
						strs[i].push_back(c);
					}
				}
				
				float rot[2];
				for (int i = 0; i < 2; i++) {
					rot[i] = stof(strs[i]);
				}
				World::Player.rot.x = rot[0];
				World::Player.rot.y = rot[1];
			} catch (const std::invalid_argument& e) {
			} catch (const std::out_of_range& e) {
			}
		}
	}
	#pragma endregion

	#pragma region
	ifstream world(sF + "/world.dat", std::ios::binary);

    if (!world) {
        return 1;
    }

    // Move to the end to get file size
    world.seekg(0, ios::end);
    streamsize size = world.tellg();
    world.seekg(0, ios::beg);

    // Allocate and read all bytes
    std::vector<uint8_t> buffer(size);
    if (!world.read(reinterpret_cast<char*>(buffer.data()), size)) {
        // Read failed
        return 1;
    }
	int i = 0;
	int chunkSize = 8 + 128*16*16;
	uint8_t intByteMem[4];
	int pos[2];
	Chunk* curChunk;
	while (i < size) {
		int iCD = i % chunkSize; //iterator Chunk Data (includes all chunk data)
		uint8_t* b = &buffer[i];
		if (iCD < 4) {
			intByteMem[iCD] = *b;
			if (iCD == 3) {
				int32_t value = (intByteMem[0]) |
								(intByteMem[1] << 8) |
								(intByteMem[2] << 16) |
								(intByteMem[3] << 24);
				pos[0] = (int)value;
			}
		} else if (iCD < 8) {
			intByteMem[iCD-4] = *b;
			if (iCD == 7) {
				int32_t value = (intByteMem[0]) |
								(intByteMem[1] << 8) |
								(intByteMem[2] << 16) |
								(intByteMem[3] << 24);
				pos[1] = (int)value;
				World::chunks[pos[0]][pos[1]].modified = true;
				World::chunks[pos[0]][pos[1]].blocksLoaded = true;
				World::chunks[pos[0]][pos[1]].loaded = false;
				curChunk = getChunkByCC(pos[0],pos[1]);
				curChunk->pos = glm::vec2(pos[0],pos[1]); //assign position for reference
				curChunk->mesh->pos = glm::vec3(pos[0]*16,0,pos[1]*16);
			}
		} else {
			int iC = iCD-8; //subtract first 8 bits
			int y = floor(iC / 256); //16^2 = 256
			int z = floor((iC%256)/16);
			int x = iC%16;
			if (curChunk) {
				curChunk->blocks[x][y][z] = *b;
			}
		}
		i++;
	}

    world.close();
	#pragma endregion

	Chunk* t = getChunkByCC(0,0);
	chunkLoader();
	worldSetup();

	return 0;
}