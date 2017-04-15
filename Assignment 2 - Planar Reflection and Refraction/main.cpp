#include "mesh.h"
#include "glew.h"
#include "glut.h"
#include "FreeImage.h"
#include <math.h>
#include <fstream>
#include <iostream>
using namespace std;


int cnt = 0;
long long int w, h;

float win;

//object
mesh *object1[50];
char tex_name[50][30];

//record
int mirror = 0;
int back_bear = 2;
int pass = 0;

GLfloat scale_value[50][3] = { 0 };
GLfloat rotation[50][4] = { 0 };
GLfloat transfer[50][3] = { 0 };

int windowSize[2];

//mouse control
GLfloat old_x = 0 , crtl_x[50] = { 0 }, rec_x =  0 ;
GLfloat old_y =  0 , crtl_y[50] = { 0 }, rec_y =  0 ;
int select=-1;


//keyboard control
GLfloat z=0.0f;
GLfloat old_z=0.0f;
GLfloat radius;

//viewing
GLfloat eye[3] = { 0 };
GLfloat vat[3] = { 0 };
GLfloat vup[3] = { 0 };
GLfloat fovy = 0, dnear = 0, dfar = 0;
GLfloat viewport[4] = { 0 };

//lighting
GLfloat light_specular[8][4] = { 0 };
GLfloat light_diffuse[8][4] = { 0 };
GLfloat light_ambient[8][4] = { 0 };
GLfloat light_position[8][4] = { 0 };
GLfloat ambient[4] = { 0 };
int light_cnt = 0;

//texture mapping
GLuint tex_object[50];
int use_tex[50] = { 0 };
//int load_num[50][6] = { 0 }; // [object][tex_num]
int tex_cnt = 0;

//refraction and reflection
GLfloat transmittance = 0.5;
GLfloat reflectance = 0.5;


//camera motion
int botton;
int angle = 0;
int rot = 0;
int dis_x, dis_z;
void zoom(GLfloat);

//function
void view_load();
void light_load();
void obj_load();
void tex_load(char*);

void viewing();
void lighting();
void draw_obj();
void draw_mirror();
void refraction();
void reflection();
void display();
void reshape(GLsizei, GLsizei);
void keyboard(unsigned char, int, int);
void mouse(int, int, int, int);
void MotionMouse(int, int);


int main(int argc, char** argv)
{
	
	view_load();
	light_load();

	glutInit(&argc, argv);
	glutInitWindowSize(w, h);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB| GLUT_DEPTH | GLUT_STENCIL |GLUT_ACCUM);
	glutCreateWindow("Test Scene1");
	glewInit();

	obj_load();
	
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
	glutMotionFunc(MotionMouse);
	glutMainLoop();

	return 0;
}

void tex_load(char* filename){
	
	FIBITMAP* pimage = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
	FIBITMAP* p32bitsImage = FreeImage_ConvertTo32Bits(pimage);
	int iwidth = FreeImage_GetWidth(p32bitsImage);
	int iheight = FreeImage_GetHeight(p32bitsImage);

	glBindTexture(GL_TEXTURE_2D, tex_object[tex_cnt]);   ////////////
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, iwidth, iheight, 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage));
	glGenerateMipmap(GL_TEXTURE_2D);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	
	FreeImage_Unload(p32bitsImage);
	FreeImage_Unload(pimage);
}


void view_load(){
	fstream viewing;
	viewing.open("CornellBox.view", ios::in);
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
		if (!strcmp(c, "viewport")){
			for (size_t i = 0; i < 4; i++){
				viewing >> viewport[i];
				w = viewport[2];
				h = viewport[3];
				//cout << "viewport[" << i << "] = " << viewport[i] << endl;
			}
		}
	}
	for (size_t i = 0; i < 3; i++) cout << "read: " << eye[i] << " ";
	for (size_t i = 0; i < 4; i++) cout << "viewport[" << i << "] = " << viewport[i] << endl;
	cout << endl;
	
	cout << "In view_load: " << endl;
	cout << "viewport[2]: " << viewport[2] << endl;
	cout << "viewport[3]: " << viewport[3] << endl;

	w = viewport[2];
	h = viewport[3];
	win = w * h;
	viewing.close();
}
void viewing(){

	radius = sqrtf(pow(eye[0] - vat[0], 2) + pow(eye[2] - vat[2], 2));
	angle = atan((double)(eye[0] - vat[0]) / (eye[2] - vat[2])) * 180 / 3.1415926;
	cout << "view_x: " << eye[0] << " center_x: " << vat[0] << endl;
	cout << "view_z: " << eye[2] << " center_z: " << vat[2] << endl;
	cout << "Angle: " << angle << endl;
	//zoom

	if (z != 0){
		radius += z;
		z = 0;
	}
	if (rot != 0){
		angle += rot;
		angle %= 360;
		rot = 0;
	}
	eye[0] = vat[0] + radius*(GLfloat)sin(angle*(3.1415926) / 180);
	eye[2] = vat[2] + radius*(GLfloat)cos(angle*(3.1415926) / 180);
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
	light.open("CornellBox.light", ios::in);
	char c[100];
	int j = 0;

	while (light >> c){
		if (!strcmp(c, "light")) {
			cout << j << " light " << endl;
			//position
			for (size_t i = 0; i < 3; i++)	{
				light >> (GLfloat)light_position[j][i];
				cout << light_position[j][i] << " ";
			}
			cout << endl;
			light_position[j][3] = (GLfloat)1.0;
			//ambient
			for (size_t i = 0; i < 3; i++)	{
				light >> (GLfloat)light_ambient[j][i];
				cout << light_ambient[j][i] << " ";
			}
			cout << endl;
			light_ambient[j][3] = (GLfloat)0.0;
			//diffuse
			for (size_t i = 0; i < 3; i++)	{
				light >> (GLfloat)light_diffuse[j][i];
				cout << light_diffuse[j][i] << " ";
			}
			cout << endl;
			light_diffuse[j][3] = (GLfloat)0.0;
			//specular
			for (size_t i = 0; i < 3; i++)	{
				light >> (GLfloat)light_specular[j][i];
				cout << light_specular[j][i] << " ";
			}
			cout << endl;
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
	light_cnt = j;
	light.close();
}
void lighting(){
	glShadeModel(GL_SMOOTH);

	// z buffer enable
	glEnable(GL_DEPTH_TEST);

	// enable lighting
	glEnable(GL_LIGHTING);
	// set light property
	
	for (int i = 0; i < light_cnt; i++){
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
	obj.open("CornellBox.scene", ios::in);
	char c[100];
	char filename[50];
	int obj_num = 0;
	int n;
	//test
	FreeImage_Initialise();
	while (obj >> c){
		if (!strcmp(c, "no-texture")){
			n = -1;
		}

		if (!strcmp(c, "single-texture")){
			obj >> filename;
			strcpy(tex_name[tex_cnt], filename);
			glGenTextures(1, &tex_object[tex_cnt]);
			tex_load(filename);
			cout << "single-texture: " << filename << " " << tex_object[tex_cnt] << endl;
			tex_cnt++;
			
			n = 1;
		}
		
		if (!strcmp(c, "multi-texture")){
			
			
			glGenTextures(2, &tex_object[tex_cnt]);
			for (size_t i = 0; i < 2; i++){
				
				obj >> filename;
				strcpy(tex_name[tex_cnt], filename);
				tex_load(filename);
				cout << "multi-texture: " << filename << " " << tex_object[tex_cnt++] << endl;
				
			}
			tex_load(filename);
			
			n = 2;
		}
		
		if (!strcmp(c, "cube-map")){
			//load cube
			int a = tex_cnt;
			
			FIBITMAP* pimage[6];
			FIBITMAP* p32bitsImage[6] ;
			int iwidth[6];
			int iheight[6] ;

			glGenTextures(1, &tex_object[tex_cnt]);
			glBindTexture(GL_TEXTURE_CUBE_MAP, tex_object[tex_cnt]);   ////////////
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			
			for (size_t i = 0; i < 6; i++){
				obj >> filename;
				strcpy(tex_name[tex_cnt], filename);
				pimage[i] = FreeImage_Load(FreeImage_GetFileType(filename, 0), filename);
				p32bitsImage[i] = FreeImage_ConvertTo32Bits(pimage[i]);
				iwidth[i] = FreeImage_GetWidth(p32bitsImage[i]);
				iheight[i] = FreeImage_GetHeight(p32bitsImage[i]);
				cout << filename << endl;
				
				if (i == 0)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGBA, iwidth[i], iheight[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage[i]));
				else if (i == 1)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGBA, iwidth[i], iheight[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage[i]));
				else if (i == 2)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGBA, iwidth[i], iheight[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage[i]));
				else if (i == 3)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGBA, iwidth[i], iheight[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage[i]));
				else if (i == 4)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGBA, iwidth[i], iheight[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage[i]));
				else if (i == 5)
					glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGBA, iwidth[i], iheight[i], 0, GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32bitsImage[i]));
				
				
				cout << "cube-map: " << filename << " " << tex_object[tex_cnt++] << endl;
				
			}
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
			glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

			for (size_t i = 0; i < 6; i++) FreeImage_Unload(p32bitsImage[i]);
			for (size_t i = 0; i < 6; i++)FreeImage_Unload(pimage[i]);
			n = 6;
		}
		
		if (!strcmp(c, "model")){
			char name[20];

			obj >> name;
			if (!strcmp(name, "Mirror.obj")) mirror = cnt;
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
			use_tex[cnt] = n;
			obj_num++;
			cnt++;
			cout << endl;
			n = 0;
		}	
	}
	FreeImage_DeInitialise();
	cout << "object_tex: " << endl;
	for (size_t i = 0; i < obj_num; i++) cout << use_tex[i] << " ";
	cout << endl;
	obj.close();
}

void draw_obj(){
	int use = 0;
	//int flag = 0;
	int use_tex1[50] = { 0 };
	for (size_t i = 0; i < 50; i++) use_tex1[i] = use_tex[i];
		

	if (pass == 1)glStencilFunc(GL_EQUAL, 1, 1);//是1的位置才畫
	else           glStencilFunc(GL_EQUAL, 0, 1);//是0的位置才畫

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	for (size_t j = 0; j < cnt; j++){
		if (j == mirror) continue;
				glPushMatrix();
				glTranslatef(transfer[j][0] + crtl_x[j], transfer[j][1] - crtl_y[j], transfer[j][2]);
				glRotatef(rotation[j][0], rotation[j][1], rotation[j][2], rotation[j][3]);
				glScalef(scale_value[j][0], scale_value[j][1], scale_value[j][2]);
			
			int lastMaterial = -1;
			for (size_t i = 0; i < object1[j]->fTotal; ++i)
			{
				// set material property if this face used different material
				int tmp_num = 1;
				
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
					if (use_tex1[j] == 1){
							glActiveTexture(GL_TEXTURE0);
							glEnable(GL_TEXTURE_2D);
							glEnable(GL_ALPHA_TEST);
							glAlphaFunc(GL_GREATER, 0.5f);
							glBindTexture(GL_TEXTURE_2D, tex_object[use]);
							glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
							
					}
					else if (use_tex1[j] == 2){
						//bind texture 0
						glActiveTexture(GL_TEXTURE0);
						glEnable(GL_TEXTURE_2D);
						glEnable(GL_ALPHA_TEST);
						glAlphaFunc(GL_GREATER, 0.5f);
						glBindTexture(GL_TEXTURE_2D, tex_object[use]);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

						//bind texture 1
						glActiveTexture(GL_TEXTURE1);
						glEnable(GL_TEXTURE_2D);
						glEnable(GL_ALPHA_TEST);
						glAlphaFunc(GL_GREATER, 0.5f);
						glBindTexture(GL_TEXTURE_2D, tex_object[use+1]);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_COMBINE);
						glTexEnvf(GL_TEXTURE_ENV, GL_COMBINE_RGB, GL_MODULATE);

					}
					else if (use_tex1[j]==6){
						glActiveTexture(GL_TEXTURE0);
						glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glTexGeni(GL_R, GL_TEXTURE_GEN_MODE, GL_REFLECTION_MAP);
						glEnable(GL_TEXTURE_GEN_S);
						glEnable(GL_TEXTURE_GEN_T);
						glEnable(GL_TEXTURE_GEN_R);
						glEnable(GL_TEXTURE_CUBE_MAP);
						glBindTexture(GL_TEXTURE_CUBE_MAP, tex_object[use]);
						glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
					}
					
					if (use_tex1[j + 1] == 0){
						use_tex1[j + 1] = use_tex1[j];
						use = use;
					}
					else if (use_tex1[j] != -1) use += use_tex1[j];
				}
				
				glBegin(GL_TRIANGLES);
				for (size_t k = 0; k < 3; ++k)
				{
					//textex corrd. object->tList[object->faceList[i][j].t].ptr
					if (use_tex1[j] == 1){
						glTexCoord2fv(object1[j]->tList[object1[j]->faceList[i][k].t].ptr);
						glNormal3fv(object1[j]->nList[object1[j]->faceList[i][k].n].ptr);
						glVertex3fv(object1[j]->vList[object1[j]->faceList[i][k].v].ptr);
					}
					else if (use_tex1[j] == 2){
						glMultiTexCoord3fv(GL_TEXTURE0, object1[j]->tList[object1[j]->faceList[i][k].t].ptr);
						glMultiTexCoord3fv(GL_TEXTURE1, object1[j]->tList[object1[j]->faceList[i][k].t].ptr);
						glNormal3fv(object1[j]->nList[object1[j]->faceList[i][k].n].ptr);
						glVertex3fv(object1[j]->vList[object1[j]->faceList[i][k].v].ptr);
					}
					else if (use_tex1[j] == 6){
						glTexCoord2fv(object1[j]->tList[object1[j]->faceList[i][k].t].ptr);
						glNormal3fv(object1[j]->nList[object1[j]->faceList[i][k].n].ptr);
						glVertex3fv(object1[j]->vList[object1[j]->faceList[i][k].v].ptr);
					}
					else {
						glNormal3fv(object1[j]->nList[object1[j]->faceList[i][k].n].ptr);
						glVertex3fv(object1[j]->vList[object1[j]->faceList[i][k].v].ptr);
					}
				}
				glEnd();
			}

			if (use_tex1[j] == 1){
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
			}
			else if (use_tex1[j] == 2){
				glActiveTexture(GL_TEXTURE1);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);

				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_2D);
				glBindTexture(GL_TEXTURE_2D, 0);
				
			}
			else if (use_tex1[j] == 6){
				glActiveTexture(GL_TEXTURE0);
				glDisable(GL_TEXTURE_GEN_S);
				glDisable(GL_TEXTURE_GEN_T);
				glDisable(GL_TEXTURE_GEN_R);
				glDisable(GL_TEXTURE_CUBE_MAP);
				glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
			}
			glPopMatrix();
		}
	
		
}

void draw_mirror(){
	glStencilFunc(GL_ALWAYS, 1, 1);
	glStencilOp(GL_ZERO, GL_ZERO, GL_ZERO);//鏡子改0
	
	int j = mirror;
	glPushMatrix();
	glTranslatef(transfer[j][0] + crtl_x[j], transfer[j][1] - crtl_y[j], transfer[j][2]);
	glRotatef(rotation[j][0], rotation[j][1], rotation[j][2], rotation[j][3]);
	glScalef(scale_value[j][0], scale_value[j][1], scale_value[j][2]);

	int lastMaterial = -1;
	for (size_t i = 0; i < object1[j]->fTotal; ++i)
	{
		// set material property if this face used different material
		int tmp_num = 1;
		int src_alpha = 1;
		if (lastMaterial != object1[j]->faceList[i].m)
		{
			lastMaterial = (int)object1[j]->faceList[i].m;
			object1[j]->mList[lastMaterial].Kd[3] = object1[j]->mList[lastMaterial].Tr; //assign alpha
			glMaterialfv(GL_FRONT, GL_AMBIENT, object1[j]->mList[lastMaterial].Ka);
			glMaterialfv(GL_FRONT, GL_DIFFUSE, object1[j]->mList[lastMaterial].Kd);
			glMaterialfv(GL_FRONT, GL_SPECULAR, object1[j]->mList[lastMaterial].Ks);
			glMaterialfv(GL_FRONT, GL_SHININESS, &object1[j]->mList[lastMaterial].Ns);
			
			//you can obtain the texture name by object->mList[lastMaterial].map_Kd
			//load them once in the main function before mainloop
			//bind them in display function here
		}
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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

void refraction(){
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glClearAccum(0.0, 0.0, 0.0, 0.0);
	
	if (pass == 1)glStencilFunc(GL_EQUAL, 1, 1);//是1的位置才畫
	else           glStencilFunc(GL_EQUAL, 0, 1);//是0的位置才畫
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);
	
	for (size_t j = 0; j < cnt; j++){
		glClear(GL_COLOR_BUFFER_BIT);
		glPushMatrix();
		glTranslatef(transfer[j][0] + crtl_x[j], transfer[j][1] - crtl_y[j], transfer[j][2]);
		glRotatef(rotation[j][0], rotation[j][1], rotation[j][2], rotation[j][3]);
		glScalef(scale_value[j][0], scale_value[j][1], scale_value[j][2]);

		int lastMaterial = -1;
		for (size_t i = 0; i < object1[j]->fTotal; ++i)
		{
			// set material property if this face used different material
			int tmp_num = 1;
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
		glAccum(GL_ACCUM, transmittance);
	}
	
}
void reflection(){
	
	glClear(GL_DEPTH_BUFFER_BIT);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(fovy, -viewport[2] / viewport[3], dfar, dnear);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt((-1)*eye[0] - 40, eye[1], eye[2],
		(-1)*vat[0] - 40, vat[1], vat[2],
		vup[0], vup[1], vup[2]);

	if (pass == 1)glStencilFunc(GL_EQUAL, 1, 1);//是1的位置才畫
	else           glStencilFunc(GL_EQUAL, 0, 1);//是0的位置才畫
	glStencilOp(GL_KEEP, GL_KEEP, GL_REPLACE);

	for (size_t j = 0; j < cnt; j++){
		if (j == back_bear) continue;
			glClear(GL_COLOR_BUFFER_BIT);
			
			glPushMatrix();
			glTranslatef(transfer[j][0] + crtl_x[j], transfer[j][1] - crtl_y[j], transfer[j][2]);
			glRotatef(rotation[j][0], rotation[j][1], rotation[j][2], rotation[j][3]);
			glScalef(scale_value[j][0], scale_value[j][1], scale_value[j][2]);

			int lastMaterial = -1;
			for (size_t i = 0; i < object1[j]->fTotal; ++i)
			{
				// set material property if this face used different material
				int tmp_num = 1;

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
			glAccum(GL_ACCUM, reflectance);
	}
	
}

void draw(){
	if (pass == 1)glStencilFunc(GL_EQUAL, 1, 1);//是1的位置才畫
	else           glStencilFunc(GL_EQUAL, 0, 1);//是0的位置才畫

	glStencilOp(GL_KEEP, GL_KEEP, GL_KEEP);

	for (size_t j = 0; j < cnt; j++){
		if (j == mirror || j==back_bear) continue;
		glPushMatrix();
		glTranslatef(transfer[j][0] + crtl_x[j], transfer[j][1] - crtl_y[j], transfer[j][2]);
		glRotatef(rotation[j][0], rotation[j][1], rotation[j][2], rotation[j][3]);
		glScalef(scale_value[j][0], scale_value[j][1], scale_value[j][2]);

		int lastMaterial = -1;
		for (size_t i = 0; i < object1[j]->fTotal; ++i)
		{
			// set material property if this face used different material
			int tmp_num = 1;

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
	glClearStencil(1);						   // 設定stencil buffer 清除值 = 1
	glClearDepth(1.0f);                        // Depth Buffer (就是z buffer) Setup
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glDepthMask(GL_FALSE);
	glEnable(GL_DEPTH_TEST);                   // Enables Depth Testing
	glEnable(GL_STENCIL_TEST);				   // Enable stencil buffer
	glEnable(GL_CULL_FACE);					   // Enable back face cutting
	glDepthFunc(GL_LEQUAL);                    // The Type Of Depth Test To Do
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT | GL_ACCUM_BUFFER_BIT);//這行把畫面清成黑色並且清除z buffer

	glCullFace(GL_BACK);

	viewing();
		
	//注意light位置的設定，要在gluLookAt之後
	lighting();
	//------------Display Objects------------------
	//set stencil buffer
	draw_mirror();

	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glDepthMask(GL_TRUE);
	glClear(GL_DEPTH_BUFFER_BIT);
	pass = 1;
	draw_obj();
	glDepthMask(GL_FALSE);
	glEnable(GL_BLEND);
	draw_mirror();
	
	//refraction
	glFrontFace(GL_CCW);
	pass = 0;
	refraction();
	glDisable(GL_BLEND);
	
	//reflection
	glFrontFace(GL_CW);
	pass = 0;
	reflection();
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(fovy, viewport[2] / viewport[3], dnear, dfar);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye[0], eye[1], eye[2],
		vat[0] , vat[1], vat[2],
		vup[0], vup[1], vup[2]);

	//combination
	glAccum(GL_RETURN, 1.0);
	glFrontFace(GL_CCW);
	pass = 1;
	draw_obj(); 
	pass = 0;
	draw();	//在鏡子後的東西不畫
	glutSwapBuffers();
	
	glFlush();
}

void reshape(GLsizei w, GLsizei h)
{
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y){
	switch (key){
		case 'w':{
			z = (-0.01)*radius ;
			//radius += z;
			glutPostRedisplay();
			break;
		}
		case 'a':{
			rot = (-2);
			//angle = (angle - 10) % 360;
			glutPostRedisplay();
			break; 
		}
		case 's':{
			z = (0.01)*radius;
			//radius += z;
			glutPostRedisplay();
			break;
		}
		case 'd':{
			rot = 2;
			//angle = (angle + 10) % 360;
			glutPostRedisplay();
			break;
		}
		case 'r':{
			if (reflectance < 1) reflectance += 0.1;
			glutPostRedisplay();
			break;
		}
		case 'f':{
			if (reflectance > 0) reflectance -= 0.1;
			glutPostRedisplay();
			break;
		}
		case 't':{
			if (transmittance < 1) transmittance += 0.1;
			glutPostRedisplay();
			break;
		}
		case 'g':{
			if (transmittance > 0 ) transmittance -= 0.1;
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
	crtl_x[select] = (float)(x - old_x)/viewport[2]*radius;
	crtl_y[select] = (float)(y - old_y)/viewport[3]*radius;
	cout << "x : " << crtl_x[select] << endl;
	cout << "y : " << crtl_y[select] << endl;

	glutPostRedisplay();
}