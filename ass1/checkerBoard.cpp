#include "checkerBoard.h"


checkerBoard::checkerBoard(wingedEdge* we)
{
	w = we;
	float minx = INFINITY,miny = INFINITY,maxx = -1*INFINITY,maxy = -1*INFINITY,z;
	for(int i = 0; i<we->vertices.size(); i++){
		if(minx > we->vertices[i]->dim[0])
			minx = we->vertices[i]->dim[0];

		if(miny > we->vertices[i]->dim[2])
			miny = we->vertices[i]->dim[2];

		if(maxx < we->vertices[i]->dim[0])
			maxx = we->vertices[i]->dim[0];

		if(maxy < we->vertices[i]->dim[2])
			maxy = we->vertices[i]->dim[2];

		
	}
	z = we->vertices[0]->dim[1];
	num_square = 20;
	box_length = (maxx-minx)/num_square;
	cout <<"board: "<<minx<<" ,"<<miny<<" ,"<<maxx<<" ,"<<maxy<<"\n";
	cout <<"box length : "<<box_length<<endl;
}


checkerBoard::~checkerBoard(void)
{
}

void checkerBoard::setColor(vec3<float> p){
	if(int((p.x - minx)/box_length)%2 == 0){
		if(int((p.z - miny)/box_length)%2 == 0){
			w->ka = vec3<float>(0.2);
			w->kd = vec3<float>(0.2);
			w->ks = vec3<float>(0.4);
		}
		else{
			w->ka = vec3<float>(0.8);
			w->kd = vec3<float>(0.8);
			w->ks = vec3<float>(0.4);
		}
	}
	else{
		if(int((p.z - miny)/box_length)%2 == 0){
			w->ka = vec3<float>(0.8);
			w->kd = vec3<float>(0.8);
			w->ks = vec3<float>(0.4);
		}
		else{
			w->ka = vec3<float>(0.2);
			w->kd = vec3<float>(0.2);
			w->ks = vec3<float>(0.4);
		}
	}
}
