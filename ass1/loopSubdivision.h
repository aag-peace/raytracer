#pragma once
#include "wingedEdge.h"

class Model{
public:
	class Face{
	public:
		GLint v[3];
		Face(GLint v1, GLint v2, GLint v3);
	};

	class Vertex{
	public:
		GLfloat dim[3];
		GLint crease[2];
		Vertex(GLfloat x, GLfloat y, GLfloat z);
	};

	class Crease{
	public:
		GLint v[2];
		Crease(GLint v1,GLint v2 );
	};

	vector<Model::Face*> faces;
	vector<Model::Crease*> creases;
	vector<Model::Vertex*> vertices;

	~Model(void);
};

class loopSubdivision
{
	Vertex* getNormal(wingedEdge* we, GLint p1, GLint p2, GLint p3);
	//Iterate over all planes and introduce new edge and vertices in new DS
	void subdivideEdges(wingedEdge* wein, wingedEdge* weout);
	void updateEdges(wingedEdge* wein, wingedEdge* weout);
	//Calculate new positions of old vertices by using new vertices pos.
	void updateOldVertices(wingedEdge* wein, wingedEdge* weout);
public:
	GLint max_neigh;
	vector<GLfloat> beta;

	loopSubdivision(void);
	~loopSubdivision(void);

	//Used to apply loop subdivision and output a new winged edge data structure
	wingedEdge* subdivide(wingedEdge* we);

	//Used to convert model to winged edge data structure
	wingedEdge* modToWe(Model * mod);

};

