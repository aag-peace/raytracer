#include "loopSubdivision.h"
#include <map>

using namespace std;
Model::Vertex::Vertex(GLfloat x, GLfloat y, GLfloat z){
	dim[0] = x;
	dim[1] = y;
	dim[2] = z;
}

Model::Face::Face(GLint v1, GLint v2, GLint v3){
	v[0] = v1;
	v[1] = v2;
	v[2] = v3;
}

Model::Crease::Crease(GLint v1, GLint v2){
	v[0] = v1;
	v[1] = v2;
}

Model::~Model(void){
	for(unsigned int i = 0; i<vertices.size(); i++){
		free(vertices[i]);
	}

	for(unsigned int i = 0; i<faces.size(); i++){
		free(faces[i]);
	}

	for(unsigned int i = 0; i<creases.size(); i++){
		free(creases[i]);
	}
}

loopSubdivision::loopSubdivision(void)
{
	max_neigh = 10;
	beta.push_back(0);
	for(int i = 1 ; i<max_neigh; i++)
	{
		beta.push_back((0.625 - pow((0.375 + 0.25*cos(6.28/i)),2))/i);
		//cout<<beta.back()<<endl;
	}
	
}


loopSubdivision::~loopSubdivision(void)
{
}


Vertex* loopSubdivision::getNormal(wingedEdge* we, GLint p0, GLint p1, GLint p2){
	Vertex* u = new Vertex(we->vertices[p1]->dim[0] - we->vertices[p0]->dim[0], we->vertices[p1]->dim[1] - we->vertices[p0]->dim[1], we->vertices[p1]->dim[2] - we->vertices[p0]->dim[2]);
	Vertex* v = new Vertex(we->vertices[p2]->dim[0] - we->vertices[p1]->dim[0], we->vertices[p2]->dim[1] - we->vertices[p1]->dim[1], we->vertices[p2]->dim[2] - we->vertices[p1]->dim[2]);
	
	GLfloat x = u->dim[1]*v->dim[2] - u->dim[2]*v->dim[1];
	GLfloat y = u->dim[2]*v->dim[0] - u->dim[0]*v->dim[2];
	GLfloat z = u->dim[0]*v->dim[1] - u->dim[1]*v->dim[0];
	GLfloat mag = sqrt(x*x + y*y + z*z);

	free(u); free(v);

	return new Vertex(x/mag,y/mag,z/mag);
}


wingedEdge* loopSubdivision::subdivide(wingedEdge* wein){
	Model* mod = new Model();

	GLint vert_offset = wein->edges.size();

	//Create vertex on each edge
	for(unsigned int i = 0; i<wein->edges.size(); i++){
		
		GLint v1 = wein->edges[i]->vert[0];
		GLint v2 = wein->edges[i]->vert[1];
		GLfloat x,y,z;
		if(wein->edges[i]->isCrease == false){
			GLint v3 = wein->getEdgeOppVertex(i,0);
			GLint v4 = wein->getEdgeOppVertex(i,1);

			x = (3*wein->vertices[v1]->dim[0] + 3*wein->vertices[v2]->dim[0] + wein->vertices[v3]->dim[0] + wein->vertices[v4]->dim[0])/8;
			y = (3*wein->vertices[v1]->dim[1] + 3*wein->vertices[v2]->dim[1] + wein->vertices[v3]->dim[1] + wein->vertices[v4]->dim[1])/8;
			z = (3*wein->vertices[v1]->dim[2] + 3*wein->vertices[v2]->dim[2] + wein->vertices[v3]->dim[2] + wein->vertices[v4]->dim[2])/8;
		}
		else
		{
			x = (wein->vertices[v1]->dim[0] + wein->vertices[v2]->dim[0])/2;
			y = (wein->vertices[v1]->dim[1] + wein->vertices[v2]->dim[1])/2;
			z = (wein->vertices[v1]->dim[2] + wein->vertices[v2]->dim[2])/2;
			//cout <<"edge vertex "<<x<<","<<y<<","<<z<<endl;
			
			mod->creases.push_back(new Model::Crease(v1+vert_offset,mod->vertices.size()));
			mod->creases.push_back(new Model::Crease(mod->vertices.size(),v2+vert_offset));
		}
		Model::Vertex* new_v = new Model::Vertex(x,y,z);
		mod->vertices.push_back(new_v);
	}

	GLint offset = mod->vertices.size();

	//Create faces with surrounding vertices
	for(unsigned int i = 0; i<wein->faces.size(); i++){
		GLint fe = wein->faces[i]->edge;
		GLint v[3],vout[3];
		v[1] = fe;
		if(wein->edges[fe]->face[0] == i){
			v[0] = wein->edges[fe]->leftTr[0];
			v[2] = wein->edges[fe]->leftTr[1];

			vout[0] = wein->edges[fe]->vert[0] + offset ;
			vout[1] = wein->edges[fe]->vert[1] + offset;
		}
		else{
			v[0] = wein->edges[fe]->rightTr[0];
			v[2] = wein->edges[fe]->rightTr[1];
			
			vout[1] = wein->edges[fe]->vert[0] + offset;
			vout[0] = wein->edges[fe]->vert[1] + offset;
		}

		if((wein->edges[v[2]]->vert[1]+offset) == vout[0] || (wein->edges[v[2]]->vert[1]+offset) == vout[1]){
			vout[2] = wein->edges[v[2]]->vert[0] + offset;
		}
		else{
			vout[2] = wein->edges[v[2]]->vert[1] + offset;
		}

		mod->faces.push_back(new Model::Face(v[0],v[1],v[2]));
		mod->faces.push_back(new Model::Face(v[0],vout[0],v[1]));
		mod->faces.push_back(new Model::Face(v[1],vout[1],v[2]));
		mod->faces.push_back(new Model::Face(v[2],vout[2],v[0]));
	}

	//Insert all old vertices
	for(unsigned int i =0; i<wein->vertices.size(); i++){
		vector<GLint> neigh;
		vector<GLint> creases;
		wein->getNeighbours(i,neigh,creases);
		
		GLfloat x = 0, y = 0, z = 0;
		GLint vert_temp = -1;

		if(creases.size() == 2)
		{
			for(unsigned int j = 0; j<creases.size(); j++){
				if(wein->edges[creases[j]]->vert[0] == i){
					vert_temp = wein->edges[creases[j]]->vert[1];
				}
				else{
					vert_temp = wein->edges[creases[j]]->vert[0];
				}
				x += wein->vertices[vert_temp]->dim[0];
				y += wein->vertices[vert_temp]->dim[1];
				z += wein->vertices[vert_temp]->dim[2];
				//cout <<"mod"<<j<<" : "<< wein->vertices[vert_temp]->dim[0]<<","<<wein->vertices[vert_temp]->dim[1]<<","<< wein->vertices[vert_temp]->dim[2]<<endl;
			}
			
			//cout << x << "," << y << "," << z << "\n";
			
			x = (x*0.125) + (0.75 * (wein->vertices[i]->dim[0]));
			y = (y*0.125) + (0.75 * (wein->vertices[i]->dim[1]));
			z = (z*0.125) + (0.75 * (wein->vertices[i]->dim[2]));
		}

		else if(creases.size() < 2)
		{
			GLfloat nbeta = beta[neigh.size()];
			//cout<<"nbeta "<<nbeta<<endl;
			for(unsigned int j = 0; j<neigh.size(); j++){
				if(wein->edges[neigh[j]]->vert[0] == i){
					vert_temp = wein->edges[neigh[j]]->vert[1];
				}
				else{
					vert_temp = wein->edges[neigh[j]]->vert[0];
				}
				x += wein->vertices[vert_temp]->dim[0];
				y += wein->vertices[vert_temp]->dim[1];
				z += wein->vertices[vert_temp]->dim[2];

				//cout <<"mod"<<j<<" : "<< wein->vertices[vert_temp]->dim[0]<<","<<wein->vertices[vert_temp]->dim[1]<<","<< wein->vertices[vert_temp]->dim[2]<<endl;
			}
			x = (x*nbeta) + ((1-(neigh.size() * nbeta)) * (wein->vertices[i]->dim[0]));
			y = (y*nbeta) + ((1-(neigh.size() * nbeta)) * (wein->vertices[i]->dim[1]));
			z = (z*nbeta) + ((1-(neigh.size() * nbeta)) * (wein->vertices[i]->dim[2]));
		}
		else{
			x = wein->vertices[i]->dim[0];
			y = wein->vertices[i]->dim[1];
			z = wein->vertices[i]->dim[2];

			cout<<"no update\n";
		}
		mod->vertices.push_back(new Model::Vertex(x,y,z));
	}

	/*
	for(int i =0; i<mod->vertices.size(); i++){
		cout<<"mod v "<<i<<" ["<<mod->vertices[i]->dim[0]<<","<<mod->vertices[i]->dim[1]<<","<<mod->vertices[i]->dim[2]<<","<<"]\n";
	}
	for(int i =0; i<mod->faces.size(); i++){
		cout<<"mod f "<<i<<" ["<<mod->faces[i]->v[0]<<","<<mod->faces[i]->v[1]<<","<<mod->faces[i]->v[2]<<","<<"]\n";
	}
	*/
	delete (wein);
	return modToWe(mod);
}


wingedEdge* loopSubdivision::modToWe(Model * mod){
	wingedEdge* we = new wingedEdge();
	map<pair<GLint,GLint>,GLint> edge_pairs;

	for(unsigned int i = 0; i< mod->vertices.size(); i++){
		we->vertices.push_back(new Vertex(mod->vertices[i]->dim[0],mod->vertices[i]->dim[1],mod->vertices[i]->dim[2]));
	}

	for(unsigned int i = 0; i<mod->faces.size(); i++){
		GLint p[3];
		p[0] = mod->faces[i]->v[0];
		p[1] = mod->faces[i]->v[1];
		p[2] = mod->faces[i]->v[2];

		GLint face_edge[6];

		for(int k = 0; k < 3; k++){
			GLint pa = p[k];
			GLint pb = p[(k+1)%3];

			map<pair<GLint,GLint>,GLint>::iterator val = edge_pairs.find(make_pair(pb,pa));

			if(val != edge_pairs.end()) {
				GLint edge_num = val->second;	
				face_edge[2*k] = edge_num;
				face_edge[2*k+1] = 1;
				we->edges[edge_num]->face[1] = we->faces.size();
			}
			else{
				face_edge[2*k] = we->edges.size();
				face_edge[2*k+1] = 0;
				edge_pairs.insert(pair<pair<GLint,GLint>,GLint>(make_pair(pa,pb),we->edges.size()));
				we->edges.push_back(new Edge(pa,pb,we->faces.size(),-1,-1,-1,-1,-1,GL_FALSE));	
				we->vertices[pa]->edge = face_edge[2*k];
			}
		}

		Vertex * norm = getNormal(we,p[0],p[1],p[2]);
		//cout << "norm "<<norm->dim[0]<<","<<norm->dim[1]<<","<<norm->dim[2]<<endl;

		we->faces.push_back(new Face(norm,face_edge[0]));				
		//cout << "face edge : "<<face_edge[0]<<","<<face_edge[1]<<","<<face_edge[2]<<","<<face_edge[3]<<","<<face_edge[4]<<","<<face_edge[5]<<"\n";

		for(int k = 0; k<3; k++){
			//cout <<"set "<<k<<","<<((2*(k-1)+6)%6)<<","<<((2*(k+1))%6)<<endl;

			if(face_edge[2*k+1] == 1)
			{
				we->edges[face_edge[2*k]]->rightTr[0] = face_edge[(2*(k+2))%6];
				we->edges[face_edge[2*k]]->rightTr[1] = face_edge[(2*(k+1))%6];
			}
			else
			{
				we->edges[face_edge[2*k]]->leftTr[0] = face_edge[(2*(k+2))%6];
				we->edges[face_edge[2*k]]->leftTr[1] = face_edge[(2*(k+1))%6];
			}
		}
	}

	for(unsigned int i = 0; i<mod->creases.size(); i++){
		
		GLint pa = mod->creases[i]->v[0];
		GLint pb = mod->creases[i]->v[1];
		
		//cout << "mod crease "<<pa<<","<<pb<<endl;

		map<pair<GLint,GLint>,GLint>::iterator val = edge_pairs.find(make_pair(pa,pb));
		if(val == edge_pairs.end()){
			val = edge_pairs.find(make_pair(pb,pa));
		}

		we->edges[val->second]->isCrease = true;
	}

	return we;
}