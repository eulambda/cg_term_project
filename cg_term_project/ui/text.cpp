#include "text.hpp"

void Text::set_content(const char* str) {
	gltSetText(glt_text, str);
}

void Text::set_color(double r, double g, double b, double a){
	this->r = (float)r;
	this->g = (float)g;
	this->b = (float)b;
	this->a = (float)a;
}

void Text::set_transform(double x, double y, double z, double scale) {
	this->x = (float)x;
	this->y = (float)y;
	this->z = (float)z;
	this->scale = (float)scale;
}
