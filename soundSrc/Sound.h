#ifndef SOUND_H
#define SOUND_H

#include <string>
#include <SDL.h>
#include <SDL_mixer.h>

class Sound {
    public:
        Sound () = delete;
        Sound (std::string fileName);
        Mix_Music* mixMusic;

        void play ();
        void play (int loops);
        void loop ();

        static void init ();
        static void setVolume (float volume);
};

// void loadSounds2();
// void playSound(std::string sound);
// void loopSound(std::string sound);
// void changeVolume(float volume);

#endif