#include <cstring>
#include <fstream>
#include <sstream>
#include <vector>

//Always include glew.h before gl.h and glfw3.h
#include <GL/glew.h>
#include <GLFW/glfw3.h>
//#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
//using namespace glm;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);
GLuint loadBMP_custom(const char * imagepath);

//#define FOURCC_DXT1 0x31545844 //"DXT1"
//#define FOURCC_DXT3 0x33545844 //"DXT3"
//#define FOURCC_DXT5 0x35545844 //"DXT5"
//GLuint loadDDS(const char * imagepath);

int main(){
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
		glfwTerminate();
		return -1;
	}
	
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1,&VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	GLuint programID=LoadShaders("vertexshader.glsl","fragmentshader.glsl");
	
	GLuint MatrixID=glGetUniformLocation(programID,"MVP");
	
	glm::mat4 Projection=glm::perspective(glm::radians(45.0f),4.0f/3.0f,0.1f,100.0f);
	glm::mat4 View=glm::lookAt(
		glm::vec3(4,3,3),
		glm::vec3(0,0,0),
		glm::vec3(0,1,0)
	);
	glm::mat4 Model=glm::mat4(1.0f);
	glm::mat4 MVP=Projection *View *Model;
	
	GLuint Texture=loadBMP_custom("uvtemplate.bmp");
	GLuint textureID=glGetUniformLocation(programID,"myTextureSampler");
	
static const GLfloat g_vertex_buffer_data[] = { 
		-1.0f,-1.0f,-1.0f,-1.0f,-1.0f, 1.0f,-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,-1.0f,-1.0f,-1.0f, 1.0f,-1.0f,-1.0f,
		 1.0f, 1.0f,-1.0f, 1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,
		-1.0f,-1.0f,-1.0f,-1.0f, 1.0f, 1.0f,-1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f, 1.0f,-1.0f,-1.0f, 1.0f,-1.0f,-1.0f,-1.0f,
		-1.0f, 1.0f, 1.0f,-1.0f,-1.0f, 1.0f, 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f, 1.0f,-1.0f,-1.0f, 1.0f, 1.0f,-1.0f,
		 1.0f,-1.0f,-1.0f, 1.0f, 1.0f, 1.0f, 1.0f,-1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,-1.0f,-1.0f, 1.0f,-1.0f,
		 1.0f, 1.0f, 1.0f,-1.0f, 1.0f,-1.0f,-1.0f, 1.0f, 1.0f,
		 1.0f, 1.0f, 1.0f,-1.0f, 1.0f, 1.0f, 1.0f,-1.0f, 1.0f
	};
	
	static const GLfloat g_uv_buffer_data[] = { 
		0.000059f, 1.0f-0.000004f, 
		0.000103f, 1.0f-0.336048f, 
		0.335973f, 1.0f-0.335903f, 
		1.000023f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.336024f, 1.0f-0.671877f, 
		0.667969f, 1.0f-0.671889f, 
		1.000023f, 1.0f-0.000013f, 
		0.668104f, 1.0f-0.000013f, 
		0.667979f, 1.0f-0.335851f, 
		0.000059f, 1.0f-0.000004f, 
		0.335973f, 1.0f-0.335903f, 
		0.336098f, 1.0f-0.000071f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.336024f, 1.0f-0.671877f, 
		1.000004f, 1.0f-0.671847f, 
		0.999958f, 1.0f-0.336064f, 
		0.667979f, 1.0f-0.335851f, 
		0.668104f, 1.0f-0.000013f, 
		0.335973f, 1.0f-0.335903f, 
		0.667979f, 1.0f-0.335851f, 
		0.335973f, 1.0f-0.335903f, 
		0.668104f, 1.0f-0.000013f, 
		0.336098f, 1.0f-0.000071f, 
		0.000103f, 1.0f-0.336048f, 
		0.000004f, 1.0f-0.671870f, 
		0.336024f, 1.0f-0.671877f, 
		0.000103f, 1.0f-0.336048f, 
		0.336024f, 1.0f-0.671877f, 
		0.335973f, 1.0f-0.335903f, 
		0.667969f, 1.0f-0.671889f, 
		1.000004f, 1.0f-0.671847f, 
		0.667979f, 1.0f-0.335851f
	};
	
	GLuint vertexbuffer;
	glGenBuffers(1,&vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER,sizeof(g_vertex_buffer_data),g_vertex_buffer_data,GL_STATIC_DRAW);
	
	GLuint uvbuffer;
	glGenBuffers(1, &uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
	glBufferData(GL_ARRAY_BUFFER, sizeof(g_uv_buffer_data), g_uv_buffer_data, GL_STATIC_DRAW);
	
	do{
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(programID);
		
		glUniformMatrix4fv(MatrixID,1,GL_FALSE,&MVP[0][0]);
		
		//Array 0: Vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
		glVertexAttribPointer(
			0,			//attribute 0
			3,			//size
			GL_FLOAT,	//type
			GL_FALSE,	//normalized
			0,			//stride
			(void *)0	//array buffer offset
		);
		
		//Array 1: UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER,uvbuffer);
		glVertexAttribPointer(
			1,
			2,
			GL_FLOAT,
			GL_FALSE,
			0,
			(void *)0
		);
		
		glDrawArrays(GL_TRIANGLES,0,12*3);
		
		//Disable Arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS&&glfwWindowShouldClose(window)==0);
	
	//Cleanup
	glDeleteBuffers(1,&vertexbuffer);
	glDeleteBuffers(1,&uvbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1,&Texture);
	glDeleteVertexArrays(1,&VertexArrayID);
	
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
	char const * VertexSourcePointer=VertexShaderCode.c_str();
	glShaderSource(VertexShaderID,1,&VertexSourcePointer,NULL);
	glCompileShader(VertexShaderID);
	
	//Check Vertex Shader
	glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&Result);
	glGetShaderiv(VertexShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID,InfoLogLength,NULL,&VertexShaderErrorMessage[0]);
		printf("%s \n",&VertexShaderErrorMessage[0]);
	}
	
	//Compile Fragment Shader
	char const * FragmentSourcePointer=FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID,1,&FragmentSourcePointer,NULL);
	glCompileShader(FragmentShaderID);
	
	//Check Fragment Shader
	glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&Result);
	glGetShaderiv(FragmentShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID,InfoLogLength,NULL,&FragmentShaderErrorMessage[0]);
		printf("%s \n",&FragmentShaderErrorMessage[0]);
	}
	
	//Link program
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
		printf("%s \n",&ProgramErrorMessage[0]);
	}
	
	glDetachShader(ProgramID,VertexShaderID);
	glDetachShader(ProgramID,FragmentShaderID);
	
	glDeleteShader(VertexShaderID);
	glDeleteShader(FragmentShaderID);
	
	return ProgramID;
}

GLuint loadBMP_custom(const char * imagepath){
	unsigned char header[54];
	unsigned int dataPos;
	unsigned int imageSize;
	unsigned int width,height;
	unsigned char * data;
	
	FILE * file=fopen(imagepath,"rb");
	if(!file){
		printf("%s could not be opened.",imagepath);
		getchar();
		return 0;
	}
	
	if(fread(header,1,54,file)!=54){printf("Not a correct BMP file \n");fclose(file);return 0;}
	if(header[0]!='B'||header[1]!='M'){printf("Not a correct BMP file \n");fclose(file);return 0;}
	if(*(int *)&(header[0x1E])!=0){printf("Not a correct BMP file \n");fclose(file);return 0;}
	if(*(int *)&(header[0x1C])!=24){printf("Not a correct BMP file \n");fclose(file);return 0;}
	
	dataPos=*(int *)&(header[0x0A]);
	imageSize=*(int *)&(header[0x22]);
	width=*(int *)&(header[0x12]);
	height=*(int *)&(header[0x16]);
	
	if(imageSize==0){imageSize=width *height *3;}
	if(dataPos==0){dataPos=54;}
	
	data=new unsigned char [imageSize];
	
	fread(data,1,imageSize,file);
	fclose(file);
	
	GLuint textureID;
	glGenTextures(1,&textureID);
	glBindTexture(GL_TEXTURE_2D,textureID);
	glTexImage2D(GL_TEXTURE_2D,0,GL_RGB,width,height,0,GL_BGR,GL_UNSIGNED_BYTE,data);
	delete [] data;
	
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
	glGenerateMipmap(GL_TEXTURE_2D);
	
	return textureID;
}

/*GLuint loadDDS(const char * imagepath){
	unsigned char header[124];
	FILE *fp;
	
	fp=fopen(imagepath,"rb");
	if(fp==NULL){printf("%s could not be opened.",imagepath);getchar();return 0;}
	
	char filecode[4];
	fread(filecode,1,4,fp);
	if(strncmp(filecode,"DDS ",4)!=0){fclose(fp);return 0;}
	
	fread(&header,124,1,fp);
	unsigned int height=*(unsigned int *)&(header[8]);
	unsigned int width=*(unsigned int *)&(header[12]);
	unsigned int linearSize=*(unsigned int *)&(header[16]);
	unsigned int mipMapCount=*(unsigned int *)&(header[24]);
	unsigned int fourCC=*(unsigned int *)&(header[80]);
	
	unsigned char * buffer;
	unsigned int bufsize;
	bufsize=mipMapCount>1?linearSize *2:linearSize;
	buffer=(unsigned char *)malloc(bufsize * sizeof(unsigned char));
	fread(buffer,1,bufsize,fp);
	fclose(fp);
	
	unsigned int components=(fourCC==FOURCC_DXT1)?3:4;
	unsigned int format;
	switch(fourCC){
		case FOURCC_DXT1:
			format=GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			break;
		case FOURCC_DXT3:
			format=GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			break;
		case FOURCC_DXT5:
			format=GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			break;
		default:
			free(buffer);
			return 0;
	}
	
	GLuint textureID;
	glGenTextures(1,&textureID);
	
	glBindTexture(GL_TEXTURE_2D,textureID);
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	
	unsigned int blockSize=(format==GL_COMPRESSED_RGBA_S3TC_DXT1_EXT)?8:16;
	unsigned int offset=0;
	
	for(unsigned int level=0;level<mipMapCount &&(width ||height);++level){
		unsigned int size = ((width+3)/4)*((height+3)/4)*blockSize;
		glCompressedTexImage2D(GL_TEXTURE_2D,level,format,width,height,0,size,buffer+offset);
		offset+=size;
		width/=2;
		height/=2;
		if(width<1){width=1;}
		if(height<1){height=1;}
	}
	
	free(buffer);
	return textureID;
}
*/