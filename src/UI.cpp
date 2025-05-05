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
		vector<float> vertices;
		vector<unsigned int> indices;
		vector<unsigned int> attr {2,2,1}; //vec2 pos, vec2 UV, int character ID

		int totalVert = 0;
		for (int i = 0; i < text.size(); i++) {
			auto it = find(f->chars.begin(), f->chars.end(), text.at(i));
			int id;
			if (it != f->chars.end()) {
				id = it - f->chars.begin();
			} else continue;
			
			totalVert++;
			vertices.insert(vertices.end(),{
				i*w,0,		0,0,	(float)id,
				i*w,h,		0,1,	(float)id,
				i*w + w,h,	1,1,	(float)id,
				i*w + w,0,	1,0,	(float)id
			});
			
			vector<float> ind {0,1,2,	0,2,3};
			for (float &f : ind) f+=totalVert*4;
			indices.insert(indices.end(), ind.begin(), ind.end());
		}
		mesh->setData(vertices,indices,attr);
	}
	void Text::setText (std::string text) {
		this->text = text;
		genMesh();
	}


	Menu::Menu () : visible(false) {
		menus.push_back(this);
	}
}