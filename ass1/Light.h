#pragma once
#include "vec3.h"

class Light{
public:
	vec3<float> Ia, Id, Is,pos;

	Light(){
		Ia = vec3<float>(0.5,0.5,0.5);
		Id = vec3<float>(0.6,0.6,0.6);
		Is = vec3<float>(0.8,0.8,0.8);
		pos = vec3<float>(5,5,4);
	};
};


