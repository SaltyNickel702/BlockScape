#include "UI.h"
#include "Engine.h"
#include "World.h"
#include <cctype>

namespace UI {
	vector<Menu*> menus;
	LObject menuTick;
	bool hoveringOverButton = false;


	bool Element::mouseOver () {
		if (!Engine::cursorEnabled) return false;
		glm::vec2 c = Engine::cursorPos;
		if (c.x >= x && c.x <= x + w && c.y >= y && c.y <= y + h) return true;
		return false;
	}
	void Element::setPos (float x, float y) {
		this->x = x;
		this->y = y;
		mesh->pos = glm::vec3(x,y,0);
	}
	void Element::center () {
		glm::vec2 newPos = glm::vec2(x,y) - .5f*glm::vec2(w,h);
		setPos(newPos.x, newPos.y);
	}

	Image::Image (unsigned int textureID, float x, float y, float w, float h) {
		mesh->shader = World::shaders["menu"];
		mesh->textures.push_back(textureID);

		setDim(w,h);
		setPos(x,y);
	}
	void Image::setDim (float w, float h) {
		this->w = w;
		this->h = h;
		glm::vec2 dim = glm::vec2(w,h);
		setPos(x,y);

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
		
		mesh->setData(vert,indices,attr);
	}
	void Image::draw () {
		mesh->draw();
	}


	Font::Font (std::string rel, int width, int height) {
		w = width;
		h = height;

		ID = Engine::genTexture(rel);
	}
	Text::Text (Font* font, float x, float y) : text("") {
		f = font;

		mesh = new Model();
		mesh->shader = World::shaders["text"];

		setHeight(f->h);

		setPos(x,y);
	}
	void Text::setFont (Font* font) {
		f = font;

		if (mesh != nullptr) mesh = new Model();

		setHeight(h);
	}
	void Text::setHeight (float h) {
		cH = h;
		cW = f->w/(float)f->h * cH;

		this->h = h;
		this->w = cW * text.size();

		Text::genMesh();
	}
	void Text::setWidth (float w) {
		cW = w;
		cH = f->h/(float)f->w * cW;

		this->h = cH;
		this->w = cW * text.size();

		Text::genMesh();
	}
	void Text::genMesh () {
		this->w = cW * text.size();

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
			
			vertices.insert(vertices.end(),{
				i*cW,0,			0,0,	(float)id,
				i*cW,cH,		0,1,	(float)id,
				i*cW + cW,cH,	1,1,	(float)id,
				i*cW + cW,0,	1,0,	(float)id
			});
			
			vector<float> ind {0,1,2,	0,2,3};
			for (float &v : ind) v+=totalVert*4;
			indices.insert(indices.end(), ind.begin(), ind.end());
			totalVert++;
		}
		if (cursorVisible) {
			vertices.insert(vertices.end(),{
				0.15f*cW + text.size()*cW,0,					0,0,	(float)-1,
				0.15f*cW + text.size()*cW,.9f*cH,				0,1,	(float)-1,
				0.15f*cW + text.size()*cW + 0.15f*cW,.9f*cH,	1,1,	(float)-1,
				0.15f*cW + text.size()*cW + 0.15f*cW,0,			1,0,	(float)-1,
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
	void Text::draw () {
		if (!mesh->dataFormatted || mesh->VAO == 0 || mesh->totalIndices == 0 || f == nullptr) return;
		glUseProgram(mesh->shader->ID);
		mesh->shader->uniforms(mesh->pos,mesh->rot);

		glUniform1f(glGetUniformLocation(mesh->shader->ID, "len"), f->chars.size());


		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, f->ID);
		glUniform1i(glGetUniformLocation(mesh->shader->ID,"tex0"),0);


		glBindVertexArray(mesh->VAO);
		glDrawElements(GL_TRIANGLES,mesh->totalIndices,GL_UNSIGNED_INT, 0);

		glBindVertexArray(0);
	}
	
	Textbox::Textbox (string text, Font* f, int maxLength,  float x, float y) : background(nullptr) {
		clickable = true;

		this->text = new Text(f, x, y);
		this->text->setText(text);

		setMaxLength(maxLength);
		h = this->text->h;

		setPos(x,y);
	}
	void Textbox::draw () {
		if (background) background->draw();
		text->draw();
	}
	void Textbox::setPos (float x, float y) {
		this->x = x;
		this->y = y;
		mesh->pos = glm::vec3(x,y,0);

		text->setPos(x,y);
	}
	void Textbox::setHeight (float height) {
		text->setHeight(height);

		w = text->cW * maxCharacterLength;
		h = text->h;
		updateBackground();
	}
	void Textbox::setWidth (float width) {
		text->setWidth(width);

		w = text->cW * maxCharacterLength;
		h = text->h;
		updateBackground();
	}
	void Textbox::setMaxLength (int length) {
		maxCharacterLength = length;

		w = text->cW * maxCharacterLength;
		updateBackground();
	}
	void Textbox::setBackground (unsigned int id) {
		if (background) delete background;
		background = new Image(id,x,y,w,h);
		Textbox::updateBackground();
	}
	void Textbox::updateBackground () {
		if (!background) return;
		float padding = 0.15 * (w > h ? h : w);

		background->setPos(x - padding,y - padding);
		background->setDim(w + 2*padding,h + 2*padding);
	}


	Menu::Menu () : visible(false) {
		menus.push_back(this);
	}
}