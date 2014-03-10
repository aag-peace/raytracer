#include "wingedEdge.h"

class loader
{
public:
	loader(void);
	~loader(void);
	
	void readOBJ(wingedEdge* we,char filename[]);
	void writeOBJ(wingedEdge* obj);
	Vertex* cross(Vertex* u, Vertex* v);
};
