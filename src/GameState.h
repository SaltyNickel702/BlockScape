#ifndef GAMESTATE_H
#define GAMESTATE_H

//too many things reference GameState, so it's been moved to its own header file
struct GameState {
	enum class State {
		MENU,
		LOAD_SELECT,
		LOAD_FROM_SAVE,
		LOAD_NEW,
		PAUSE,
		PLAYING
	};

	static GameState::State currentState;
};

#endif