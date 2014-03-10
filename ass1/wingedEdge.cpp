#include "wingedEdge.h"
#include "vec3.h"

Edge::Edge(GLint v1, GLint v2, GLint f1, GLint f2, GLint l1, GLint l2, GLint r1, GLint r2, GLboolean crease)
{
	vert[0] = v1; vert[1] = v2;
	face[0] = f1; face[1] = f2;
	leftTr[0] = l1; leftTr[1] = l2;
	rightTr[0] = r1; rightTr[1] = r2;

	isCrease = crease;
}


Vertex::Vertex(GLfloat x,GLfloat y, GLfloat z)
{
	dim[0] = x;
	dim[1] = y;
	dim[2] = z;
	isPartofCrease = 0;
}

Face::Face(Vertex* n,GLint fe)
{
	norm[0] = n->dim[0];
	norm[1] = n->dim[1];
	norm[2] = n->dim[2];
	edge = fe;
}

wingedEdge::wingedEdge()
{
	ka = vec3<float>(0.3,0.3,0);
	kd = vec3<float>(0.3,0.6,0.2);
	ks = vec3<float>(0.5,0.5,0.5);
	alpha = 1;
	krg = vec3<float>(0.4,0.4,0.4);
	isCheckerboard = false;
}

wingedEdge::~wingedEdge(void)
{
	for(int i = 0; i<vertices.size();i++)
	{
		free(vertices[i]);
	}
	for(int i = 0; i<faces.size(); i++){
		free(faces[i]);
	}
	for(int i = 0; i<edges.size(); i++){
		free(edges[i]);
	}
}

void wingedEdge::getNeighbours(GLint vert, vector<GLint> &neigh, vector<GLint> &creases){
	//cout<<"edge size "<<edges.size()<<endl;

	GLint ed = vertices[vert]->edge;
	
	GLint prev_edge = ed;
	GLint next_edge = -1;
	GLint prev_face = -1;

	while(next_edge != ed){
		
		if( edges[prev_edge]->face[1] == prev_face){
			if(edges[prev_edge]->vert[0] == vert){
				next_edge = edges[prev_edge]->leftTr[0];
			}
			else{
				next_edge = edges[prev_edge]->leftTr[1];
			}
			prev_face = edges[prev_edge]->face[0];
		}
		else{
			if(edges[prev_edge]->vert[0] == vert){
				next_edge = edges[prev_edge]->rightTr[1];
			}
			else{
				next_edge = edges[prev_edge]->rightTr[0];
			}
			prev_face = edges[prev_edge]->face[1];
		}
		if(edges[next_edge]->isCrease == true)
			creases.push_back(next_edge);

		neigh.push_back(next_edge);
		prev_edge = next_edge;
	}

	/*
	cout<<"neigh "<<vert<<" : [";
	for(unsigned int i= 0 ; i < neigh.size(); i++){
		cout<<neigh[i]<<",";
	}
	cout<<"]\n";
	*/
}

GLint wingedEdge::getOppVertex(GLint f_num){
	GLint fe = faces[f_num]->edge;
	GLint next_ed;
	if(edges[fe]->face[0] == f_num){
		next_ed = edges[fe]->leftTr[1];
	}
	else{
		next_ed = edges[fe]->rightTr[1];
	}

	if(edges[fe]->vert[0] == edges[next_ed]->vert[0] || edges[fe]->vert[1] == edges[next_ed]->vert[0])
	{
		return edges[next_ed]->vert[1];
	}
	else
	{
		return edges[next_ed]->vert[0];
	}
}

GLint wingedEdge::getEdgeOppVertex(GLint e_num, GLint dir){
	GLint next_ed;
	if(dir == 0)
	{
		next_ed = edges[e_num]->leftTr[1];
	}
	else
	{
		next_ed = edges[e_num]->rightTr[1];
	}

	if(edges[e_num]->vert[0] == edges[next_ed]->vert[0] || edges[e_num]->vert[1] == edges[next_ed]->vert[0])
	{
		return edges[next_ed]->vert[1];
	}
	else
	{
		return edges[next_ed]->vert[0];
	}
}

void wingedEdge::render(void)
{
	for(int i = 0; i<faces.size(); i++)
	{
		GLint fe = faces[i]->edge;
		glBegin(GL_TRIANGLES);
		glNormal3f(faces[i]->norm[0],faces[i]->norm[1],faces[i]->norm[2]);
		glVertex3f(vertices[edges[fe]->vert[0]]->dim[0],vertices[edges[fe]->vert[0]]->dim[1],vertices[edges[fe]->vert[0]]->dim[2]);
		glVertex3f(vertices[edges[fe]->vert[1]]->dim[0],vertices[edges[fe]->vert[1]]->dim[1],vertices[edges[fe]->vert[1]]->dim[2]);
		
		GLint thirdVert = getOppVertex(i);
		
		glVertex3f(vertices[thirdVert]->dim[0],vertices[thirdVert]->dim[1],vertices[thirdVert]->dim[2]);
		glEnd();

	}
}

GLfloat inline wingedEdge::sabs(GLfloat x){
	return (x>0?x:-x);
}

void wingedEdge::unitize() {
	GLfloat maxx, minx, maxy, miny, maxz, minz;
	GLfloat cx, cy, cz, w, h, d;
	GLfloat scale;
	 
	/* get the max/mins */
	maxx = minx = vertices[0]->dim[0];
	maxy = miny = vertices[0]->dim[1];
	maxz = minz = vertices[0]->dim[2];

	for(int i = 1; i< vertices.size(); i++){
		if (maxx < vertices[i]->dim[0]) maxx = vertices[i]->dim[0];
		if (minx > vertices[i]->dim[0]) minx = vertices[i]->dim[0];
        
		if (maxy < vertices[i]->dim[1]) maxy = vertices[i]->dim[1];
		if (miny > vertices[i]->dim[1]) miny = vertices[i]->dim[1];

		if (maxz < vertices[i]->dim[2]) maxz = vertices[i]->dim[2];
		if (minz > vertices[i]->dim[2]) minz = vertices[i]->dim[2];
	}
    
	/* calculate model width, height, and depth */
	w = sabs(maxx) + sabs(minx);
	h = sabs(maxy) + sabs(miny);
	d = sabs(maxz) + sabs(minz);
    
	/* calculate center of the model */
	cx = (maxx + minx) / 2.0;
	cy = (maxy + miny) / 2.0;
	cz = (maxz + minz) / 2.0;

	/* calculate unitizing scale factor */
	scale = 2.0 / sqrt(w*w+h*h+d*d);
    
	/* translate around center then scale */
	
	for(int i = 0; i< vertices.size(); i++){
		vertices[i]->dim[0] -= cx;
		vertices[i]->dim[1] -= cy;
		vertices[i]->dim[2] -= cz;
		vertices[i]->dim[0] *= scale;
		vertices[i]->dim[1] *= scale;
		vertices[i]->dim[2] *= scale;
	}
	
}


vec3<float> wingedEdge::getColor(int f,vec3<float> p,const vec3<float> viewer, vector<Light> &ls , bool inShadow){

	vec3<float> color = vec3<float>(0);
	for(int i = 0; i< ls.size(); i++){
		if(inShadow){
			color = color + ls[i].Ia*ka;
			//cout<<"color :"<<color<<" , viewer: "<<viewer<<", point: "<<p<<endl;
		}
		else
		{
			vec3<float> lm = (ls[i].pos - p).normalize();
			vec3<float> norm = vec3<float>(faces[f]->norm[0],faces[f]->norm[1],faces[f]->norm[2]);
			float ldotn = max(lm.dot(norm),float(0));
			vec3<float> rm = ((norm*(2*ldotn)) - lm).normalize();
			vec3<float> v = (viewer-p).normalize();
			float rdotv = max(rm.dot(v),float(0));
			color = color + ls[i].Ia*ka + ls[i].Id*kd*ldotn + ls[i].Is*pow(rdotv,alpha);
			//if(color.y > 0)
				//cout<<"color :"<<color<<", rdotv: "<<rdotv<<" , ldotn: "<<ldotn<<" , viewer: "<<viewer<<", point: "<<p<<endl;
		}
	}
	return color;
}

float wingedEdge::intersect(const vec3<float> & origin, vec3<float> &dir, vec3<float> &point, int & fnum){
	//cout << origin << endl;
	float min_t = INFINITY;
	fnum = -1;
	//cout <<"intersect, "<<faces.size()<<","<<edges.size()<<","<<vertices.size()<<"\n";
	for(unsigned int i = 0; i<faces.size(); i++){
		//cout << faces[i]->edge<<endl;
		Edge* ed = edges[faces[i]->edge];
		vec3<float> v0,v1;
		
		if(edges[faces[i]->edge]->face[0] == i){
			v0 = vec3<float>(vertices[ed->vert[0]]->dim[0],vertices[ed->vert[0]]->dim[1],vertices[ed->vert[0]]->dim[2]);
			v1 = vec3<float>(vertices[ed->vert[1]]->dim[0],vertices[ed->vert[1]]->dim[1],vertices[ed->vert[1]]->dim[2]);
		}
		else{
			v1 = vec3<float>(vertices[ed->vert[0]]->dim[0],vertices[ed->vert[0]]->dim[1],vertices[ed->vert[0]]->dim[2]);
			v0 = vec3<float>(vertices[ed->vert[1]]->dim[0],vertices[ed->vert[1]]->dim[1],vertices[ed->vert[1]]->dim[2]);
		}
		
		GLint op_v = getOppVertex(i);
		vec3<float> v2(vertices[op_v]->dim[0],vertices[op_v]->dim[1],vertices[op_v]->dim[2]);
		
		vec3<float> norm(faces[i]->norm[0],faces[i]->norm[1],faces[i]->norm[2]);
		//vec3<float> norm = (v1-v0).cross(v2-v0);

		float ndotray = dir.dot(norm);
		if(ndotray > 0)
			continue;

		if(fabs(ndotray) < 0.001){
			continue;
		}

		float d = -1*norm.dot(v0);
		float t = -1*(norm.dot(origin) + d)/ndotray;
		
		//The triangle is behind the ray
		if(t<0.0001)
			continue;

		//intersection point
		vec3<float> p = origin + (dir*t);
		//if(dir.y == 0){
			//cout << "dir :"<<dir<<endl;
			//cout << "t :"<<t<<endl;
			//cout << "origin :"<<origin<<endl;
			//cout << "ndotray :"<<ndotray<<endl;
			//cout<<"\n--nodtray : "<<ndotray<<", norm.dot(origin): "<<norm.dot(origin)<<", face : "<<i<<", v0: "<<v0<<", "<<v1<<", "<<v2<<" , org : "<<origin<<", dir "<<dir<<", norm : "<<norm<<" ,dist: "<<d<<","<<t<<", point : "<<p<<endl;
		//}

		//inside outside test
		//edge0 check point on right side
		vec3<float> C;
		vec3<float> edge0 = v1 - v0;
		vec3<float> vp0 = p - v0;
		vec3<float> C1 = edge0.cross(vp0);
		if(norm.dot(C1) < -0.001){
			//if(dir.y == 0)
				//cout << "skipped1 : "<<C1<<endl;
			continue;
		}

		//edge1 check point on right side
		vec3<float> edge1 = v2 - v1;
		vec3<float> vp1 = p - v1;
		vec3<float> C2 = edge1.cross(vp1);
		if(norm.dot(C2) < -0.001){
			//if(dir.y == 0)
				//cout << "skipped2 : "<<C2<<endl;
			continue;
		}
		//edge2 check point on right side
		vec3<float> edge2 = v0 - v2;
		vec3<float> vp2 = p - v2;
		vec3<float> C3 = edge2.cross(vp2);
		if(norm.dot(C3) < -0.001){
			//if(dir.y == 0)
				//cout << "skipped3 : "<<C3<<endl;
			continue;
		}
		if(min_t > t){
			min_t = t;
			point = p;
			fnum = i;
		}
	}
	/*
	if(min_t == INFINITY){
		//if(dir.y ==0)
			//cout<<"@@@@"<<origin<<", dir "<<dir<<"\n\n";
	}
	else
		cout <<"---------------------"<<min_t<<", origin: "<<origin<<", point: "<<point<<", face : "<<fnum<<"\n";
	*/
	return min_t;
}