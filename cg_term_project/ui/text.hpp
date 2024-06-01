#include <glad/glad.h> 
#define GLT_IMPLEMENTATION
#include "../gltext.h"

struct Text {
	GLTtext* glt_text;
	float x, y, z, scale;
	float r{ 1 }, g{ 1 }, b{ 1 }, a{ 1 };
	void set_content(const char* str);
	void set_color(double r, double g, double b, double a);
	void set_transform(double x, double y, double z, double scale);
};