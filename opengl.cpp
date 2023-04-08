#include <stdio.h>
#include <stdlib.h>
//Always include glew.h before gl.h and glfw3.h
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
using namespace glm;

int main(){
	glewExperimental=true;
	if(!glfwInit()){
		fprintf(stderr,"Failed to initialize GLFW\n");
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES,4);//4x AntiAliasing
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);//OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);//Make MacOS Happy
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	GLFWwindow* window;
	window=glfwCreateWindow(640,480,"Tutorial 1",NULL,NULL);
	if(window==NULL){
		fprintf(stderr,"Failed to open GLFW window.");
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glewExperimental=true;
	if(glewInit()!=GLEW_OK){
		fprintf(stderr,"Failed to initialize GLEW\n");
		return -1;
	}
	
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	do{
		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS&&glfwWindowShouldClose(window)==0);
}