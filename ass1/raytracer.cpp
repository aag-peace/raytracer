#include "raytracer.h"
#include "vec3.h"
#include <algorithm>
#include <fstream>

using namespace std;

raytracer::raytracer()
{
	width = 100;
	height = 100;
	lights.push_back(*(new Light()));
}


raytracer::~raytracer()
{
}

void raytracer::set_img_size(unsigned int x, unsigned int y)
{
	width = x;
	height = y;
}

void raytracer::render(vector<wingedEdge*> &objects)
{
	/*
	for(int obj1 = 0 ; obj1 < objects.size(); obj1++){
		for(int i = 0; i<objects[obj1]->vertices.size();i++)
		{
			cout <<"v "<< i << " : " <<(objects[obj1]->vertices[i]->dim[0])<< " : " <<(objects[obj1]->vertices[i]->dim[1])<< " : " <<(objects[obj1]->vertices[i]->dim[2])<< " : " <<(objects[obj1]->vertices[i]->edge)<< endl;
		}
		for(int i = 0; i<objects[obj1]->faces.size(); i++){
			cout <<"f "<< i << " : " <<(objects[obj1]->faces[i]->norm[0])<< " : " <<(objects[obj1]->faces[i]->norm[1])<< " : " <<(objects[obj1]->faces[i]->norm[2])<< " : " <<(objects[obj1]->faces[i]->edge)<< endl;
		}
		for(int i = 0; i<objects[obj1]->edges.size(); i++){
			cout <<"e "<< i << " : " <<(objects[obj1]->edges[i]->vert[0])<< " : " <<(objects[obj1]->edges[i]->vert[1])<< " : " <<(objects[obj1]->edges[i]->face[0])<< " : " <<(objects[obj1]->edges[i]->face[1]) << " : " <<(objects[obj1]->edges[i]->leftTr[0])<< " : " <<(objects[obj1]->edges[i]->leftTr[1])<< " : " <<(objects[obj1]->edges[i]->rightTr[0])<< " : " <<(objects[obj1]->edges[i]->rightTr[1])<< " : " <<(objects[obj1]->edges[i]->isCrease)<< endl;
		}
	}
	*/

	vector<vec3<float>> image(width*height);
	vec3<float> eye_pos(0,0,2);
	float screen_pos = 1;
	float global_hw = 1;//half width of opengl window space
	float global_hh = 1;//half height of opengl window space

	float dx = global_hw/width;
	float dy = global_hh/height;

	//cout <<"\n eye pos "<<eye_pos;
	//cout <<"height "<<height<<", width "<<width<<endl;
	//parallelize this code
	for(int i = -1*height; i<height; i+=2){
		for(int j = -1*width; j<width; j+=2){	
			vec3<float> dir = vec3<float>(j*dx,i*dy,screen_pos) - eye_pos;
			dir.normalize();			
			//cout<<"dir : " <<dir<<endl;
			//cout << i<<","<<j<<","<<((-1*i+height-1)/2.0)<<","<<((j+width)/2.0)<<endl;
			image[(((-1*i)+ height - 2)*width/2.0) + ((j+width)/2.0)] = trace(eye_pos,dir,objects,0);
		}
	}


	ofstream ofs("./ray_traced.ppm", ios::out | ios::binary);
	ofs << "P6\n" << width << " " << height << "\n255\n";
	for(unsigned int i =0 ; i< image.size(); i++){
		//if(image[i].x >0)
			//cout << "out :- " << " i : "<< i/width <<","<< image[i] << ", " << min(float(1),image[i].x)*255<<endl;
		ofs<<(unsigned char)(min(float(1),image[i].x)*255) <<
			(unsigned char)(min(float(1),image[i].y)*255) <<
			(unsigned char)(min(float(1),image[i].z)*255);
	}
	ofs.close();
}

vec3<float> raytracer::trace(const vec3<float> &o, vec3<float> &dir, vector<wingedEdge*> &objects, const int depth){
	if(depth > 4)
		return vec3<float>(0);

	float near = INFINITY;
	vec3<float> near_point;
	float new_near;
	vec3<float> point;
	int fnum = -1;
	int objnum = -1;
	//shoot primary ray and 
	for(unsigned int i = 0; i<objects.size(); i++ ){
		//cout << i+100000 <<endl;
		int f = -1;
		if((new_near = objects[i]->intersect(o,dir,point,f)) < near){
			near = new_near;
			near_point = point;
			objnum = i;
			fnum = f;
		}
	}

	
	vec3<float> p_temp;
	int f_temp;
	bool isInShadow = false;
	vec3<float> color(0);
	if(near != INFINITY){
		//Check for shadow ray, whether it is in shadow or not
		for(int lnum = 0; lnum < lights.size(); lnum++){
			vec3<float> shadowray = lights[0].pos - near_point;
			shadowray.normalize();
			for(unsigned int i = 0; i<objects.size(); i++ ){
				if(objects[i]->intersect(near_point,shadowray,p_temp,f_temp) != INFINITY){
					isInShadow = true;
					//cout <<"shadow\n";
					break;
				}
			}

			if(isInShadow)
				break;
		}
		if(objects[objnum]->isCheckerboard == true)
			board->setColor(near_point);

		color = objects[objnum]->getColor(fnum,near_point,o,lights,isInShadow);
		// shoot reflection ray
		vec3<float> norm(objects[objnum]->faces[fnum]->norm[0],objects[objnum]->faces[fnum]->norm[1],objects[objnum]->faces[fnum]->norm[2]);
		vec3<float> refldir = dir - norm * 2 * dir.dot(norm);

		refldir.normalize();
		//if(objnum == 0)
			//cout << "color before reflection : "<<color<<", depth: "<<depth<<", o:"<<o<<", near_point"<<near_point<<", dir: "<<dir<<", refl: "<<refldir<<endl;
		color  = color + trace(near_point,refldir,objects,depth+1)*objects[objnum]->krg;
		//if(objnum == 0)
			//cout << "color after reflection : "<<color<<", depth: "<<depth<<endl;

		//return vec3<float>(1);
	}
	else{
		color = vec3<float>(0);
	}

	return color;
}