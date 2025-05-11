#ifndef SOUND_H
#define SOUND_H

#include <string>

void loadSounds();
void playSound(std::string sound);
void playSound(std::string, float volume); //for louder or quieter sounds
void changeVolume(float volume);

#endif