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
	class Element {
		public:
			Element () : mesh(new Model()), clickable(false), hovering(false), x(0), y(0), w(0), h(0), onClick([](){}), onHover([](){}), onLeave([](){}), ID("") {};

			float x, y, w, h;

			Model* mesh;

			bool clickable;
			bool hovering;
			virtual bool mouseOver ();
			function<void()> onClick; //once on click
			function<void()> onHover; //once on hover
			function<void()> onLeave; //once on leave

			std::string ID;

			virtual void setPos (float x, float y);
			virtual void center ();

			virtual void draw () {};
			
			virtual ~Element() {
				delete mesh;
			};
	};

	class Image : public Element { //Static Image to be displayed using a menu or button
		public:
			Image() = delete;
			Image(unsigned int textureID, float x, float y, float w, float h);

			unsigned int textureID;

			void setDim (float w, float h);
						
			void draw () override;
	};

	class Font {
		public:
			Font() = delete;
			Font(std::string rel, int width, int height);
			
			std::string chars;
			
			int w,h;
			unsigned int ID;
	};
	class Text : public Element {
		public:
			Text() : f(nullptr) {};
			Text(Font* font, float x, float y);

			Font* f;
			std::string text; //Do no set, reference only. Use setText method to modify

			float cW, cH;

			bool editing = false;
			bool cursorVisible = false;
			float elapsedTime = 0;
			float cursorTickRate = 2;

			void setFont (Font* font);
			void setHeight (float height); //Sets height, and adjusts width to maintain font aspect ratio
			void setWidth (float width); //Sets width, and adjusts height to maintain font aspect ratio
			void genMesh ();
			void setText (std::string text);

			void draw () override;
	};
	class Textbox : public Element {
		private:
			void updateBackground();
		public:
			Textbox() = delete;
			Textbox(string text, Font* f, int maxLength, float x, float y);

			Text* text;
			Image* background;
			int maxCharacterLength;

			void setHeight (float height); //Sets height, and adjusts width to maintain font aspect ratio
			void setWidth (float width); //Sets width, and adjusts height to maintain font aspect ratio
			void setPos (float x, float y) override;
			void setMaxLength (int length);
			void setBackground (unsigned int id);

			void draw () override;

			~Textbox () override {
				delete mesh;
				delete text;
				if (background) delete background;
			};
	};

	class Menu { //Collection of Buttons and Images that should be drawn to the screen
		public:
			Menu();

			vector<Element*> elements;

			bool visible;
			vector<GameState::State> activeStates;

			function<void()> onTick;

			Element* getByID (std::string ID) {
				for (Element* e : elements) {
					if (e->ID == ID) return e;
				}
				return nullptr;
			}

			~Menu () {
				for (Element* e : elements) delete e;
			}
	};

	extern vector<Menu*> menus;
	extern LObject menuTick;
	extern bool hoveringOverButton;
}

#endif