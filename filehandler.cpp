#include <algorithm> //std::transform
#include <fstream>
#include <iostream>
#include <string>

int createfile(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::out|std::ios::trunc);
	if(!fs.is_open()){std::cout<<"Error!\n";return 1;}
fs.close();return 0;}

int readfile(std::string fn){
	std::string line;
	std::fstream fs;
	fs.open(fn,std::ios::in);
	if(!fs.is_open()){std::cout<<"Error! Unable to open "<<fn<<"!\n";return 1;}
	else{
		while(getline(fs,line)){
			std::cout<<line<<'\n';
		}
	}
fs.close();return 0;}

int writefile(std::string fn){
	std::string line="init";
	std::fstream fs;
	fs.open(fn,std::ios::out|std::ios::app);
	if(!fs.is_open()){std::cout<<"Error! Unable to write to "<<fn<<"!\n";return 1;}
	while(line!=""){getline(std::cin,line);fs<<line<<'\n';}
fs.close();return 0;}

int filehandler(std::string fn,std::string ui){
	std::transform(ui.begin(),ui.end(),ui.begin(),[](unsigned char c){return std::tolower(c);});//ui tolowercase
	if (ui=="a"||ui=="w"||ui=="write"){
		return writefile(fn);
	}
	else if(ui=="c"||ui=="clear"||ui=="create"){
		return createfile(fn);
	}
	else if(ui=="r"||ui=="read"){
		return readfile(fn);
	}
	else{return 1;}
return 1;}

int main(int argc, char *argv[]){
	//Variables
	std::string filenames[argc];
	std::string userinput;
	
	//Filenames
	if(argc>=2){
		for(int i=0;i<argc-1;i++){filenames[i]=argv[i+1];}//filename=argv[1];
	}else{
		while(filenames[0]==""){
			std::cout<<"Filename: ";
			std::getline(std::cin,filenames[0]);
			if(filenames[0]==""){std::cout<<"No file name, try again.\n";}
		}argc=argc+1;
	}
	
	std::cout<<"Clear/create file(C)\n";
	std::cout<<"Read file(R)\n";
	std::cout<<"Write line(W)\n";
	
	for(int i=0;i<argc-1;i++){
		std::cout<<"What would you like to do with "<<filenames[i]<<"?\n";
		std::getline(std::cin,userinput);
		filehandler(filenames[i],userinput);
	}
return 0;}