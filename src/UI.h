#include <iostream>
#include <glm/glm.hpp>
#include <functional>
#include <vector>

#include "Logic.h"
#include "Model.h"
#include "GameState.h"

#ifndef UI_H
#define UI_H

//UI Management
namespace UI {
	class Image { //Static Image to be displayed using a menu or button
		public:
			Image() = delete;
			Image(unsigned int textureID, float x, float y, float w, float h);

			void setPos(float x, float y);
			void setDim(float w, float h);
						
			Model* imgMesh;

			glm::vec2 pos;
			glm::vec2 dim;
	};

	class Button { //Rectangular hitbox that checks if selected image is hovered/clicked on. Auto renders selected Image using Menu
		public:
			Button() = delete;
			Button(Image* image);
			Button(Image* image, function<void()> onClick, function<void()> onHover, function<void()> onLeave);

			vector<Image*> images;
			int currentImg;

			bool hovering;
			function<void()> onClick; //once on click
			function<void()> onHover; //once on hover
			function<void()> onLeave; //once on leave
	};

	class Font {
		public:
			Font() = delete;
			Font(std::string rel, int width, int height);
			
			std::string chars;
			
			int w,h;
			unsigned int ID;
	};
	class Text {
		private:
			float w,h;
			Font* f;

		public:
			Text() = delete;
			Text(Font* font, float x, float y);

			Model* mesh;
			std::string text; //Do no set, reference only. Use setText method to modify
			float x,y;

			void setFont (Font* font);
			void setHeight (float height); //Sets height, and adjusts width to maintain font aspect ratio
			void setWidth (float width); //Sets width, and adjusts height to maintain font aspect ratio
			void genMesh ();
			void setText (std::string text);
			void setPos (float x, float y);
	};

	class Menu { //Collection of Buttons and Images that should be drawn to the screen
		public:
			Menu();

			vector<Button*> buttons;
			vector<Image*> images;
			vector<Text*> texts;

			bool visible;
			vector<GameState::State> activeStates;
	};

	extern vector<Menu*> menus;
	extern LObject menuTick;
	extern bool hoveringOverButton;
}

#endif