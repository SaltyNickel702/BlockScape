#define STB_IMAGE_IMPLEMENTATION 
#include "Game.h" //includes all needed includes

int main () {
    // Game::init(800,800);
    thread render(Game::init,800,800);

    render.join();
}