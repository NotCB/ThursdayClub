#include <cstring>
#include <fstream>
#include <map>
#include <sstream>
#include <vector>

#include <GL/glew.h> //Always include glew.h before gl.h and glfw3.h
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

GLFWwindow* window;
int windowWidth,windowHeight;

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path);

GLuint loadBMP_custom(const char * imagepath);
//"DXT1" "DXT3" "DXT5"
#define FOURCC_DXT1 0x31545844
#define FOURCC_DXT3 0x33545844
#define FOURCC_DXT5 0x35545844
GLuint loadDDS(const char * imagepath);

void computeMatricesFromInputs();
glm::mat4 ViewMatrix;
glm::mat4 getViewMatrix(){return ViewMatrix;}
glm::mat4 ProjectionMatrix;
glm::mat4 getProjectionMatrix(){return ProjectionMatrix;}

bool loadOBJ(
	const char *path,
	std::vector<glm::vec3>&out_vertices,
	std::vector<glm::vec2>&out_uvs,
	std::vector<glm::vec3>&out_normals
);

struct PackedVertex{
	glm::vec3 position;
	glm::vec2 uv;
	glm::vec3 normal;
	bool operator<(const PackedVertex that) const{
		return memcmp((void*)this, (void*)&that, sizeof(PackedVertex))>0;
	};
};

void indexVBO(
	std::vector<glm::vec3>&in_vertices,
	std::vector<glm::vec2>&in_uvs,
	std::vector<glm::vec3>&in_normals,
	std::vector<unsigned short>&out_indices,
	std::vector<glm::vec3>&out_vertices,
	std::vector<glm::vec2>&out_uvs,
	std::vector<glm::vec3>&out_normals
);

glm::vec3 position=glm::vec3(0,0,2);
float horizontalAngle=180.0f/180.0f*3.14f;
float verticalAngle=0.0f/180.0f*3.14f;
float initialFoV=80.0f;//60-100
float speed=4.0f;
float mouseSensitivity=0.003f;

int main(){
	if(!glfwInit()){
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<"Failed to initialize GLFW\n";
		eout.close();
		return -1;
	}
	
	glfwWindowHint(GLFW_SAMPLES,0);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR,3);//OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR,3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT,GL_TRUE);//MacOS Happy
	glfwWindowHint(GLFW_OPENGL_PROFILE,GLFW_OPENGL_CORE_PROFILE);
	
	window=glfwCreateWindow(640,480,"Tutorial",NULL,NULL);
	if(window==NULL){
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<"Failed to open GLFW window.";
		eout.close();
		glfwTerminate();
		return -1;
	}
	glfwMakeContextCurrent(window);
	
	glewExperimental=true;
	if(glewInit()!=GLEW_OK){
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<"Failed to initialize GLEW\n";
		eout.close();
		glfwTerminate();
		return -1;
	}
	
	glfwGetWindowSize(window,&windowWidth,&windowHeight);
	glfwSetInputMode(window,GLFW_STICKY_KEYS,GL_TRUE);
	glfwSetInputMode(window,GLFW_CURSOR,GLFW_CURSOR_DISABLED);
	glfwPollEvents();
	glfwSetCursorPos(window,windowWidth /2,windowHeight /2);
	
	glClearColor(0.0f,0.0f,0.0f,0.0f);
	
	GLuint VertexArrayID;
	glGenVertexArrays(1,&VertexArrayID);
	glBindVertexArray(VertexArrayID);
	
	GLuint programID=LoadShaders("vertexshader.glsl","fragmentshader.glsl");
	GLuint MatrixID=glGetUniformLocation(programID,"MVP");
	GLuint ModelMatrixID=glGetUniformLocation(programID,"M");
	GLuint ViewMatrixID=glGetUniformLocation(programID,"V");
	
	GLuint Texture=loadBMP_custom("uvtemplate.bmp");
	GLuint textureID=glGetUniformLocation(programID,"myTextureSampler");
	
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	bool res=loadOBJ("suzanne.obj",vertices,uvs,normals);
	
	std::vector<unsigned short>indices;
	std::vector<glm::vec3>indexed_vertices;
	std::vector<glm::vec2>indexed_uvs;
	std::vector<glm::vec3>indexed_normals;
	indexVBO(vertices,uvs,normals,indices,indexed_vertices,indexed_uvs,indexed_normals);
	
	GLuint vertexbuffer;
	glGenBuffers(1,&vertexbuffer);
	glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
	glBufferData(GL_ARRAY_BUFFER,indexed_vertices.size()*sizeof(glm::vec3),&indexed_vertices[0],GL_STATIC_DRAW);
	
	GLuint uvbuffer;
	glGenBuffers(1,&uvbuffer);
	glBindBuffer(GL_ARRAY_BUFFER,uvbuffer);
	glBufferData(GL_ARRAY_BUFFER,indexed_uvs.size()*sizeof(glm::vec2),&indexed_uvs[0],GL_STATIC_DRAW);
	
	GLuint normalbuffer;
	glGenBuffers(1,&normalbuffer);
	glBindBuffer(GL_ARRAY_BUFFER,normalbuffer);
	glBufferData(GL_ARRAY_BUFFER,indexed_normals.size()*sizeof(glm::vec3),&indexed_normals[0],GL_STATIC_DRAW);
	
	GLuint elementbuffer;
	glGenBuffers(1,&elementbuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementbuffer);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,indices.size()*sizeof(unsigned short),&indices[0],GL_STATIC_DRAW);
	
	glUseProgram(programID);
	GLuint LightID=glGetUniformLocation(programID,"LightPosition_worldspace");
	
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	
	double lastTime=glfwGetTime();
	int nbFrames=0;
	
	do{
		double currentTime=glfwGetTime();
		nbFrames++;
		if(currentTime-lastTime>=1.0){
			printf("%f ms/frame\n",1000.0/double(nbFrames));
			nbFrames=0;
			lastTime+=1.0;
		}
		
		glClear(GL_COLOR_BUFFER_BIT |GL_DEPTH_BUFFER_BIT);
		
		glUseProgram(programID);
		
		computeMatricesFromInputs();
		glm::mat4 ProjectionMatrix=getProjectionMatrix();
		glm::mat4 ViewMatrix=getViewMatrix();
		glm::mat4 ModelMatrix=glm::mat4(1.0);
		glm::mat4 MVP=ProjectionMatrix *ViewMatrix *ModelMatrix;
		
		glUniformMatrix4fv(MatrixID,1,GL_FALSE,&MVP[0][0]);
		glUniformMatrix4fv(ModelMatrixID,1,GL_FALSE,&ModelMatrix[0][0]);
		glUniformMatrix4fv(ViewMatrixID,1,GL_FALSE,&ViewMatrix[0][0]);
		
		glm::vec3 lightPos=glm::vec3(4,4,4);
		glUniform3f(LightID,lightPos.x,lightPos.y,lightPos.z);
		
		//Array 0: Vertices
		glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER,vertexbuffer);
		glVertexAttribPointer(
			0,			//attribute
			3,			//size
			GL_FLOAT,	//type
			GL_FALSE,	//normalized
			0,			//stride
			(void *)0	//array buffer offset
		);
		
		//Array 1: UVs
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER,uvbuffer);
		glVertexAttribPointer(1,2,GL_FLOAT,GL_FALSE,0,(void *)0);
		
		//Array 2: normals
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER,normalbuffer);
		glVertexAttribPointer(2,3,GL_FLOAT,GL_FALSE,0,(void*)0);
		
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER,elementbuffer);
		
		glDrawElements(GL_TRIANGLES,indices.size(),GL_UNSIGNED_SHORT,(void*)0);
		
		//Disable Arrays
		glDisableVertexAttribArray(0);
		glDisableVertexAttribArray(1);
		glDisableVertexAttribArray(2);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
	}while(glfwGetKey(window,GLFW_KEY_ESCAPE)!=GLFW_PRESS&&glfwWindowShouldClose(window)==0);
	
	glDeleteBuffers(1,&vertexbuffer);
	glDeleteBuffers(1,&uvbuffer);
	glDeleteBuffers(1,&normalbuffer);
	glDeleteBuffers(1,&elementbuffer);
	glDeleteProgram(programID);
	glDeleteTextures(1,&Texture);
	glDeleteVertexArrays(1,&VertexArrayID);
	
	glfwTerminate();
	
	return 0;
}

GLuint LoadShaders(const char * vertex_file_path,const char * fragment_file_path){
	GLuint VertexShaderID=glCreateShader(GL_VERTEX_SHADER);
	GLuint FragmentShaderID=glCreateShader(GL_FRAGMENT_SHADER);
	
	std::string VertexShaderCode;
	std::ifstream VertexShaderStream(vertex_file_path,std::ios::in);
	if(VertexShaderStream.is_open()){
		std::stringstream sstr;
		sstr<<VertexShaderStream.rdbuf();
		VertexShaderCode=sstr.str();
		VertexShaderStream.close();
	}else{
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<"Impossible to open "<<vertex_file_path<<"\n";
		eout.close();
		return 0;
	}
	
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
	
	char const * VertexSourcePointer=VertexShaderCode.c_str();
	glShaderSource(VertexShaderID,1,&VertexSourcePointer,NULL);
	glCompileShader(VertexShaderID);
	
	glGetShaderiv(VertexShaderID,GL_COMPILE_STATUS,&Result);
	glGetShaderiv(VertexShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> VertexShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(VertexShaderID,InfoLogLength,NULL,&VertexShaderErrorMessage[0]);
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<&VertexShaderErrorMessage[0]<<"\n";
		eout.close();
	}
	
	char const * FragmentSourcePointer=FragmentShaderCode.c_str();
	glShaderSource(FragmentShaderID,1,&FragmentSourcePointer,NULL);
	glCompileShader(FragmentShaderID);
	
	glGetShaderiv(FragmentShaderID,GL_COMPILE_STATUS,&Result);
	glGetShaderiv(FragmentShaderID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> FragmentShaderErrorMessage(InfoLogLength+1);
		glGetShaderInfoLog(FragmentShaderID,InfoLogLength,NULL,&FragmentShaderErrorMessage[0]);
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<&FragmentShaderErrorMessage[0]<<"\n";
		eout.close();
	}
	
	GLuint ProgramID=glCreateProgram();
	glAttachShader(ProgramID,VertexShaderID);
	glAttachShader(ProgramID,FragmentShaderID);
	glLinkProgram(ProgramID);
	
	glGetProgramiv(ProgramID,GL_LINK_STATUS,&Result);
	glGetProgramiv(ProgramID,GL_INFO_LOG_LENGTH,&InfoLogLength);
	if(InfoLogLength>0){
		std::vector<char> ProgramErrorMessage(InfoLogLength+1);
		glGetProgramInfoLog(ProgramID,InfoLogLength,NULL,&ProgramErrorMessage[0]);
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<&ProgramErrorMessage[0]<<"\n";
		eout.close();
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
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<imagepath<<" could not be opened.";
		eout.close();
		return 0;
	}
	
	std::fstream eout("error.log",std::ios::out | std::ios::app);
	if(fread(header,1,54,file)!=54){eout<<"Not a correct BMP file \n";fclose(file);return 0;}
	if(header[0]!='B'||header[1]!='M'){eout<<"Not a correct BMP file \n";fclose(file);return 0;}
	if(*(int *)&(header[0x1E])!=0){eout<<"Not a correct BMP file \n";fclose(file);return 0;}
	if(*(int *)&(header[0x1C])!=24){eout<<"Not a correct BMP file \n";fclose(file);return 0;}
	
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

GLuint loadDDS(const char * imagepath){
	unsigned char header[124];
	FILE *fp;
	
	fp=fopen(imagepath,"rb");
	if(fp==NULL){
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<imagepath<<" could not be opened.";
		eout.close();
		return 0;
	}
	
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
		width /=2;
		height /=2;
		if(width<1){width=1;}
		if(height<1){height=1;}
	}
	
	free(buffer);
	return textureID;
}

void computeMatricesFromInputs(){
	static double lastTime=glfwGetTime();
	double currentTime=glfwGetTime();
	float deltaTime=float(currentTime -lastTime);
	
	double xpos,ypos;
	glfwGetCursorPos(window,&xpos,&ypos);
	glfwSetCursorPos(window,windowWidth /2,windowHeight /2);
	horizontalAngle+=mouseSensitivity*float(windowWidth /2-xpos);
	verticalAngle+=mouseSensitivity*float(windowHeight /2-ypos);
	
	glm::vec3 direction(
		cos(verticalAngle)*sin(horizontalAngle),
		sin(verticalAngle),
		cos(verticalAngle)*cos(horizontalAngle)
	);
	
	glm::vec3 right=glm::vec3(
		sin(horizontalAngle-3.14f /2.0f),
		0,
		cos(horizontalAngle-3.14f /2.0f)
	);
	
	glm::vec3 up=glm::cross(right,direction);
	
	if(glfwGetKey(window,GLFW_KEY_UP)==GLFW_PRESS){position+=direction *deltaTime *speed;}
	if(glfwGetKey(window,GLFW_KEY_DOWN)==GLFW_PRESS){position-=direction *deltaTime *speed;}
	if(glfwGetKey(window,GLFW_KEY_RIGHT)==GLFW_PRESS){position+=right *deltaTime *speed;}
	if(glfwGetKey(window,GLFW_KEY_LEFT)==GLFW_PRESS){position-=right *deltaTime *speed;}
	
	float FoV=initialFoV;
	
	ProjectionMatrix=glm::perspective(glm::radians(FoV),4.0f/3.0f,0.1f,100.0f);
	ViewMatrix=glm::lookAt(position,position+direction,up);
	
	lastTime=currentTime;
}

bool loadOBJ(
	const char *path,
	std::vector<glm::vec3>&out_vertices,
	std::vector<glm::vec2>&out_uvs,
	std::vector<glm::vec3>&out_normals
){
	std::vector<unsigned int>vertexIndices,uvIndices,normalIndices;
	std::vector<glm::vec3>temp_vertices;
	std::vector<glm::vec2>temp_uvs;
	std::vector<glm::vec3>temp_normals;
	
	FILE *file=fopen(path,"r");
	if(file==NULL){
		std::fstream eout("error.log",std::ios::out | std::ios::app);
		eout<<"Impossible to open the file!\n";
		eout.close();
		return false;
	}
	
	while(1){
		char lineHeader[128];
		int res=fscanf(file,"%s",lineHeader);
		if(res==EOF){break;}
		
		if(strcmp(lineHeader,"v")==0){
			glm::vec3 vertex;
			fscanf(file,"%f %f %f\n",&vertex.x,&vertex.y,&vertex.z);
			temp_vertices.push_back(vertex);
		}else if(strcmp(lineHeader,"vt")==0){
			glm::vec2 uv;
			fscanf(file,"%f %f\n",&uv.x,&uv.y);
			//uv.y=-uv.y;//Invert V coordinate for DDS texture
			temp_uvs.push_back(uv);
		}else if(strcmp(lineHeader,"vn")==0){
			glm::vec3 normal;
			fscanf(file,"%f %f %f\n",&normal.x,&normal.y,&normal.z);
			temp_normals.push_back(normal);
		}else if(strcmp(lineHeader,"f")==0){
			std::string vertex1,vertex2,vertex3;
			unsigned int vI[3],uvI[3],nI[3];
			int matches=fscanf(file,"%d/%d/%d %d/%d/%d %d/%d/%d\n",&vI[0],&uvI[0],&nI[0],&vI[1],&uvI[1],&nI[1],&vI[2],&uvI[2],&nI[2]);
			if(matches!=9){
				std::fstream eout("error.log",std::ios::out | std::ios::app);
				eout<<"File cannot be read by simple parser\n";
				eout.close();
				fclose(file);
				return false;
			}
			vertexIndices.push_back(vI[0]);
			vertexIndices.push_back(vI[1]);
			vertexIndices.push_back(vI[2]);
			uvIndices.push_back(uvI[0]);
			uvIndices.push_back(uvI[1]);
			uvIndices.push_back(uvI[2]);
			normalIndices.push_back(nI[0]);
			normalIndices.push_back(nI[1]);
			normalIndices.push_back(nI[2]);
		}else{
			char stupidBuffer[1024];
			fgets(stupidBuffer,1024,file);
		}
	}
	
	for(unsigned int i=0;i<vertexIndices.size();i++){
		unsigned int vertexIndex=vertexIndices[i];
		unsigned int uvIndex=uvIndices[i];
		unsigned int normalIndex=normalIndices[i];
		
		glm::vec3 vertex=temp_vertices[vertexIndex-1];
		glm::vec2 uv=temp_uvs[uvIndex-1];
		glm::vec3 normal=temp_normals[normalIndex-1];
		
		out_vertices.push_back(vertex);
		out_uvs.push_back(uv);
		out_normals.push_back(normal);
	}
	
	fclose(file);
	return true;
}

bool getSimilarVertexIndex_fast( 
	PackedVertex & packed, 
	std::map<PackedVertex,unsigned short> & VertexToOutIndex,
	unsigned short & result
){
	std::map<PackedVertex,unsigned short>::iterator it = VertexToOutIndex.find(packed);
	if ( it == VertexToOutIndex.end() ){
		return false;
	}else{
		result = it->second;
		return true;
	}
}

void indexVBO(
	std::vector<glm::vec3>&in_vertices,
	std::vector<glm::vec2>&in_uvs,
	std::vector<glm::vec3>&in_normals,
	std::vector<unsigned short>&out_indices,
	std::vector<glm::vec3>&out_vertices,
	std::vector<glm::vec2>&out_uvs,
	std::vector<glm::vec3>&out_normals
){
	std::map<PackedVertex,unsigned short>VertexToOutIndex;
	
	for(unsigned int i=0;i<in_vertices.size();i++){
		PackedVertex packed={in_vertices[i],in_uvs[i],in_normals[i]};
		
		unsigned short index;
		bool found=getSimilarVertexIndex_fast(packed,VertexToOutIndex,index);
		if(found){
			out_indices.push_back(index);
		}else{
			out_vertices.push_back(in_vertices[i]);
			out_uvs.push_back(in_uvs[i]);
			out_normals.push_back(in_normals[i]);
			unsigned short newindex=(unsigned short)out_vertices.size()-1;
			out_indices.push_back(newindex);
			VertexToOutIndex[packed]=newindex;
		}
	}
}