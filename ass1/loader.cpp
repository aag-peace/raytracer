#include "loader.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <map>

using namespace std;

loader::loader(void)
{

}


loader::~loader(void)
{
}

Vertex* loader::cross(Vertex* u, Vertex* v){
	GLfloat x = u->dim[1]*v->dim[2] - u->dim[2]*v->dim[1];
	GLfloat y = u->dim[2]*v->dim[0] - u->dim[0]*v->dim[2];
	GLfloat z = u->dim[0]*v->dim[1] - u->dim[1]*v->dim[0];
	GLfloat mag = sqrt(x*x + y*y + z*z);
	return new Vertex(x/mag,y/mag,z/mag);
}

void loader::readOBJ(wingedEdge* we, char filename[])
{
	string line;
	ifstream objfile(filename);
	GLfloat x,y,z;
	GLint p[3];
	GLint pa, pb;
	map<pair<GLint,GLint>,GLint>::iterator val;

	if(objfile.is_open())
	{
		map<pair<GLint,GLint>,GLint> edge_pairs;

		while(getline(objfile,line)){
			istringstream ss (line);
			char type = 0;
			ss>>type;
			Vertex* norm;
			Vertex *v,*u;


			switch(type){
			case 'v':
				ss>>x>>y>>z;
				we->vertices.push_back(new Vertex(x,y,z));
				break;

			case 'f':
				ss>>p[0]>>p[1]>>p[2];

				GLint face_edge[6];

				for(int k = 0; k < 3; k++){
					pa = p[k]-1;
					pb = p[(k+1)%3]-1;

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
				u = new Vertex(we->vertices[p[1]-1]->dim[0] - we->vertices[p[0]-1]->dim[0], we->vertices[p[1]-1]->dim[1] - we->vertices[p[0]-1]->dim[1], we->vertices[p[1]-1]->dim[2] - we->vertices[p[0]-1]->dim[2]);

				v = new Vertex(we->vertices[p[2]-1]->dim[0] - we->vertices[p[1]-1]->dim[0], we->vertices[p[2]-1]->dim[1] - we->vertices[p[1]-1]->dim[1], we->vertices[p[2]-1]->dim[2] - we->vertices[p[1]-1]->dim[2]);
				norm = cross(u,v);
				
				free(u);free(v);

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

				break;

			case 'c':
				ss>>p[0]>>p[1];
				pa = p[0]-1;
				pb = p[1]-1;
				cout << "mod crease "<<pa<<","<<pb<<endl;

				val = edge_pairs.find(make_pair(pa,pb));
				if(val == edge_pairs.end()){
					val = edge_pairs.find(make_pair(pb,pa));
				}
				we->edges[val->second]->isCrease = true;
				break;
				
			default:
				;
			}
		}

		objfile.close();

		/*we->edges[0]->isCrease = true;
		we->edges[1]->isCrease = true;
		we->edges[2]->isCrease = true;
		cout <<"e "<< 0 << " : " <<(we->edges[0]->vert[0])<< " : " <<(we->edges[0]->vert[1])<< " : " <<(we->edges[0]->face[0])<< " : " <<(we->edges[0]->face[1]) << " : " <<(we->edges[0]->leftTr[0])<< " : " <<(we->edges[0]->leftTr[1])<< " : " <<(we->edges[0]->rightTr[0])<< " : " <<(we->edges[0]->rightTr[1])<< " : " <<(we->edges[0]->isCrease)<< endl;
		cout <<"e "<< 1 << " : " <<(we->edges[1]->vert[0])<< " : " <<(we->edges[1]->vert[1])<< " : " <<(we->edges[1]->face[0])<< " : " <<(we->edges[1]->face[1]) << " : " <<(we->edges[1]->leftTr[0])<< " : " <<(we->edges[1]->leftTr[1])<< " : " <<(we->edges[1]->rightTr[0])<< " : " <<(we->edges[1]->rightTr[1])<< " : " <<(we->edges[1]->isCrease)<< endl;
		cout <<"e "<< 2 << " : " <<(we->edges[2]->vert[0])<< " : " <<(we->edges[2]->vert[1])<< " : " <<(we->edges[2]->face[0])<< " : " <<(we->edges[2]->face[1]) << " : " <<(we->edges[2]->leftTr[0])<< " : " <<(we->edges[2]->leftTr[1])<< " : " <<(we->edges[2]->rightTr[0])<< " : " <<(we->edges[2]->rightTr[1])<< " : " <<(we->edges[2]->isCrease)<< endl;*/

		cout << we->vertices.size()<<endl;
		cout << we->edges.size()<<endl;
		cout << we->faces.size()<<endl;
		cout<<endl;
		/*
		for(int i = 0; i<we->vertices.size();i++)
		{
			cout <<"v "<< i << " : " <<(we->vertices[i]->dim[0])<< " : " <<(we->vertices[i]->dim[1])<< " : " <<(we->vertices[i]->dim[2])<< " : " <<(we->vertices[i]->edge)<< endl;
		}
		for(int i = 0; i<we->faces.size(); i++){
			cout <<"f "<< i << " : " <<(we->faces[i]->norm[0])<< " : " <<(we->faces[i]->norm[1])<< " : " <<(we->faces[i]->norm[2])<< " : " <<(we->faces[i]->edge)<< endl;
		}
		for(int i = 0; i<we->edges.size(); i++){
			cout <<"e "<< i << " : " <<(we->edges[i]->vert[0])<< " : " <<(we->edges[i]->vert[1])<< " : " <<(we->edges[i]->face[0])<< " : " <<(we->edges[i]->face[1]) << " : " <<(we->edges[i]->leftTr[0])<< " : " <<(we->edges[i]->leftTr[1])<< " : " <<(we->edges[i]->rightTr[0])<< " : " <<(we->edges[i]->rightTr[1])<< " : " <<(we->edges[i]->isCrease)<< endl;
		}
		*/
		
	}
}

void loader::writeOBJ(wingedEdge* obj)
{

}