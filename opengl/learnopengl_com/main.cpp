#include <fstream>

#include <glad/glad.h> //include glad before GLFW
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void processInput(GLFWwindow *window);

std::fstream eout("error.log");

unsigned int windowWidth=640;
unsigned int windowHeight=480;

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	
	GLFWwindow* window=glfwCreateWindow(windowWidth,windowHeight,"",NULL,NULL);
	if(window==NULL){
		eout<<"Failed to create GLFW window.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		eout<<"Failed to initialize GLAD.\n";
		return -1;
	}
	
	while(!glfwWindowShouldClose(window)){
		processInput(window);
		
		glClearColor(0.75f,0.75f,0.75f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window){
	if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS){
		glfwSetWindowShouldClose(window,true);
	}
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
	glViewport(0,0,width,height);
}
