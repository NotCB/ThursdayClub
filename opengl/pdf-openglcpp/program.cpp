// 126 / 535

#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <cmath>
#include <fstream>
#include <string>

std::fstream eout("error.log",std::ios::out);

class Utils{
public:
	GLuint createShaderProgram(const char *vp,const char *fp){
		GLint vertCompiled,fragCompiled,linked;
		GLuint vShader=glCreateShader(GL_VERTEX_SHADER);
		GLuint fShader=glCreateShader(GL_FRAGMENT_SHADER);
		std::string vertShaderStr=readShaderSource(vp);
		std::string fragShaderStr=readShaderSource(fp);
		const char* vertShaderSrc=vertShaderStr.c_str();
		const char* fragShaderSrc=fragShaderStr.c_str();
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
	
	//GLuint createShaderProgram(const char *vp,const char *gp,const char *fp){}
	//GLuint createShaderProgram(const char *vp,const char *tCS,const char *tES,const char *fp){}
	//GLuint createShaderProgram(const char *vp,const char *tCS,const char *tES,const char *gp,const char *fp){}
	
	/*GLuint loadTexture(const char *texImagePath){
		GLuint textureID;
		textureID=SOIL_load_OGL_texture(texImagePath,
		SOIL_LOAD_AUTO,SOIL_CREATE_NEW_ID,SOIL_FLAG_INVERT_Y);
		if(textureID=0){eout<<"Could not find texture file "<<texImagePath<<'\n';}
		return textureID;
	}*/
	
private:
	std::string readShaderSource(const char* filePath){
		std::string content;
		std::ifstream fileStream(filePath,std::ios::in);
		std::string line="";
		while(!fileStream.eof()){
			getline(fileStream,line);
			content.append(line+'\n');
		}
		fileStream.close();
		return content;
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
};

#define numVAOs 1
#define numVBOs 2

float cameraX,cameraY,cameraZ;
float cubeLocX,cubeLocY,cubeLocZ;
GLuint renderingProgram;
GLuint vao[numVAOs];
GLuint vbo[numVBOs];

GLuint mvLoc,projLoc;
float aspect;
glm::mat4 pMat,vMat,mMat,mvMat,tMat,rMat;

void init(GLFWwindow* window);
void display(GLFWwindow* window,double currentTime);
void setupVertices();
void window_reshape_callback(GLFWwindow* window,int newWidth,int newHeight);

int width=640;
int height=480;

int main(void){
	if(!glfwInit()){exit(EXIT_FAILURE);}
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	GLFWwindow* window=glfwCreateWindow(width,height,"",NULL,NULL);
	glfwMakeContextCurrent(window);
	if(glewInit()!=GLEW_OK){exit(EXIT_FAILURE);}
	glfwSwapInterval(1);
	glfwSetWindowSizeCallback(window,window_reshape_callback);
	
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
	Utils initUtils;
	renderingProgram=initUtils.createShaderProgram("vShader.glsl","fShader.glsl");
	cameraX=0.0f;cameraY=0.0f;cameraZ=8.0f;
	cubeLocX=0.0f;cubeLocY=-2.0f;cubeLocZ=0.0f;
	setupVertices();
	glfwGetFramebufferSize(window,&width,&height);
	aspect=(float)width/(float)height;
	pMat=glm::perspective(1.0472f,aspect,0.1f,1000.0f);
}

void display(GLFWwindow* window,double currentTime){
	glClear(GL_DEPTH_BUFFER_BIT);
	glClear(GL_COLOR_BUFFER_BIT);
	glUseProgram(renderingProgram);
	
	mvLoc=glGetUniformLocation(renderingProgram,"mv_matrix");
	projLoc=glGetUniformLocation(renderingProgram,"proj_matrix");
	
	vMat=glm::translate(glm::mat4(1.0f),glm::vec3(-cameraX,-cameraY,-cameraZ));
	mMat=glm::translate(glm::mat4(1.0f),glm::vec3(cubeLocX,cubeLocY,cubeLocZ));
	
	tMat=glm::translate(glm::mat4(1.0f),glm::vec3(sin(0.35f*currentTime)*2.0f,cos(0.52f*currentTime)*2.0f,sin(0.7*currentTime)*2.0f));
	rMat=glm::rotate(glm::mat4(1.0f),1.75f *(float)currentTime,glm::vec3(0.0f,1.0f,0.0f));
	rMat=glm::rotate(rMat,1.75f *(float)currentTime,glm::vec3(1.0f,0.0f,0.0f));
	rMat=glm::rotate(rMat,1.75f *(float)currentTime,glm::vec3(0.0f,0.0f,1.0f));
	mMat=tMat*rMat;
	
	mvMat=vMat*mMat;
	
	glUniformMatrix4fv(mvLoc,1,GL_FALSE,glm::value_ptr(mvMat));
	glUniformMatrix4fv(projLoc,1,GL_FALSE,glm::value_ptr(pMat));
	
	glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,0,0);
	glEnableVertexAttribArray(0);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);
	glDrawArrays(GL_TRIANGLES,0,36);
}

void setupVertices(){
	float vertexPositions[108]={
		-1.0f,1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,1.0f,1.0f,-1.0f,-1.0f,1.0f,-1.0f,
		1.0,-1.0f,-1.0f,1.0f,-1.0f,1.0f,1.0f,1.0f,-1.0f,
		1.0f,-1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,-1.0f,
		1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,-1.0f,1.0f,1.0f,1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,1.0f,
		-1.0f,-1.0f,-1.0f,-1.0f,1.0f,-1.0f,-1.0f,1.0f,1.0f,
		-1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,1.0f,-1.0f,-1.0f,
		1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,1.0f,
		-1.0f,1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,1.0f,1.0f,
		1.0f,1.0f,1.0f,-1.0f,1.0f,1.0f,-1.0f,1.0f,-1.0f
	};
	
	glGenVertexArrays(1,vao);
	glBindVertexArray(vao[0]);
	glGenBuffers(numVBOs,vbo);
	glBindBuffer(GL_ARRAY_BUFFER,vbo[0]);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertexPositions),vertexPositions,GL_STATIC_DRAW);
}

void window_reshape_callback(GLFWwindow* window,int newWidth,int newHeight){
	aspect=(float)newWidth /(float)newHeight;
	glViewport(0,0,newWidth,newHeight);
	pMat=glm::perspective(1.0472f,aspect,0.1f,1000.0f);
}