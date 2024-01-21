//
// ================================================
// | Grafica pe calculator                        |
// ================================================
// | Laboratorul XI - 11_01_umbra.cpp |
// ====================================
// 
//	Program ce deseneaza o casa si care suprinde efectele de umbrire folosindu-se tehnicile MODERN OpenGL; 
//	Elemente de NOUTATE (modelul de iluminare):
//	 - pentru a genera umbra unei surse este utilizata o matrice 4x4;
//	 - in shaderul de varfuri este inclusa si matricea umbrei;
//	 - in shaderul de fragment umbra este colorata separat;
//	 - sursa de lumina este punctuala(varianta de sursa directionala este comentata);
// 
//  
// 
//	Biblioteci
#include <random>
#include <windows.h>        //	Utilizarea functiilor de sistem Windows (crearea de ferestre, manipularea fisierelor si directoarelor);
#include <stdlib.h>         //  Biblioteci necesare pentru citirea shaderelor;
#include <stdio.h>
#include <math.h>			//	Biblioteca pentru calcule matematice;
#include <GL/glew.h>        //  Definește prototipurile functiilor OpenGL si constantele necesare pentru programarea OpenGL moderna; 
#include <GL/freeglut.h>    //	Include functii pentru: 
							//	- gestionarea ferestrelor si evenimentelor de tastatura si mouse, 
							//  - desenarea de primitive grafice precum dreptunghiuri, cercuri sau linii, 
							//  - crearea de meniuri si submeniuri;
#include "loadShaders.h"	//	Fisierul care face legatura intre program si shadere;
#include "glm/glm.hpp"		//	Bibloteci utilizate pentru transformari grafice;
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/transform.hpp"
#include "glm/gtc/type_ptr.hpp"
#include <SOIL.h>


//  Identificatorii obiectelor de tip OpenGL; 
GLuint
VaoId,
VboId,
EboId,
VaoId2,
VboId2,
EboId2,
VaoId4, VaoId5, VaoId6,
VboId4, VboId5, VboId6,
EboId4, EboId5, EboId6,
ColorBufferId,
ProgramId,
myMatrixLocation,
matrUmbraLocation,
viewLocation,
projLocation,
matrRotlLocation,
lightColorLocation,
lightPosLocation,
viewPosLocation,
codColLocation,
textureIarba,
textureIarbaNormal,
textureBrickWall,
textureBrickWallNormal;



int codCol;
float PI = 3.141592;
float constusy = 200.0f;
float constusx = 200.0f;
float constusz = 200.0f;
// Elemente pentru reprezentarea suprafetei
// (1) intervalele pentru parametrii considerati (u si v)
float const U_MIN = -PI / 2, U_MAX = PI / 2, V_MIN = 0, V_MAX = 2 * PI;
// (2) numarul de paralele/meridiane, de fapt numarul de valori ptr parametri
int const NR_PARR = 10, NR_MERID = 20;
// (3) pasul cu care vom incrementa u, respectiv v
float step_u = (U_MAX - U_MIN) / NR_PARR, step_v = (V_MAX - V_MIN) / NR_MERID;

float const U_MIN2 = -PI, U_MAX2 = PI, V_MIN2 = 0, V_MAX2 = PI;
float step_u2 = (U_MAX2 - U_MIN2) / NR_PARR, step_v2 = (V_MAX2 - V_MIN2) / NR_MERID;
float radius1 = 20;

bool copacus[60][60];

// alte variabile
float radius = 50;
int index, index_aux;

// matrice utilizate
glm::mat4 myMatrix, matrRot;

// elemente pentru matricea de vizualizare
float Refx = 0.0f, Refy = 0.0f, Refz = 0.0f;
float alpha = PI / 8, beta = 0.0f, dist = 400.0f;
float Obsx, Obsy, Obsz;
float Vx = 0.0, Vy = 0.0, Vz = 1.0;
glm::mat4 view;

// elemente pentru matricea de proiectie
float width = 800, height = 600, xwmin = -800.f, xwmax = 800, ywmin = -600, ywmax = 600, znear = 0.1, zfar = 1, fov = 45;
glm::mat4 projection;

// sursa de lumina
float xL = 500.f, yL = 100.f, zL = 400.f;

// matricea umbrei
float matrUmbra[4][4];

void processNormalKeys(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'l':
		Vx -= 0.1;
		break;
	case 'r':
		Vx += 0.1;
		break;
	case '+':
		dist += 5;
		break;
	case '-':
		dist -= 5;
		break;
	}
	if (key == 27)
		exit(0);
}
void processSpecialKeys(int key, int xx, int yy)
{
	switch (key)
	{
	case GLUT_KEY_LEFT:
		beta -= 0.01;
		break;
	case GLUT_KEY_RIGHT:
		beta += 0.01;
		break;
	case GLUT_KEY_UP:
		alpha += 0.01;
		break;
	case GLUT_KEY_DOWN:
		alpha -= 0.01;
		break;
	}
}
void LoadTextureIarba(const char* photoPath) //incarc textura buggati
{
	glGenTextures(1, &textureIarba);
	glBindTexture(GL_TEXTURE_2D, textureIarba);
	//	Desfasurarea imaginii pe orizonatala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 0);//la 0!!!
}

void LoadTextureIarbaNormal(const char* photoPath) //incarc textura buggati
{
	glGenTextures(1, &textureIarbaNormal);
	glBindTexture(GL_TEXTURE_2D, textureIarbaNormal);
	//	Desfasurarea imaginii pe orizonatala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 1);//la 0!!!
}

void LoadTextureBrickWall(const char* photoPath) //incarc textura buggati
{
	glGenTextures(1, &textureBrickWall);
	glBindTexture(GL_TEXTURE_2D, textureBrickWall);
	//	Desfasurarea imaginii pe orizonatala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 2);//la 0!!!
}

void LoadTextureBrickWallNormal(const char* photoPath) //incarc textura buggati
{
	glGenTextures(1, &textureBrickWallNormal);
	glBindTexture(GL_TEXTURE_2D, textureBrickWallNormal);
	//	Desfasurarea imaginii pe orizonatala/verticala in functie de parametrii de texturare;
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	int width, height;
	unsigned char* image = SOIL_load_image(photoPath, &width, &height, 0, SOIL_LOAD_RGB);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
	glGenerateMipmap(GL_TEXTURE_2D);

	SOIL_free_image_data(image);
	glBindTexture(GL_TEXTURE_2D, 3);//la 0!!!
}

void CreateVBO(void)
{

	glm::vec4 Vertices[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Normals[(NR_PARR + 1) * NR_MERID];
	GLushort Indices[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN + parr * step_u; // valori pentru u si v
			float v = V_MIN + merid * step_v;
			float x_vf = radius * cosf(u) * cosf(v); // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius * cosf(u) * sinf(v);
			float z_vf = radius * sinf(u);
			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices[index] = glm::vec4(xL + x_vf + 500.0f, yL + y_vf, zL + z_vf, 1.0);
			//Vertices[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors[index] = glm::vec3(1.0f, 1.0f, 1.0f);
			// Colors[index] = glm::vec3(0.1f + sinf(u), 0.1f + cosf(v), 0.1f + -1.5 * sinf(u));
			Normals[index] = glm::vec3(x_vf, y_vf, 0);
			Indices[index] = index;
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices[(NR_PARR + 1) * NR_MERID + index_aux] = index;
			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices[AUX + 4 * index + 1] = index2;
				Indices[AUX + 4 * index + 2] = index3;
				Indices[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenBuffers(1, &VboId); // atribute
	glGenBuffers(1, &EboId); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors) + sizeof(Normals), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices), Vertices);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices), sizeof(Colors), Colors);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices) + sizeof(Colors), sizeof(Normals), Normals);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices) + sizeof(Colors)));
}

void CreateVBO2(void)
{

	//glm::vec2 delta1 = { 0, 50 }, delta2 = {-50, 0};
	//glm::vec3 edge1 = { 0, 3000, 0 }, edge2 = {-3000, 0, 0};


	// varfurile 
	GLfloat Vertices[] =
	{
		// coordonate                   // culori			// normale			  // texturi     //tangente si bitangente
		// varfuri iarba
	   -15000.0f,  -15000.0f, 0.0f, 1.0f,  0.0f, 0.502f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 0.0f,    60.0f, 0.0f, 0.0f, 0.0f, 60.0f, 0.0f,
		15000.0f,  -15000.0f, 0.0f, 1.0f,  0.0f, 0.502f, 0.0f,  0.0f, 0.0f, 1.0f,   50.0f, 0.0f, 60.0f, 0.0f, 0.0f, 0.0f, 60.0f, 0.0f,
		15000.0f,  15000.0f,  0.0f, 1.0f,  0.0f, 0.502f, 0.0f,  0.0f, 0.0f, 1.0f,   50.0f, 50.0f, 60.0f, 0.0f, 0.0f, 0.0f, 60.0f, 0.0f,
	   -15000.0f,  15000.0f,  0.0f, 1.0f,  0.0f, 0.502f, 0.0f,  0.0f, 0.0f, 1.0f,   0.0f, 50.0f, 60.0f, 0.0f, 0.0f, 0.0f, 60.0f, 0.0f,
	   // varfuri casa (paralelipiped)
		-200.0f,  -100.0f, 0.5f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//4
		 100.0f,  -100.0f,  0.5f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, -1.0f, -1.0f,   1.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//5
		 100.0f,  100.0f,  0.5f, 1.0f,   1.0f, 0.5f, 0.2f,  1.0f, 1.0f, -1.0f,    1.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//6
		-200.0f,  100.0f, 0.5f, 1.0f,    1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, -1.0f,   0.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//7
		//-200.0f,  -100.0f, 150.5f, 1.0f,  1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, 1.0f,  1.0f, 0.0f,//8
		 //100.0f,  -100.0f,  150.5f, 1.0f, 1.0f, 0.5f, 0.2f,  1.0f, -1.0f, 1.0f,   1.0f, 0.0f,//9
		 100.0f,  100.0f,  150.5f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, 1.0f, 1.0f,    1.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//8
		-200.0f,  100.0f, 150.5f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, 1.0f,   0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//9
		// varfuri acoperis (prisma triunghiulara)
		 -220.0f, -120.0f, 0.5f, 1.0f,   1.0f, 0.0f, 0.0f, -40.0f, -69.28f, 80.0f,  0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//12 baza spate-stanga
		 120.0f, -120.0f, 0.5f, 1.0f,    1.0f, 0.0f, 0.0f, 40.0f, -69.28f, 80.0f,   1.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//13 baza fata-stanga
		 120.0f,  120.0f, 0.5f, 1.0f,    1.0f, 0.0f, 0.0f, 80.0f, 0.0f, 80.0f,      1.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//14 baza fata-dreapta
		 -220.0f, 120.0f, 0.5f, 1.0f,	1.0f, 0.0f, 0.0f, 40.0f, 69.28f, 80.0f,     0.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//15 baza spate-dreapta
		 120.0f, 0.0f, 70.5f, 1.0f,     1.0f, 0.0f, 0.0f, -40.0f, 69.28f, 80.0f,    0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//16 varf fata
		 -220.0f, 0.0f, 70.5f, 1.0f,      1.0f, 0.0f, 0.0f, -80.0f, 0.0f, 80.0f,    1.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//17 varf spate

		 -200.0f,  -100.0f, 0.5f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, -1.0f,  1.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//16-4
		 100.0f,  -100.0f,  0.5f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, -1.0f, -1.0f,   0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//17-5
		-200.0f,  100.0f, 0.5f, 1.0f,    1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, -1.0f,   0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//18
		-200.0f,  -100.0f, 150.5f, 1.0f,  1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, 1.0f,  1.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//19
		 100.0f,  -100.0f,  150.5f, 1.0f, 1.0f, 0.5f, 0.2f,  1.0f, -1.0f, 1.0f,   0.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//20
		-200.0f,  100.0f, 150.5f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, 1.0f,   0.0f, 1.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//21

		//-200.0f,  -100.0f, 0.5f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, -1.0f,  0.0f, 0.0f,//22
		 100.0f,  -100.0f,  0.5f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, -1.0f, -1.0f,   0.0f, 0.0f,0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 150.0f,//22
		 100.0f,  100.0f,  0.5f, 1.0f,   1.0f, 0.5f, 0.2f,  1.0f, 1.0f, -1.0f,    1.0f, 0.0f,0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 150.0f,//23
		 //-200.0f,  100.0f, 0.5f, 1.0f,    1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, -1.0f,   0.0f, 1.0f,//25
		 -200.0f,  -100.0f, 150.5f, 1.0f,  1.0f, 0.5f, 0.2f,  -1.0f, -1.0f, 1.0f,  0.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//24
		  100.0f,  -100.0f,  150.5f, 1.0f, 1.0f, 0.5f, 0.2f,  1.0f, -1.0f, 1.0f,   0.0f, 1.0f,0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 150.0f,//25
		  100.0f,  100.0f,  150.5f, 1.0f,  1.0f, 0.5f, 0.2f,  1.0f, 1.0f, 1.0f,    1.0f, 1.0f,0.0f, 200.0f, 0.0f, 0.0f, 0.0f, 150.0f,//26
		 -200.0f,  100.0f, 150.5f, 1.0f,   1.0f, 0.5f, 0.2f,  -1.0f, 1.0f, 1.0f,   1.0f, 0.0f,1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,//27
	};

	// indicii pentru varfuri
	GLubyte Indices[] =
	{
		// desenare iarba
		 1, 2, 0,   2, 0, 3,
		 // desenare fete casa (paralelipiped)
		 5, 6, 4,   6, 4, 7,
		 6, 7, 8,   8, 7, 9,
		 21, 18, 19,   19, 18, 16,
		 19, 16, 20,   20, 16, 17,
		 22, 23, 25,   25, 23, 26,
		 25, 26, 24,   24, 26, 27,
		 // desenare fete acoperis (prisma triunghiulara)
		 10, 11, 12, 10, 12, 13, //baza acoperis
		 11, 12, 14, // triunghi fata acoperis
		 10, 13, 15, // triunghi spate acoperis 
		 10, 11, 14, 10, 14, 15,// fata lateral stanga
		 12, 13, 15, 12, 14, 15,// fata lateral dreapta		
	};
	glGenVertexArrays(1, &VaoId2);
	glGenBuffers(1, &VboId2);
	glGenBuffers(1, &EboId2);
	glBindVertexArray(VaoId2);

	glBindBuffer(GL_ARRAY_BUFFER, VboId2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 18 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 18 * sizeof(GLfloat), (GLvoid*)(10 * sizeof(GLfloat)));
	//tangente
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(GLfloat), (GLvoid*)(12 * sizeof(GLfloat)));
	//bitangente
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(5, 3, GL_FLOAT, GL_FALSE, 18 * sizeof(GLfloat), (GLvoid*)(15 * sizeof(GLfloat)));
	/*
	glGenVertexArrays(1, &VaoId);
	glGenBuffers(1, &VboId);
	glGenBuffers(1, &EboId);
	glBindVertexArray(VaoId);

	glBindBuffer(GL_ARRAY_BUFFER, VboId);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices), Indices, GL_STATIC_DRAW);

	// atributul 0 = pozitie
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)0);
	// atributul 1 = culoare
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(4 * sizeof(GLfloat)));
	// atributul 2 = normale
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 10 * sizeof(GLfloat), (GLvoid*)(7 * sizeof(GLfloat)));
	*/
}

void CreateVAO4(void)
{
	glm::vec4 Vertices2[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors2[(NR_PARR + 1) * NR_MERID];
	GLushort Indices2[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	glm::vec3 Normals2[(NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN2 + parr * step_u2; // valori pentru u si v
			float v = V_MIN2 + merid * step_v2;
			float x_vf = radius1 * cos(u) + constusx; // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius1 * sin(u) + constusy;
			float z_vf = v * radius1;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices2[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors2[index] = glm::vec3(0.65f, 0.17f, 0.17f);
			Indices2[index] = index;
			Normals2[index] = glm::vec3(radius1 * cos(u), radius1 * sin(u), 0);
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices2[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices2[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices2[AUX + 4 * index + 1] = index2;
				Indices2[AUX + 4 * index + 2] = index3;
				Indices2[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId4);
	glBindVertexArray(VaoId4);
	glGenBuffers(1, &VboId4); // atribute
	glGenBuffers(1, &EboId4); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId4);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId4);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2) + sizeof(Normals2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2), sizeof(Colors2), Colors2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2), sizeof(Normals2), Normals2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices2));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices2) + sizeof(Colors2)));
}
void CreateVAO5(void)
{
	glm::vec4 Vertices2[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors2[(NR_PARR + 1) * NR_MERID];
	GLushort Indices2[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	glm::vec3 Normals2[(NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN2 + parr * step_u2; // valori pentru u si v
			float v = V_MIN2 + merid * step_v2;
			float x_vf = radius1 / 1.2 * v * cos(u) + constusx; // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius1 / 1.2 * v * sin(u) + constusy;
			float z_vf = radius1 / 1.2 * v * 2 * (-1) + 140.0f;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices2[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors2[index] = glm::vec3(0.0f, 1.0f, 0.0f);
			Indices2[index] = index;
			Normals2[index] = glm::vec3(v * cos(u), v * sin(u), -v);
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices2[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices2[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices2[AUX + 4 * index + 1] = index2;
				Indices2[AUX + 4 * index + 2] = index3;
				Indices2[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId5);
	glBindVertexArray(VaoId5);
	glGenBuffers(1, &VboId5); // atribute
	glGenBuffers(1, &EboId5); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId5);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId5);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2) + sizeof(Normals2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2), sizeof(Colors2), Colors2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2), sizeof(Normals2), Normals2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices2));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices2) + sizeof(Colors2)));
}
void CreateVAO6(void)
{
	glm::vec4 Vertices2[(NR_PARR + 1) * NR_MERID];
	glm::vec3 Colors2[(NR_PARR + 1) * NR_MERID];
	GLushort Indices2[2 * (NR_PARR + 1) * NR_MERID + 4 * (NR_PARR + 1) * NR_MERID];
	glm::vec3 Normals2[(NR_PARR + 1) * NR_MERID];
	for (int merid = 0; merid < NR_MERID; merid++)
	{
		for (int parr = 0; parr < NR_PARR + 1; parr++)
		{
			// implementarea reprezentarii parametrice 
			float u = U_MIN2 + parr * step_u2; // valori pentru u si v
			float v = V_MIN2 + merid * step_v2;
			float x_vf = radius1 / 1.5 * v * cos(u) + constusx; // coordonatele varfului corespunzator lui (u,v)
			float y_vf = radius1 / 1.5 * v * sin(u) + constusy;
			float z_vf = radius1 / 1.5 * v * 2 * (-1) + 170.0f;

			// identificator ptr varf; coordonate + culoare + indice la parcurgerea meridianelor
			index = merid * (NR_PARR + 1) + parr;
			Vertices2[index] = glm::vec4(x_vf, y_vf, z_vf, 1.0);
			Colors2[index] = glm::vec3(0.0f, 1.0f, 0.0f);
			Indices2[index] = index;
			Normals2[index] = glm::vec3(v * cos(u), v * sin(u), -v);
			// indice ptr acelasi varf la parcurgerea paralelelor
			index_aux = parr * (NR_MERID)+merid;
			Indices2[(NR_PARR + 1) * NR_MERID + index_aux] = index;

			// indicii pentru desenarea fetelor, pentru varful curent sunt definite 4 varfuri
			if ((parr + 1) % (NR_PARR + 1) != 0) // varful considerat sa nu fie Polul Nord
			{
				int AUX = 2 * (NR_PARR + 1) * NR_MERID;
				int index1 = index; // varful v considerat
				int index2 = index + (NR_PARR + 1); // dreapta lui v, pe meridianul urmator
				int index3 = index2 + 1;  // dreapta sus fata de v
				int index4 = index + 1;  // deasupra lui v, pe acelasi meridian
				if (merid == NR_MERID - 1)  // la ultimul meridian, trebuie revenit la meridianul initial
				{
					index2 = index2 % (NR_PARR + 1);
					index3 = index3 % (NR_PARR + 1);
				}
				Indices2[AUX + 4 * index] = index1;  // unele valori ale lui Indices, corespunzatoare Polului Nord, au valori neadecvate
				Indices2[AUX + 4 * index + 1] = index2;
				Indices2[AUX + 4 * index + 2] = index3;
				Indices2[AUX + 4 * index + 3] = index4;
			}
		}
	};

	// generare VAO/buffere
	glGenVertexArrays(1, &VaoId6);
	glBindVertexArray(VaoId6);
	glGenBuffers(1, &VboId6); // atribute
	glGenBuffers(1, &EboId6); // indici

	// legare+"incarcare" buffer
	glBindBuffer(GL_ARRAY_BUFFER, VboId6);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EboId6);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2) + sizeof(Normals2), NULL, GL_STATIC_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(Vertices2), Vertices2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2), sizeof(Colors2), Colors2);
	glBufferSubData(GL_ARRAY_BUFFER, sizeof(Vertices2) + sizeof(Colors2), sizeof(Normals2), Normals2);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(Indices2), Indices2, GL_STATIC_DRAW);

	// atributele; 
	glEnableVertexAttribArray(0); // atributul 0 = pozitie
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (GLvoid*)0);
	glEnableVertexAttribArray(1); // atributul 1 = culoare
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)sizeof(Vertices2));
	glEnableVertexAttribArray(2); // atributul 2 = normala
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GLfloat), (GLvoid*)(sizeof(Vertices2) + sizeof(Colors2)));
}

void DestroyVBO(void)
{
	glDisableVertexAttribArray(5);
	glDisableVertexAttribArray(4);
	glDisableVertexAttribArray(3);
	glDisableVertexAttribArray(2);
	glDisableVertexAttribArray(1);
	glDisableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glDeleteBuffers(1, &VboId);
	glDeleteBuffers(1, &VboId2);
	glDeleteBuffers(1, &EboId);
	glDeleteBuffers(1, &EboId2);

	glBindVertexArray(0);
	glDeleteVertexArrays(1, &VaoId);
	glDeleteVertexArrays(1, &VaoId2);

	glDeleteBuffers(1, &VboId4);
	glDeleteBuffers(1, &EboId4);
	glDeleteVertexArrays(1, &VaoId4);

	glDeleteBuffers(1, &VboId5);
	glDeleteBuffers(1, &EboId5);
	glDeleteVertexArrays(1, &VaoId5);

	glDeleteBuffers(1, &VboId6);
	glDeleteBuffers(1, &EboId6);
	glDeleteVertexArrays(1, &VaoId6);
}

void CreateShaders(void)
{
	ProgramId = LoadShaders("11_01_Shader.vert", "11_01_Shader.frag");
	glUseProgram(ProgramId);
}

void DestroyShaders(void)
{
	glDeleteProgram(ProgramId);
}

bool shouldReturnTrue()
{
	std::random_device rd;
	std::mt19937 gen(rd());

	const double probability = 0.3;

	std::uniform_real_distribution<> dis(0.0, 1.0);
	double randomValue = dis(gen);

	return randomValue < probability;
}
void Initialize(void)
{
	myMatrix = glm::mat4(1.0f);
	matrRot = glm::rotate(glm::mat4(1.0f), PI / 8, glm::vec3(0.0, 0.0, 1.0));
	glClearColor(0.098f, 0.098f, 0.250f, 0.0f); //Cerul albastru-inchis
	CreateVBO();
	CreateVBO2();
	CreateVAO4();
	CreateVAO5();
	CreateVAO6();
	CreateShaders();
	// locatii pentru shader-e
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	matrUmbraLocation = glGetUniformLocation(ProgramId, "matrUmbra");
	viewLocation = glGetUniformLocation(ProgramId, "view");
	projLocation = glGetUniformLocation(ProgramId, "projection");
	lightColorLocation = glGetUniformLocation(ProgramId, "lightColor");
	lightPosLocation = glGetUniformLocation(ProgramId, "lightPos");
	viewPosLocation = glGetUniformLocation(ProgramId, "viewPos");
	codColLocation = glGetUniformLocation(ProgramId, "codCol");

	LoadTextureIarba("Grafica_iarba.png");
	//LoadTextureIarba("brickwall.png");
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textureIarba);
	glUniform1i(glGetUniformLocation(ProgramId, "iarba"), 0);
	LoadTextureIarbaNormal("Grafica_iarba_normal.png");
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textureIarbaNormal);
	glUniform1i(glGetUniformLocation(ProgramId, "iarbaNormal"), 1);
	LoadTextureBrickWall("brickwall.png");
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textureBrickWall);
	glUniform1i(glGetUniformLocation(ProgramId, "brickwall"), 2);
	LoadTextureBrickWallNormal("brickwall_normal1.png");
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textureBrickWallNormal);
	glUniform1i(glGetUniformLocation(ProgramId, "brickwall_normal"), 3);
	for (int i = 0; i < 60; i += 5)
	{
		for (int j = 0; j < 60; j += 5)
		{
			copacus[i][j] = shouldReturnTrue();
		}
	}
}

void RenderFunction(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);

	//pozitia observatorului
	Obsx = Refx + dist * cos(alpha) * cos(beta);
	Obsy = Refy + dist * cos(alpha) * sin(beta);
	Obsz = Refz + dist * sin(alpha);

	// matrice de vizualizare + proiectie
	glm::vec3 Obs = glm::vec3(Obsx, Obsy, Obsz);   // se schimba pozitia observatorului	
	glm::vec3 PctRef = glm::vec3(Refx, Refy, Refz); // pozitia punctului de referinta
	glm::vec3 Vert = glm::vec3(Vx, Vy, Vz); // verticala din planul de vizualizare 
	view = glm::lookAt(Obs, PctRef, Vert);
	glUniformMatrix4fv(viewLocation, 1, GL_FALSE, &view[0][0]);
	projection = glm::infinitePerspective(fov, GLfloat(width) / GLfloat(height), znear);
	glUniformMatrix4fv(projLocation, 1, GL_FALSE, &projection[0][0]);

	// matricea pentru umbra
	float D = -0.5f;
	matrUmbra[0][0] = zL + D; matrUmbra[0][1] = 0; matrUmbra[0][2] = 0; matrUmbra[0][3] = 0;
	matrUmbra[1][0] = 0; matrUmbra[1][1] = zL + D; matrUmbra[1][2] = 0; matrUmbra[1][3] = 0;
	matrUmbra[2][0] = -xL; matrUmbra[2][1] = -yL; matrUmbra[2][2] = D; matrUmbra[2][3] = -1;
	matrUmbra[3][0] = -D * xL; matrUmbra[3][1] = -D * yL; matrUmbra[3][2] = -D * zL; matrUmbra[3][3] = zL;
	glUniformMatrix4fv(matrUmbraLocation, 1, GL_FALSE, &matrUmbra[0][0]);

	// Variabile uniforme pentru iluminare
	glUniform3f(lightColorLocation, 1.0f, 1.0f, 1.0f);
	glUniform3f(lightPosLocation, xL, yL, zL);
	glUniform3f(viewPosLocation, Obsx, Obsy, Obsz);

	// desenare Luna
	glBindVertexArray(VaoId);
	codCol = 0;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	//glUniform1i(codColLocation, codCol);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	//desenare casa
	glBindVertexArray(VaoId2);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);

	codCol = 2;
	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_BYTE, 0);

	codCol = 3;

	glUniform1i(codColLocation, codCol);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)(6));

	codCol = 0;
	glUniform1i(codColLocation, codCol);
	// desenare acoperis
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 150.0));
	myMatrixLocation = glGetUniformLocation(ProgramId, "myMatrix");
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 24, GL_UNSIGNED_BYTE, (void*)(42));

	// desenare umbra casa
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_BYTE, (void*)(6));

	// desenare umbra acoperis
	myMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(0.f, 0.f, 150.0));
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	glDrawElements(GL_TRIANGLES, 18, GL_UNSIGNED_BYTE, (void*)(42));


	//FACERE COPAAAAAAAAAAAC
	////////////////////////
	glBindVertexArray(VaoId4);

	codCol = 0;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	codCol = 1;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_QUADS,
				4,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}

	glBindVertexArray(VaoId5);

	codCol = 0;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_TRIANGLE_FAN,
				NR_MERID,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	glDrawElements(
		GL_TRIANGLE_FAN,
		NR_MERID,
		GL_UNSIGNED_SHORT,
		(GLvoid*)((0) * sizeof(GLushort)));

	codCol = 1;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_TRIANGLE_FAN,
				NR_MERID,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	glDrawElements(
		GL_TRIANGLE_FAN,
		NR_MERID,
		GL_UNSIGNED_SHORT,
		(GLvoid*)((0) * sizeof(GLushort)));


	glBindVertexArray(VaoId6);

	codCol = 0;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_TRIANGLE_FAN,
				NR_MERID,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	glDrawElements(
		GL_TRIANGLE_FAN,
		NR_MERID,
		GL_UNSIGNED_SHORT,
		(GLvoid*)((0) * sizeof(GLushort)));
	//!!!
	//! 
	//! UMBREEEE
	codCol = 1;
	glUniform1i(codColLocation, codCol);
	myMatrix = glm::mat4(1.0f);
	glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &myMatrix[0][0]);
	for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
	{
		if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
			glDrawElements(
				GL_TRIANGLE_FAN,
				NR_MERID,
				GL_UNSIGNED_SHORT,
				(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
	}
	glDrawElements(
		GL_TRIANGLE_FAN,
		NR_MERID,
		GL_UNSIGNED_SHORT,
		(GLvoid*)((0) * sizeof(GLushort)));
	//!!!



	glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f, 50.0f, 0.0f));
	glm::mat4 scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(1.5f, 1.5f, 1.5f));
	glm::mat4 modelMatrix = translationMatrix * scalingMatrix;
	//FACERE COPAAAAAAAAAAAC
	////////////////////////

	for (int i = -30; i < 30; i+=5)
	{
		for (int j = -30; j < 30; j+=5)
		{		
			if (!((i * j == 25) && (i == -5) && (j == -5)) && !((i * j == 0) && (i == 0) && (j == -5)))
			{
				if (copacus[i + 30][j + 30] == true)
				{
					glBindVertexArray(VaoId4);

					codCol = 0;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
					modelMatrix = translationMatrix * scalingMatrix;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_QUADS,
								4,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}

					codCol = 1;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
					modelMatrix = translationMatrix * scalingMatrix;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_QUADS,
								4,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}

					glBindVertexArray(VaoId5);

					codCol = 0;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
					modelMatrix = translationMatrix * scalingMatrix;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));

					codCol = 1;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
					modelMatrix = translationMatrix * scalingMatrix;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));


					glBindVertexArray(VaoId6);

					codCol = 0;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
					modelMatrix = translationMatrix * scalingMatrix;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));
					//!!!
					//! 
					//! UMBREEEE
					codCol = 1;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					scalingMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(2.5f, 2.5f, 2.5f));
					modelMatrix = translationMatrix * scalingMatrix;
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &modelMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));
					//!!!
				}
				else
				{
					glBindVertexArray(VaoId4);

					codCol = 0;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_QUADS,
								4,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}

					codCol = 1;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_QUADS,
								4,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}

					glBindVertexArray(VaoId5);

					codCol = 0;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));

					codCol = 1;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));


					glBindVertexArray(VaoId6);

					codCol = 0;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));
					//!!!
					//! 
					//! UMBREEEE
					codCol = 1;
					glUniform1i(codColLocation, codCol);
					myMatrix = glm::mat4(1.0f);
					translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(50.0f * i, 50.0f * j, 0.0f));
					glUniformMatrix4fv(myMatrixLocation, 1, GL_FALSE, &translationMatrix[0][0]);
					for (int patr = 0; patr < (NR_PARR + 1) * NR_MERID; patr++)
					{
						if ((patr + 1) % (NR_PARR + 1) != 0) // nu sunt considerate fetele in care in stanga jos este Polul Nord
							glDrawElements(
								GL_TRIANGLE_FAN,
								NR_MERID,
								GL_UNSIGNED_SHORT,
								(GLvoid*)((2 * (NR_PARR + 1) * (NR_MERID)+4 * patr) * sizeof(GLushort)));
					}
					glDrawElements(
						GL_TRIANGLE_FAN,
						NR_MERID,
						GL_UNSIGNED_SHORT,
						(GLvoid*)((0) * sizeof(GLushort)));
					//!!!
				}
			}
		}
	}


	glutSwapBuffers();
	glFlush();
}
void Cleanup(void)
{
	DestroyShaders();
	DestroyVBO();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);
	glutInitWindowPosition(100, 100);
	glutInitWindowSize(1200, 900);
	glutCreateWindow("Iluminare - Umbre - OpenGL <<nou>>");
	glewInit();
	Initialize();
	glutIdleFunc(RenderFunction);
	glutDisplayFunc(RenderFunction);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutCloseFunc(Cleanup);
	glutMainLoop();
}