#ifndef GAMESTATE_H
#define GAMESTATE_H

//too many things reference GameState, so it's been moved to its own header file
struct GameState {
	enum class State {
		MENU,
		GAME_SELECT,
		GAME_LOAD_SCREEN,
		PAUSE,
		PLAYING
	};

	static GameState::State currentState;
};

#endif