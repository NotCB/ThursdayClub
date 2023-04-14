#include <fstream>

#include <glad/glad.h> //include glad before GLFW
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window,int width, int height);
void processInput(GLFWwindow *window);

const char *vertexShaderSource="#version 330 core\n"
	"layout(location=0)in vec3 aPos;\n"
	"void main(){\n"
	"gl_Position=vec4(aPos.x,aPos.y,aPos.z,1.0);\n"
	"}\0";
	
const char *fragmentShaderSource="#version 330 core\n"
	"out vec4 FragColor;\n"
	"void main(){\n"
	"FragColor=vec4(0.34f,0.0f,0.8f,1.0f);\n"
	"}\0";

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
	
	int success;
	char infoLog[512];
	
	unsigned int vertexShader=glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertexShader,1,&vertexShaderSource,NULL);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(vertexShader,512,NULL,infoLog);
		eout<<"ERROR::SHADER::VERTEX::COMPILATION_FAILED\n"<<infoLog<<'\n';
	}
	
	unsigned int fragmentShader=glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragmentShader,1,&fragmentShaderSource,NULL);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader,GL_COMPILE_STATUS,&success);
	if(!success){
		glGetShaderInfoLog(fragmentShader,512,NULL,infoLog);
		eout<<"ERROR::SHADER::FRAGMENT:COMPILATION_FAILED\n"<<infoLog<<'\n';
	}
	
	unsigned int shaderProgram=glCreateProgram();
	glAttachShader(shaderProgram,vertexShader);
	glAttachShader(shaderProgram,fragmentShader);
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram,GL_LINK_STATUS,&success);
	if(!success){
		glGetProgramInfoLog(shaderProgram,512,NULL,infoLog);
		eout<<"ERROR::SHADER::PROGRAM::LINKING_FAILED\n"<<infoLog<<'\n';
	}
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
	
	float vertices[]={
		-0.5f,-0.5f,0.0f,
		0.5f,-0.5f,0.0f,
		0.0f,0.5f,0.0f
	};
	
	unsigned int indices[]={
		0,1,2
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
	glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,3*sizeof(float),(void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER,0);
	glBindVertexArray(0);
	
	//glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);//wireframe polygons
	
	while(!glfwWindowShouldClose(window)){
		processInput(window);
		
		glClearColor(0.75f,0.75f,0.75f,1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		
		glUseProgram(shaderProgram);
		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,0);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	
	glDeleteVertexArrays(1,&VAO);
	glDeleteBuffers(1,&VBO);
	glDeleteBuffers(1,&EBO);
	glDeleteProgram(shaderProgram);
	
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

