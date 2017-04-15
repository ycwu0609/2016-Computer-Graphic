#include "mesh.h"
#include "glut.h"
#include <math.h>
#include <fstream>
#include <iostream>
using namespace std;

int cnt = 0,w,h;

float win;
mesh *object1[10];

GLfloat scale_value[10][3] = { 0 };
GLfloat rotation[10][4] = { 0 };
GLfloat transfer[10][3] = { 0 };

int windowSize[2];

//mouse control
GLfloat old_x = 0 , crtl_x[10] = { 0 }, rec_x =  0 ;
GLfloat old_y =  0 , crtl_y[10] = { 0 }, rec_y =  0 ;
int select=-1;


//keyboard control
GLfloat z=0.0f;
GLfloat old_z=0.0f;
GLfloat radius;

int botton;
int angle = 0;
int rot = 0;
int dis_x, dis_z;
void zoom(GLfloat);



void view_load();
void light_load();
void obj_load();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void MotionMouse(int, int);


int main(int argc, char** argv)
{
	obj_load();

	glutInit(&argc, argv);
	glutInitWindowSize(500, 500);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH);
	glutCreateWindow("Test Scene1");
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(MotionMouse);
	glutMainLoop();

	return 0;
}

void view_load(){


	GLfloat eye[3];
	GLfloat vat[3];
	GLfloat vup[3];
	GLfloat fovy, dnear, dfar;
	GLfloat viewport[4];


	fstream viewing;
	viewing.open("view.view", ios::in);
	char c[100];
	while (viewing >> c){
		cout << c << endl;
		if (!strcmp(c, "eye"))
			for (size_t i = 0; i < 3; i++) viewing >> eye[i];
		if (!strcmp(c, "vat"))
			for (size_t i = 0; i < 3; i++) viewing >> vat[i];
		if (!strcmp(c, "vup"))
			for (size_t i = 0; i < 3; i++) viewing >> vup[i];
		if (!strcmp(c, "fovy")) viewing >> fovy;
		if (!strcmp(c, "dnear")) viewing >> dnear;
		if (!strcmp(c, "dfar")) viewing >> dfar;
		if (!strcmp(c, "viewpoint"))
			for (size_t i = 0; i < 4; i++) viewing >> viewport[i];
	}
	for (size_t i = 0; i < 3; i++) cout <<"read: "<< eye[i] <<" ";
	cout << endl;
	radius = sqrtf(pow(eye[0] - vat[0], 2) + pow(eye[1] - vat[1], 2) + pow(eye[2] - vat[2], 2));
	
	angle = atan((double)(eye[0] - vat[0]) /(eye[2] - vat[2]))*180/3.1415926;
	cout << "view_x: " << eye[0] << " center_x: " << vat[0] << endl;
	cout << "view_z: " << eye[2] << " center_z: " << vat[2] << endl;
	cout << "Angle: " << angle << endl;
	//zoom
	
	if (z != 0){
		radius += z;
		eye[0] = vat[0] + radius*(GLfloat)sin(angle*(3.1415926) / 180);
		eye[2] = vat[2] + radius*(GLfloat)cos(angle*(3.1415926) / 180);
	}
	if (rot != 0){
		angle += rot;
		angle %= 360;
		eye[0] = vat[0] + radius*(GLfloat)sin(angle*(3.1415926) / 180);
		eye[2] = vat[2] + radius*(GLfloat)cos(angle*(3.1415926) / 180);
	}
	
	w = viewport[2];
	h = viewport[3];
	win = w * h;
	
	//---------------viewing-------------------
	// viewport transformation
	glViewport(viewport[0], viewport[1], viewport[2], viewport[3]);

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, viewport[2] / viewport[3], dnear, dfar);
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	cout << "camera: " << eye[0] << " " << eye[1] << " " << eye[2] << endl;
	gluLookAt(eye[0], eye[1], eye[2],// eye
	vat[0], vat[1], vat[2],     // center
	vup[0], vup[1], vup[2]);    // up
	//------------------------------------------
}

void light_load()
{
	fstream light;
	light.open("light.light", ios::in);
	char c[100];
	int j = 0;


	GLfloat light_specular[8][4] = { 0 };
	GLfloat light_diffuse[8][4] = { 0 };
	GLfloat light_ambient[8][4] = { 0 };
	GLfloat light_position[8][4] = { 0 };
	
	GLfloat ambient[4] = { 0 };


	while (light >> c){
		if (!strcmp(c, "light")) {
			//cout << j << endl;
			//position
			for (size_t i = 0; i < 3; i++)	light >> (GLfloat)light_position[j][i];
			light_position[j][3] = (GLfloat)1.0;
			//ambient
			for (size_t i = 0; i < 3; i++)	light >> (GLfloat)light_ambient[j][i];
			light_ambient[j][3] = (GLfloat)0.0;
			//diffuse
			for (size_t i = 0; i < 3; i++)	light >> (GLfloat)light_diffuse[j][i];
			light_diffuse[j][3] = (GLfloat)0.0;
			//specular
			for (size_t i = 0; i < 3; i++)	light >> (GLfloat)light_specular[j][i];
			light_specular[j][3] = (GLfloat)0.0;

			j++;
			//light >> c;
		}
		

		else if (!strcmp(c, "ambient")){
			for (size_t i = 0; i < 3; i++) light >> (GLfloat)ambient[i];
			ambient[3] = (GLfloat)0.0;
			//light >> c;
		}
	}

	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	
	for (int i = 0; i < j; i++){
		glLightfv(GL_LIGHT0+i, GL_POSITION, light_position[i]);
		glLightfv(GL_LIGHT0+i, GL_DIFFUSE, light_diffuse[i]);
		glLightfv(GL_LIGHT0+i, GL_SPECULAR, light_specular[i]);
		glLightfv(GL_LIGHT0+i, GL_AMBIENT, light_ambient[i]);

		glEnable(GL_LIGHT0+i);
	}
	
	glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient);
}


void obj_load(){

	fstream obj;
	obj.open("scene.scene", ios::in);
	char c[100];
	
	while (obj >> c){
		if (!strcmp(c, "model")){
			char name[20];

			obj >> name;
			cout << name;
			object1[cnt] = new mesh(name);
			for (size_t i = 0; i < 3; i++) {
				obj >> (GLfloat)scale_value[cnt][i];
				cout << scale_value[cnt][i] << " ";
			}
			for (size_t i = 0; i < 4; i++) {
				obj >> (GLfloat)rotation[cnt][i];
				cout << rotation[cnt][i] << " ";
			}
			for (size_t i = 0; i < 3; i++) {
				obj >> (GLfloat)transfer[cnt][i];
				cout << transfer[cnt][i] << " ";
			}
			cnt++;
			cout << endl;
		}
			
	}

}

void draw_obj(){

		for (size_t j = 0; j < cnt; j++){
				glPushMatrix();
				glTranslatef(transfer[j][0] + crtl_x[j], transfer[j][1] - crtl_y[j], transfer[j][2]);
				glRotatef(rotation[j][0], rotation[j][1], rotation[j][2], rotation[j][3]);
				glScalef(scale_value[j][0], scale_value[j][1], scale_value[j][2]);
			
			int lastMaterial = -1;
			for (size_t i = 0; i < object1[j]->fTotal; ++i)
			{
				// set material property if this face used different material
				if (lastMaterial != object1[j]->faceList[i].m)
				{
					lastMaterial = (int)object1[j]->faceList[i].m;
					glMaterialfv(GL_FRONT, GL_AMBIENT, object1[j]->mList[lastMaterial].Ka);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, object1[j]->mList[lastMaterial].Kd);
					glMaterialfv(GL_FRONT, GL_SPECULAR, object1[j]->mList[lastMaterial].Ks);
					glMaterialfv(GL_FRONT, GL_SHININESS, &object1[j]->mList[lastMaterial].Ns);

					//you can obtain the texture name by object->mList[lastMaterial].map_Kd
					//load them once in the main function before mainloop
					//bind them in display function here
				}
				glBegin(GL_TRIANGLES);
				for (size_t k = 0; k < 3; ++k)
				{
					//textex corrd. object->tList[object->faceList[i][j].t].ptr
					glNormal3fv(object1[j]->nList[object1[j]->faceList[i][k].n].ptr);
					glVertex3fv(object1[j]->vList[object1[j]->faceList[i][k].v].ptr);
				}
				glEnd();
			}
			glPopMatrix();
		}
	}


void display()
{
	// clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);      //清除用color
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer


	view_load();
	
	//注意light位置的設定，要在gluLookAt之後
	light_load();

	//------------Display Objects------------------
	draw_obj();
	
	glutSwapBuffers();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}


void keyboard(unsigned char key, int x, int y){
	switch (key){
		case 'w':{
			z += (-0.1)*radius ;
			//radius += z;
			glutPostRedisplay();
			break;
		}
		case 'a':{
			rot += -10;
			//angle = (angle - 10) % 360;
			glutPostRedisplay();
			break; 
		}
		case 's':{
			z += (0.1)*radius;
			//radius += z;
			glutPostRedisplay();
			break;
		}
		case 'd':{
			rot += 10;
			//angle = (angle + 10) % 360;
			glutPostRedisplay();
			break;
		}
		case '1':{
			select = 0;
			break;
		}
		case '2':{
			select = 1;
			break;
		}
		case '3':{
			select = 2;
			break; 
		}
		case '4':{
			select = 3;
			break; 
		}
		case '5':{
			select = 4;
			break; 
		}
		case '6':{
			select = 5;
			break; 
		}
		case '7':{
			select = 6;
			break; 
		}
		case '8':{
			select = 7;
			break; 
		}
		case '9':{
			select = 8;
			break;
		}
		
		default:
			break;
	
	}
}

void mouse(int button, int state, int x, int y){
	if (state){
		rec_x += (x - old_x);
		rec_y += (y - old_y);
	}
	else{
		old_x = x;
		old_y = y;
	}
}

void MotionMouse(int x, int y){
	crtl_x[select] = (float)(x - old_x)/500*radius;
	crtl_y[select] = (float)(y - old_y)/500*radius;
	cout << "x : " << crtl_x[select] << endl;
	cout << "y : " << crtl_y[select] << endl;

	glutPostRedisplay();
}