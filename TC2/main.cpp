#include <GL/freeglut.h>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <cmath>
#include "tinyxml2.h"

#define _USE_MATH_DEFINES

//NAMESPACES
using tinyxml2::XMLDocument;
using tinyxml2::XMLNode;
using tinyxml2::XMLElement;
using std::cout;
using std::vector;

struct Circle{
	float centerX,centerY,radius;
	std::string fill,id;
};

struct Rectangle{
	float bottomX,bottomY,width,length;
	std::string fill,id;
};

int alturaJanela,larguraJanela;
Circle biggerCircle, smallerCircle, player;
vector<Circle> enemies;
Rectangle finishLine;

bool keyStatus[256];


void processaArquivoSVG(const XMLNode* node);
void display();
void init(void);
void decideColor(std::string color);
void drawCircle(Circle c);
void mouse(int,int,int,int);
void keyUp(unsigned char,int,int);
void keyPress(unsigned char,int,int);
void idle();

int main(int argc, char** argv) {
	std::string path = argv[1],
				filename = "config.xml";
	path = path + filename;

	XMLDocument* doc = new XMLDocument;
	if(doc->LoadFile(path.c_str())){
		printf("Erro na leitura do arquivo config.xml");
		exit(1);
	}

	const XMLNode* root = doc->FirstChild();
	const XMLNode* svgLine = root->FirstChild();

	const XMLElement* elem = svgLine->ToElement();

	std::string name = elem->FindAttribute("nome")->Value(),
				ext = elem->FindAttribute("tipo")->Value(),
				pathSVG = elem->FindAttribute("caminho")->Value(),dot = ".";
	pathSVG = pathSVG + name + dot+ ext;

	XMLDocument* docSVG = new XMLDocument;
	if(docSVG->LoadFile(pathSVG.c_str())){
		printf("Erro na leitura do arquivo arena.svg\n");
		exit(1);
	}

	const XMLNode* rootSVG = docSVG->FirstChild();

	processaArquivoSVG(rootSVG);

	larguraJanela = biggerCircle.radius * 2;
	alturaJanela = biggerCircle.radius * 2;

	glutInitWindowPosition(0, 0); 
	glutInitWindowSize(larguraJanela, alturaJanela);
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);                      
	glutCreateWindow("Tela"); 

	init();

	glutMouseFunc(mouse);
	glutDisplayFunc(display);
	glutKeyboardFunc(keyPress);
	glutKeyboardUpFunc(keyUp);
	glutIdleFunc(idle); 

	glutMainLoop();    
	return 0;
}

void idle(){
	if(keyStatus['w'] || keyStatus['W']){
		//if(!colisao()){
			player.centerY -= 1;
		//}
	}
	if(keyStatus['s'] || keyStatus['S']){
		//if(!colisao()){
			player.centerY += 1;
		//}
	}
	if(keyStatus['d'] || keyStatus['D']){
		//if(!colisao()){
			player.centerX += 1;
		//} 
	}
	if(keyStatus['a'] || keyStatus['A']){
		//if(!colisao()){
			player.centerX -= 1;
		//} 
	}
	glutPostRedisplay();
}

void keyUp(unsigned char key,int x,int y){
	keyStatus[key] = false;
}

void keyPress(unsigned char key,int x,int y){
	keyStatus[key] = true;
}

void decideColor(std::string color){
	if(!strcmp(color.c_str(),"red")){
		glColor3f(1,0,0);
	}else if(!strcmp(color.c_str(),"blue")){
		glColor3f(0,0,1);
	}else if(!strcmp(color.c_str(),"green")){
		glColor3f(0,1,0);
	}else if(!strcmp(color.c_str(),"white")){
		glColor3f(1,1,1);
	}else{
		glColor3f(0,0,0);
	}
}

void drawCircle(Circle c){

	GLfloat twicePi = 2.0f * M_PI;
	glBegin(GL_TRIANGLE_FAN);
		decideColor(c.fill);
		glVertex3f(c.centerX,alturaJanela - c.centerY,0.0);
		for(int i = 0; i <= 360; i++){
			glVertex3f(c.centerX + (c.radius * cos(i * twicePi/360)),(alturaJanela-c.centerY) + (c.radius * sin(i * twicePi/360)),0.0);
		}
	glEnd();
}

void mouse(int button,int state,int x,int y){
	printf("%d e %d\n",x,alturaJanela-y);
}

void drawRectangle(Rectangle r){
	glBegin(GL_QUADS);
		decideColor(r.fill);
		glVertex3f(r.bottomX,alturaJanela - r.bottomY,0.0); // bottom left
		glVertex3f(r.bottomX,alturaJanela - r.bottomY - r.length,0.0); // top left 
		glVertex3f(r.bottomX + r.width,alturaJanela - r.bottomY - r.length,0.0); // top right
		glVertex3f(r.bottomX + r.width, alturaJanela - r.bottomY,0.0); // bottom right
	glEnd();
}

void display(){
	glClearColor(1,1,1,0); 
	glClear(GL_COLOR_BUFFER_BIT);

	drawCircle(biggerCircle);
	drawCircle(smallerCircle);
	drawRectangle(finishLine);
	for(int i = 0; i < enemies.size(); i++){
		drawCircle(enemies.at(i));
	}
	drawCircle(player);
	glutSwapBuffers();
}

void init(void){
	glClearColor(0.0,0.0,0.0,0.0);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0.0,larguraJanela,0.0,alturaJanela,-1.0,1.0);
}

void processaArquivoSVG(const XMLNode* node){
	/*for(const XMLNode* sonNode = node->FirstChild(); sonNode != nullptr; sonNode = sonNode->NextSibling()){
		const XMLElement* elem = sonNode->ToElement();
		if(!strcmp(elem->Value(),"circle")){
			if(!strcmp(elem->Attribute("id"),"Pista")){
				elem->QueryIntAttribute("cx",&biggerCircle.centerX);
				elem->QueryIntAttribute("cy",&biggerCircle.centerY);
				elem->QueryIntAttribute("r",&biggerCircle.radius);

			}
		}else if(!strcmp(elem->Value(),"rect")){
			
		}
	}*/
	node->FirstChild()->ToElement()->QueryFloatAttribute("cx",&biggerCircle.centerX);
	node->FirstChild()->ToElement()->QueryFloatAttribute("cy",&biggerCircle.centerY);
	node->FirstChild()->ToElement()->QueryFloatAttribute("r",&biggerCircle.radius);
	biggerCircle.fill = node->FirstChild()->ToElement()->Attribute("fill");

	node->FirstChild()->NextSibling()->ToElement()->QueryFloatAttribute("cx",&smallerCircle.centerX);
	node->FirstChild()->NextSibling()->ToElement()->QueryFloatAttribute("cy",&smallerCircle.centerY);
	node->FirstChild()->NextSibling()->ToElement()->QueryFloatAttribute("r",&smallerCircle.radius);
	smallerCircle.fill = node->FirstChild()->NextSibling()->ToElement()->Attribute("fill");

	node->FirstChild()->NextSibling()->NextSibling()->ToElement()->QueryFloatAttribute("x",&finishLine.bottomX);
	node->FirstChild()->NextSibling()->NextSibling()->ToElement()->QueryFloatAttribute("y",&finishLine.bottomY);
	node->FirstChild()->NextSibling()->NextSibling()->ToElement()->QueryFloatAttribute("width",&finishLine.width);
	node->FirstChild()->NextSibling()->NextSibling()->ToElement()->QueryFloatAttribute("height",&finishLine.length);
	finishLine.fill = node->FirstChild()->NextSibling()->NextSibling()->ToElement()->Attribute("fill");

	for(const XMLNode* sonNode = node->FirstChild()->NextSibling()->NextSibling(); strcmp(sonNode->ToElement()->Attribute("id"),"Jogador"); sonNode = sonNode->NextSibling()){
		const XMLElement* elem = sonNode->ToElement();
		Circle enemy;
		elem->QueryFloatAttribute("cx",&enemy.centerX);
		elem->QueryFloatAttribute("cy",&enemy.centerY);
		elem->QueryFloatAttribute("r",&enemy.radius);
		enemy.fill = elem->Attribute("fill");
		enemies.push_back(enemy);
	}

	node->LastChild()->ToElement()->QueryFloatAttribute("cx",&player.centerX);
	node->LastChild()->ToElement()->QueryFloatAttribute("cy",&player.centerY);
	node->LastChild()->ToElement()->QueryFloatAttribute("r",&player.radius);
	player.fill = node->LastChild()->ToElement()->Attribute("fill");
}