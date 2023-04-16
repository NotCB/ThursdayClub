#include <fstream>

#include <glad/glad.h> //include glad before GLFW
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/eout.h"
#include "include/filesystem.h"
#include "include/shader.h"

void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void processInput(GLFWwindow *window);

unsigned int loadTextureRGB(const char file[]);
unsigned int loadTextureRGBA(const char file[]);

unsigned int windowWidth=1920;
unsigned int windowHeight=1080;
char windowName[]="Window Name";

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
	GLFWwindow* window=glfwCreateWindow(windowWidth,windowHeight,windowName,NULL,NULL);
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
	
	Shader ourShader("vs.glsl","fs.glsl");
	
	float vertices[]={
		-0.5f,0.5f,0.0f,0.0f,1.0f,
		0.5f,0.5f,0.0f,1.0f,1.0f,
		-0.5f,-0.5f,0.0f,0.0f,0.0f,
		0.5f,-0.5f,0.0f,1.0f,0.0f
	};
	unsigned int indices[]={
		0,1,2,
		1,2,3
	};
	
	unsigned int VAO,VBO,EBO;
	glGenVertexArrays(1,&VAO);
	glGenBuffers(1,&VBO);
	glGenBuffers(1,&EBO);
	
	glBindVertexArray(VAO);
	
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
	
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	
	glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	stbi_set_flip_vertically_on_load(true);
	
	unsigned int texture1=loadTextureRGB("textures/container.jpg");
	unsigned int texture2=loadTextureRGBA("textures/awesomeface.png");
	
	ourShader.use();
	ourShader.setInt("texture1",0);
	ourShader.setInt("texture2",1);
	
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//wireframe polygons
	
	while(!glfwWindowShouldClose(window)){
		processInput(window);
		
		glClearColor(0.75f,0.75f,0.75f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texture2);
		
		glm::mat4 transform=glm::mat4(1.0f);
		transform=glm::translate(transform,glm::vec3(0.5f,-0.5f,0.0f));
		transform=glm::rotate(transform,(float)glfwGetTime(),glm::vec3(0.0f,0.0f,1.0f));
		
		ourShader.use();
		unsigned int transformLoc=glGetUniformLocation(ourShader.ID,"transform");
		glUniformMatrix4fv(transformLoc,1,GL_FALSE,glm::value_ptr(transform));
		
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,&EBO);
	
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

unsigned int loadTextureRGB(const char file[]){
	unsigned int texture;
	int width,height,nrChannels;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	unsigned char *data=stbi_load(FileSystem::getPath(file).c_str(),&width,&height,&nrChannels,0);
	if(data){
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_RGB,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
		eout<<"Failed to load texture.\n";
	}
	stbi_image_free(data);
	return texture;
}

unsigned int loadTextureRGBA(const char file[]){
	unsigned int texture;
	int width,height,nrChannels;
	glGenTextures(1,&texture);
	glBindTexture(GL_TEXTURE_2D,texture);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	unsigned char *data=stbi_load(FileSystem::getPath(file).c_str(),&width,&height,&nrChannels,0);
	if(data){
		glTexImage2D(GL_TEXTURE_2D,0,GL_RGBA,width,height,0,GL_RGBA,GL_UNSIGNED_BYTE,data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}else{
		eout<<"Failed to load texture.\n";
	}
	stbi_image_free(data);
	return texture;
}

