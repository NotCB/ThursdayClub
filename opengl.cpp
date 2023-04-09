#include <stdio.h>
#include <stdlib.h>

#include <fstream>
#include <string>
#include <sstream>
#include <vector>

//Always include glew.h before gl.h and glfw3.h
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

int main(void){
	if(!glfwInit()){
		fprintf(stderr,"Failed to initialize GLFW\n");
		getchar();
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES,4);//4x AntiAliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);//OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);//Make MacOS Happy
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window;
	window=glfwCreateWindow(640,480,"Tutorial",NULL,NULL);
	if(window==NULL){
		fprintf(stderr,"Failed to open GLFW window.");
		getchar();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	glewExperimental=true;
	if(glewInit()!=GLEW_OK){
		fprintf(stderr,"Failed to initialize GLEW\n");
		getchar();
		glfwTerminate();
		return -1;
	}
	
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	
	glClearColor(0.0f,0.0f,0.5f,0.0f);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1,&VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	GLuint programID=LoadShaders("vertexshader.glsl","fragmentshader.glsl");
	
	static const GLfloat g_vertex_buffer_data[]={
		-1.0f,-1.0f,0.0f,
		1.0f,-1.0f,0.0f,
		0.0f,1.0f,0.0f
	};
	
	GLuint vertexbuffer;
	glGenBuffers(1,&vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(g_vertex_buffer_data),g_vertex_buffer_data,GL_STATIC_DRAW);
	
	do{
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(programID);
		
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
		glVertexAttribPointer(
			0,			//attribute 0
			3,			//size
			GL_FLOAT,	//type
			GL_FALSE,	//normalized
			0,			//stride
			(void*)0	//array buffer offset
		);
		glDrawArrays(GL_TRIANGLES,0,3);
		glDisableVertexAttribArray(0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS&&glfwWindowShouldClose(window)==0);
	
	//Cleanup
	glDeleteBuffers(1,&vertexbuffer);
	glDeleteVertexArrays(1,&VertexArrayID);
	glDeleteProgram(programID);
	
	glfwTerminate();
	
	return 0;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
	//Create the shaders
	GLuint VertexShaderID=glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
	
	//Read the Vertex Shader code from file
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path,std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr<<VertexShaderStream.rdbuf();
		VertexShaderCode=sstr.str();
		VertexShaderStream.close();
	}else{
		printf("Impossible to open %s.",vertex_file_path);
		getchar();
		return 0;
	}
	
	//Read the Fragment Shader code from file
	std::string FragmentShaderCode;
	std::ifstream FragmentShaderStream(fragment_file_path,std::ios::in);
	if(FragmentShaderStream.is_open()){
		std::stringstream sstr;
		sstr<<FragmentShaderStream.rdbuf();
		FragmentShaderCode=sstr.str();
		FragmentShaderStream.close();
	}
	
	GLint Result=GL_FALSE;
	int InfoLogLength;
	
	//Compile Vertex Shader
	//printf("Compiling shader: %s\n",vertex_file_path);
	char const * VertexSourcePointer=VertexShaderCode.c_str();
	glShaderSource(VertexShaderID,1,&VertexSourcePointer,NULL);
	glCompileShader(VertexShaderID);
	
	//Check Vertex Shader
	glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&Result);
	glGetShaderiv(VertexShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID,InfoLogLength,NULL,&VertexShaderErrorMessage[0]);
		printf("%s\n",&VertexShaderErrorMessage[0]);
	}
	
	//Compile Fragment Shader
	//printf("Compiling shader: %s\n",fragment_file_path);
	char const * FragmentSourcePointer=FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID,1,&FragmentSourcePointer,NULL);
	glCompileShader(FragmentShaderID);
	
	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&Result);
	glGetShaderiv(FragmentShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID,InfoLogLength,NULL,&FragmentShaderErrorMessage[0]);
		printf("%s\n",&FragmentShaderErrorMessage[0]);
	}
	
	//Link program
	//printf("Linking program.\n");
	GLuint ProgramID=glCreateProgram();
	glAttachShader(ProgramID,VertexShaderID);
	glAttachShader(ProgramID,FragmentShaderID);
	glLinkProgram(ProgramID);
	
	//Check program
	glGetProgramiv(ProgramID,GL_LINK_STATUS,&Result);
	glGetProgramiv(ProgramID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID,InfoLogLength,NULL,&ProgramErrorMessage[0]);
		printf("%s\n",&ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID,VertexShaderID);
	glDetachShader(ProgramID,FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	
	return ProgramID;
}