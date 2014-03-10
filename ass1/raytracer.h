#pragma once
#include "wingedEdge.h"
#include "vec3.h"
#include "checkerBoard.h"

#define INFINITY 1e8

using namespace std;


class raytracer
{
	int width;
	int height;
	vector<Light> lights;
public:

	checkerBoard *board;
	raytracer();
	~raytracer();
	void set_img_size(unsigned int x, unsigned int y);
	void render(vector<wingedEdge*> &objects);
	vec3<float> trace(const vec3<float> &o, vec3<float> &dir, vector<wingedEdge*> &objects, const int depth);
};

