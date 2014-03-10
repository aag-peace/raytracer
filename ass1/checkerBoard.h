#pragma once

#include "wingedEdge.h"

class checkerBoard
{
	float minx,miny,maxx,maxy,z;
	int num_square;
	float box_length;
	wingedEdge* w;
public:
	checkerBoard(wingedEdge * we);
	~checkerBoard(void);
	void setColor(vec3<float> p);
};

