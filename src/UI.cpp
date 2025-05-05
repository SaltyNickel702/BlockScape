#include "UI.h"
#include "Engine.h"
#include "World.h"
#include <cctype>

namespace UI {
	vector<Menu*> menus;
	LObject menuTick;
	bool hoveringOverButton = false;

	Image::Image (unsigned int textureID, float x, float y, float w, float h) : imgMesh(new Model()) {
		imgMesh->shader = World::shaders["menu"];
		imgMesh->textures.push_back(textureID);

		setDim(w,h);
		setPos(x,y);
	}
	void Image::setDim (float w, float h) {
		dim = glm::vec2(w,h);
		setPos(pos.x,pos.y);

		vector<float> vert {
			0,0,	0,0,
			w,0,	1,0,
			w,h,	1,1,
			0,h,	0,1
		};
		vector<unsigned int> indices {
			2,1,0,
			3,2,0
		};
		vector<unsigned int> attr {
			2,2
		};
		
		imgMesh->setData(vert,indices,attr);
	}
	void Image::setPos (float x, float y) {
		pos = glm::vec2(x,y);
		imgMesh->pos = glm::vec3(pos - .5f*dim,0);
	}


	Button::Button (Image* img) : currentImg(0), hovering(false), onClick([&](){}), onHover([&](){}), onLeave([&](){}) {
		images.push_back(img);
	}
	Button::Button (Image* img, function<void()> onClick, function<void()> onHover, function<void()> onLeave) : currentImg(0), hovering(false) {
		images.push_back(img);

		this->onClick = onClick;
		this->onHover = onHover;
		this->onLeave = onLeave;
	}

	Font::Font (std::string rel, int width, int height, int length) {
		w = width;
		h = height;
		l = length;

		ID = Engine::genTexture(rel);
	}
	Text::Text (Font* font) {
		f = font;

		mesh = new Model();
		mesh->textures = vector<unsigned int> {f->ID};
		mesh->shader = World::shaders["text"];

		setHeight(f->h);
	}
	void Text::setFont (Font* font) {
		f = font;

		if (mesh != nullptr) mesh = new Model();
		mesh->textures = vector<unsigned int> {f->ID};

		setHeight(h);
	}
	void Text::setHeight (float h) {
		this->h = h;
		w = f->w/(float)f->h * h;
		Text::genMesh();
	}
	void Text::setWidth (float w) {
		this->w = w;
		h = f->h/(float)f->w * w;
		Text::genMesh();
	}
	void Text::genMesh () {
		
	}


	Menu::Menu () : visible(false) {
		menus.push_back(this);
	}
}