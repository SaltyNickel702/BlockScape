#include "Sound.h"
#include <SDL.h>
#include <SDL_mixer.h>
#include <iostream>
#include <string>
#include <map>
#include <thread>

using namespace std;

int soundSize = 1; 
string sounds[1] = {"goofy"}; //List all sounds with their name of files here (convert wav files to mp3 if needed)
map<string, Mix_Music*> loadedSounds;

void loadSounds(){
    for(int i = 0;i < soundSize;i++){ //Loads every sound first so there's no delay for them to play (in theory)
        string soundPath = "./assets/sounds/" + sounds[i] + ".mp3";

        Mix_Music* music = Mix_LoadMUS((soundPath).c_str());
        if (!music) {
            cerr << "Mix_LoadMUS error: " << Mix_GetError() << "\n";
            return;
        }

        loadedSounds.insert({sounds[i], music});
    }
}

void playSound(string sound){
    Mix_PlayMusic(loadedSounds[sound], 1);
}

void playSound(string sound, float volume){
    Mix_Chunk* soundEffect = Mix_LoadWAV("sound_effect.wav");
    Mix_VolumeChunk(soundEffect, MIX_MAX_VOLUME / 2); // Set sound effect volume to 50%
    while (Mix_PlayingMusic()) {
        SDL_Delay(100);
    }
}

void changeVolume(float volume){
    Mix_VolumeMusic(MIX_MAX_VOLUME / (100.0 / volume));
}

void loopSound(string sound){
    Mix_PlayMusic(loadedSounds[sound], -1);
}