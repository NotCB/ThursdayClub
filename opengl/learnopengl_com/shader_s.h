#ifndef SHADER_H
#define SHADER_H

#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

#include <glad/glad.h>

#ifndef EOUT_H
#include "eout.h"
#endif

class Shader{
public:
	unsigned int ID;
	Shader(const char* vertexPath,const char* fragmentPath){
		//source code from filePath
		std::string vertexCode,fragmentCode;
		std::ifstream vShaderFile,fShaderFile;
		vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
		try{
			vShaderFile.open(vertexPath);
			fShaderFile.open(fragmentPath);
			std::stringstream vShaderStream,fShaderStream;
			vShaderStream<<vShaderFile.rdbuf();
			fShaderStream<<fShaderFile.rdbuf();
			vShaderFile.close();
			fShaderFile.close();
			vertexCode=vShaderStream.str();
			fragmentCode=fShaderStream.str();
		}catch(std::ifstream::failure& e){
			eout<<"ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: "<<e.what();'\n';
		}
		const char* vShaderCode=vertexCode.c_str();
		const char* fShaderCode=fragmentCode.c_str();
		//compile shaders
		unsigned int vertex,fragment;
		vertex=glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertex,1,&vShaderCode,NULL);
		glCompileShader(vertex);
		checkCompileErrors(vertex,"VERTEX");
		fragment=glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragment,1,&fShaderCode,NULL);
		glCompileShader(fragment);
		checkCompileErrors(fragment,"FRAGMENT");
		ID=glCreateProgram();
		glAttachShader(ID,vertex);
		glAttachShader(ID,fragment);
		glLinkProgram(ID);
		checkCompileErrors(ID,"PROGRAM");
		glDeleteShader(vertex);
		glDeleteShader(fragment);
	}
	void use(){
		glUseProgram(ID);
	}
	void setBool(const std::string &name,bool value)const{
		glUniform1i(glGetUniformLocation(ID,name.c_str()),(int)value);
	}
	void setInt(const std::string &name,int value)const{
		glUniform1i(glGetUniformLocation(ID,name.c_str()),value);
	}
	void setFloat(const std::string &name,float value)const{
		glUniform1f(glGetUniformLocation(ID,name.c_str()),value);
	}
private:
	void checkCompileErrors(unsigned int shader,std::string type){
		int success;
		char infoLog[1024];
		if(type=="PROGRAM"){
			glGetProgramiv(shader,GL_LINK_STATUS,&success);
			if(!success){
				glGetProgramInfoLog(shader,1024,NULL,infoLog);
				eout<<"ERROR::PROGRAM_LINKING_ERROR of type: "<<type<<'\n'<<infoLog<<'\n';
			}
		}else{
			glGetShaderiv(shader,GL_COMPILE_STATUS,&success);
			if(!success){
				glGetShaderInfoLog(shader,1024,NULL,infoLog);
				eout<<"ERROR::SHADER_COMPILATION_ERROR of type: "<<type<<'\n'<<infoLog<<'\n';
			}
		}
	}
};

#endif

