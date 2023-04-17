#include <glad/glad.h> //include glad before GLFW
#include <GLFW/glfw3.h>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "include/filesystem.h"
#include "include/shader.h"
#include "include/camera.h"

void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void mouse_callback(GLFWwindow* window,double xpos,double ypos);
void scroll_callback(GLFWwindow* window,double xoffset,double yoffset);
void processInput(GLFWwindow *window);

unsigned int loadTextureRGB(const char file[]);
unsigned int loadTextureRGBA(const char file[]);

unsigned int screenWidth=800;
unsigned int screenHeight=600;
char screenName[]="Window Name";

Camera camera(glm::vec3(0.0f,0.0f,3.0f));
float lastX=screenWidth/2.0f;
float lastY=screenHeight/2.0f;
bool firstMouse=true;

float deltaTime=0.0f;
float lastFrame=0.0f;

glm::vec3 lightPos(1.2f,1.0f,2.0f);

int main(){
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);
#endif

	GLFWwindow* window=glfwCreateWindow(screenWidth,screenHeight,screenName,NULL,NULL);
	if(window==NULL){
		eout<<"Failed to create GLFW window.\n";
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window,framebuffer_size_callback);
	glfwSetCursorPosCallback(window,mouse_callback);
	glfwSetScrollCallback(window,scroll_callback);
	
	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	
	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		eout<<"Failed to initialize GLAD.\n";
		return -1;
	}
	
	glEnable(GL_DEPTH_TEST);
	
	Shader lightingShader("shaders/1.colors.vs","shaders/1.colors.fs");
	Shader lightCubeShader("shaders/1.light_cube.vs","shaders/1.light_cube.fs");
	
 float vertices[] = {
        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f, -0.5f,  
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f, -0.5f,  0.5f, 
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f, -0.5f,  0.5f, 

        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f, -0.5f, 
        -0.5f, -0.5f,  0.5f, 
        -0.5f,  0.5f,  0.5f, 

         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  

        -0.5f, -0.5f, -0.5f, 
         0.5f, -0.5f, -0.5f,  
         0.5f, -0.5f,  0.5f,  
         0.5f, -0.5f,  0.5f,  
        -0.5f, -0.5f,  0.5f, 
        -0.5f, -0.5f, -0.5f, 

        -0.5f,  0.5f, -0.5f, 
         0.5f,  0.5f, -0.5f,  
         0.5f,  0.5f,  0.5f,  
         0.5f,  0.5f,  0.5f,  
        -0.5f,  0.5f,  0.5f, 
        -0.5f,  0.5f, -0.5f, 
    };
    // world space positions of our cubes
    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
	
	unsigned int cubeVAO,VBO,EBO;
	glGenVertexArrays(1,&cubeVAO);
	glGenBuffers(1,&VBO);
	//glGenBuffers(1,&EBO);
	
	glBindVertexArray(cubeVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);
	
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,EBO);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER,sizeof(indices),indices,GL_STATIC_DRAW);
	
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	
	//glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,5*sizeof(float),(void*)(3*sizeof(float)));
	//glEnableVertexAttribArray(1);
	
	unsigned int lightCubeVAO;
	glGenVertexArrays(1,&lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	
	glBindBuffer(GL_ARRAY_BUFFER,VBO);
	
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	
	stbi_set_flip_vertically_on_load(true);
	
	unsigned int texture1=loadTextureRGB("textures/container.jpg");
	unsigned int texture2=loadTextureRGBA("textures/awesomeface.png");
	
	//ourShader.use();
	//ourShader.setInt("texture1",0);
	//ourShader.setInt("texture2",1);
	
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//wireframe polygons
	
	while(!glfwWindowShouldClose(window)){
		float currentFrame=static_cast<float>(glfwGetTime());
		deltaTime=currentFrame-lastFrame;
		lastFrame=currentFrame;
		
		processInput(window);
		
		glClearColor(0.1f,0.1f,0.1f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D,texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D,texture2);
		
		lightingShader.use();
		lightingShader.setVec3("objectColor",1.0f,0.5f,0.31f);
		lightingShader.setVec3("lightColor",1.0f,1.0f,1.0f);
		
		glm::mat4 projection=glm::perspective(glm::radians(camera.Zoom),(float)screenWidth/(float)screenHeight,0.1f,100.0f);
		glm::mat4 view=camera.GetViewMatrix();
		lightingShader.setMat4("projection",projection);
		lightingShader.setMat4("view",view);
		
		glm::mat4 model=glm::mat4(1.0f);
		lightingShader.setMat4("model",model);
		
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES,0,36);
		
		lightCubeShader.use();
		lightCubeShader.setMat4("projection",projection);
		lightCubeShader.setMat4("view",view);
		model=glm::mat4(1.0f);
		model=glm::translate(model,lightPos);
		model=glm::scale(model,glm::vec3(0.2));
		lightCubeShader.setMat4("model",model);
		
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES,0,36);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1,&cubeVAO);
	glDeleteVertexArrays(1,&lightCubeVAO);
	glDeleteBuffers(1,&VBO);
	//glDeleteBuffers(1,&EBO);
	
	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow *window){
	if(glfwGetKey(window,GLFW_KEY_ESCAPE)==GLFW_PRESS){
		glfwSetWindowShouldClose(window,true);
	}
	
	if(glfwGetKey(window,GLFW_KEY_W)==GLFW_PRESS){
		camera.ProcessKeyboard(FORWARD,deltaTime);
	}
	if(glfwGetKey(window,GLFW_KEY_S)==GLFW_PRESS){
		camera.ProcessKeyboard(BACKWARD,deltaTime);
	}
	if(glfwGetKey(window,GLFW_KEY_A)==GLFW_PRESS){
		camera.ProcessKeyboard(LEFT,deltaTime);
	}
	if(glfwGetKey(window,GLFW_KEY_D)==GLFW_PRESS){
		camera.ProcessKeyboard(RIGHT,deltaTime);
	}
}

void framebuffer_size_callback(GLFWwindow* window,int width,int height){
	glViewport(0,0,width,height);
}

void mouse_callback(GLFWwindow* window,double xposIn,double yposIn){
	float xpos=static_cast<float>(xposIn);
	float ypos=static_cast<float>(yposIn);
	if(firstMouse){
		lastX=xpos;lastY=ypos;
		firstMouse=false;
	}
	float xoffset=xpos-lastX;
	float yoffset=lastY-ypos;// reversed since y-coordinates go from bottom to top
	lastX=xpos;lastY=ypos;
	camera.ProcessMouseMovement(xoffset,yoffset);
}

void scroll_callback(GLFWwindow* window,double xoffset,double yoffset){
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
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

