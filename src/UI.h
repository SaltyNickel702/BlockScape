#include <iostream>
#include <glm/glm.hpp>
#include <functional>
#include <vector>

#include "Logic.h"
#include "Model.h"

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

	class Menu { //Collection of Buttons and Images that should be drawn to the screen
		public:
			Menu();

			vector<Button*> buttons;
			vector<Image*> images;

			bool visible;
	};

	extern vector<Menu*> menus;
	extern LObject menuTick;
}

#endif