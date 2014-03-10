#pragma once
#include <iostream>
#include <vector>
#include <GL/glut.h>
#include "vec3.h"
#include "Light.h"

using namespace std;

class Edge{

public:
	GLint vert[2];
	GLint face[2];
	GLint leftTr[2];
	GLint rightTr[2];

	//Used to insert new vertices while subdividing
	//GLint newVert;

	Edge(GLint v1, GLint v2, GLint f1, GLint f2, GLint l1, GLint l2, GLint r1, GLint r2, GLboolean crease);
	Edge(GLint v1, GLint v2);

	GLboolean isCrease; // To check whether its a crease
};

class Vertex{
public:
	GLfloat dim[3];
	GLint edge;
	Vertex(GLfloat x,GLfloat y, GLfloat z);
	GLboolean isPartofCrease; // to check whether any of its neighbour is a crease
};

class Face{
	
public:
	GLfloat norm[3];
	GLint edge;
	Face(Vertex* n,GLint fe);
};

class wingedEdge{
public:
	GLfloat inline sabs(GLfloat x);
	vec3<float> ka,kd,ks,krg;
	int alpha;
	bool isCheckerboard;

	vector<Edge*> edges;
	vector<Vertex*> vertices;
	vector<Face*> faces;

	wingedEdge();
	~wingedEdge(void);

	GLint getOppVertex(GLint f_num);
	GLint getEdgeOppVertex(GLint e_num, GLint face);
	void render(void);
	void getNeighbours(GLint vert, vector<GLint> &neigh, vector<GLint> &creases);
	void unitize();
	float intersect(const vec3<float> & origin, vec3<float> &dir, vec3<float> &point, int &fnum);
	vec3<float> getColor(int f, vec3<float> p,const vec3<float> viewer, vector<Light> &ls , bool inShadow);
};
