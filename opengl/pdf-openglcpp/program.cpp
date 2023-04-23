// 48 / 535

#include <fstream>
#include <string>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

std::fstream eout("error.log",std::ios::out);

#define numVAOs 1

GLuint renderingProgram;
GLuint vao[numVAOs];

void init(GLFWwindow* window);
void display(GLFWwindow* window,double currentTime);
GLuint createShaderProgram();
void printShaderLog(GLuint shader);
void printProgramLog(int prog);
bool checkOpenGLError();
std::string readShaderSource(const char* filePath);

int main(void){
	if(!glfwInit()){exit(EXIT_FAILURE);}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	GLFWwindow* window=glfwCreateWindow(640,480,"",NULL,NULL);
	glfwMakeContextCurrent(window);
	if(glewInit()!=GLEW_OK){exit(EXIT_FAILURE);}
	glfwSwapInterval(1);
	
	init(window);
	
	while(!glfwWindowShouldClose(window)){
		display(window,glfwGetTime());
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(EXIT_SUCCESS);
}

void init(GLFWwindow* window){
	renderingProgram=createShaderProgram();
	glGenVertexArrays(numVAOs,vao);
	glBindVertexArray(vao[0]);
}

void display(GLFWwindow* window,double currentTime){
	//glClearColor(0.75f,0.75f,0.75f,1.0f);
	//glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	glDrawArrays(GL_TRIANGLES,0,3);
}

GLuint createShaderProgram(){
	GLint vertCompiled,fragCompiled,linked;
	GLuint vShader=glCreateShader(GL_VERTEX_SHADER);
	GLuint fShader=glCreateShader(GL_FRAGMENT_SHADER);
	
	string vertShaderStr=readShaderSource("vertShader.glsl");
	string fragShaderStr=readShaderSource("fragShader.glsl");
	const char* vertShaderSrc=vertShader.c_str();
	const char* fragShaderSrc=fragShader.c_str();
	glShaderSource(vShader,1,&vertShaderSrc,NULL);
	glShaderSource(fShader,1,&fragShaderSrc,NULL);
	
	glCompileShader(vShader);
	checkOpenGLError();
	glGetShaderiv(vShader,GL_COMPILE_STATUS,&vertCompiled);
	if(vertCompiled!=1){
		eout<<"Vertex Compilation Failed!\n";
		printShaderLog(vShader);
	}
	
	glCompileShader(fShader);
	checkOpenGLError();
	glGetShaderiv(fShader,GL_COMPILE_STATUS,&fragCompiled);
	if(fragCompiled!=1){
		eout<<"Fragment Compilation Failed!\n";
		printShaderLog(fShader);
	}
	
	GLuint vfProgram=glCreateProgram();
	glAttachShader(vfProgram,vShader);
	glAttachShader(vfProgram,fShader);
	
	glLinkProgram(vfProgram);
	checkOpenGLError();
	glGetProgramiv(vfProgram,GL_LINK_STATUS,&linked);
	if(linked!=1){
		eout<<"Linking Failed!\n";
		printProgramLog(vfProgram);
	}
	return vfProgram;
}

void printShaderLog(GLuint shader){
	int len=0;
	int chWrittn=0;
	char* log;
	glGetShaderiv(shader,GL_INFO_LOG_LENGTH,&len);
	if(len>0){
		log=(char*)malloc(len);
		glGetShaderInfoLog(shader,len,&chWrittn,log);
		eout<<"Shader Info Log: "<<log<<'\n';
		free(log);
	}
}

void printProgramLog(int prog){
	int len=0;
	int chWrittn=0;
	char* log;
	glGetProgramiv(prog,GL_INFO_LOG_LENGTH,&len);
	if(len>0){
		log=(char*)malloc(len);
		glGetProgramInfoLog(prog,len,&chWrittn,log);
		eout<<"Program Info Log: "<<log<<'\n';
		free(log);
	}
}

bool checkOpenGLError(){
	bool foundError=false;
	int glErr=glGetError();
	while(glErr!=GL_NO_ERROR){
		eout<<"glError: "<<glErr<<'\n';
		foundError=true;
		glErr=glGetError();
	}
	return foundError;
}

std::string readShaderSource(const char* filePath){
	std::string content;
	std::ifstream fileStream(filePath,ios::in);
	string line="";
	while(!fileStream.eof()){
		getline(fileStream,line);
		content.append(line+'\n');
	}
	fileStream.close();
	return content;
}

