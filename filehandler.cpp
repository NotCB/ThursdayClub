#include <algorithm> //std::transform
#include <fstream>
#include <iostream>
#include <string>

//Global Variables
int errorcount=0;
std::string elfn="error.log";
std::fstream errorlog(elfn,std::ios::out|std::ios::trunc);

//Alltime.log
bool atflag=false; //Alltime Flag
void atfunc(std::string fn,char atc){
	std::fstream alltime("alltime.log",std::ios::out|std::ios::app);
	if(atc=='c'){alltime<<"Cleared/Created "<<fn<<".\n";}
	if(atc=='r'){alltime<<"Read "<<fn<<".\n";}
	if(atc=='w'){alltime<<"Write to "<<fn<<".\n";}
	alltime.close();
}

//Functions
void errorout(std::string fn,char errortype){
	std::string errorstring="";
	if(errortype=='c'){errorstring="";}
	if(errortype=='r'){errorstring="Unable to open ";}
	if(errortype=='w'){errorstring="Unable to write to ";}
	errorstring="Error! "+errorstring+fn+"!\n";
	std::cout<<errorstring;
	std::fstream errorlog(elfn,std::ios::out|std::ios::app);
	errorlog<<errorstring;
	errorlog.close();
	errorcount++;
}

int filecreate(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::out|std::ios::trunc);
	if(!fs.is_open()){
		errorout(fn,'c');
		return 1;
	}else{
		if(atflag){atfunc(fn,'c');}
	}
fs.close();return 0;}

int fileread(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::in);
	if(!fs.is_open()){
		errorout(fn,'r');
		return 1;
	}
	else{
		std::string line;
		while(getline(fs,line)){
			std::cout<<line<<'\n';
		}
		if(atflag){atfunc(fn,'r');}
	}
fs.close();return 0;}

int filewrite(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::out|std::ios::app);
	if(!fs.is_open()){
		errorout(fn,'w');
		return 1;
	}
	std::string line="init";
	while(line!=""){getline(std::cin,line);fs<<line<<'\n';}
	if(atflag){atfunc(fn,'w');}
fs.close();return 0;}

int filehandle(std::string fn){
	std::string ui;std::getline(std::cin,ui);
	std::transform(ui.begin(),ui.end(),ui.begin(),[](unsigned char c){return std::tolower(c);});//ui tolowercase
	if(ui=="c"||ui=="clear"||ui=="create"){return filecreate(fn);}
	else if(ui=="r"||ui=="read"){return fileread(fn);}
	else if(ui=="a"||ui=="w"||ui=="write"){return filewrite(fn);}
return 1;}

int main(int argc,char *argv[]){
	//Variables
	int argb=argc-1;
	std::string filenames[argc];
	
	//Filename(s)
	if(argc>=2){
		for(int i=0;i<argb;i++){filenames[i]=argv[i+1];}
	}else{
		while(filenames[0]==""){
			std::cout<<"Filename: ";
			std::getline(std::cin,filenames[0]);
			if(filenames[0]==""){std::cout<<"No file name, try again.\n";}
		}argb++;
	}
	
	std::cout<<"Clear/create file(C)\n";
	std::cout<<"Read file(R)\n";
	std::cout<<"Write line(W)\n";
	
	//Handle Files
	for(int i=0;i<argb;i++){
		std::cout<<"What would you like to do with "<<filenames[i]<<"?\n";
		filehandle(filenames[i]);
	}
	
	//Error Counting
	if(errorcount>=1){
		std::fstream errorlog(elfn,std::ios::out|std::ios::app);
		if(errorcount==1){
			std::cout<<"There was "<<errorcount<<" error!\n";
			errorlog<<"There was "<<errorcount<<" error!\n";
		}else{
			std::cout<<"There was "<<errorcount<<" errors!\n";
			errorlog<<"There was "<<errorcount<<" errors!\n";
		}
		errorlog.close();
		return 1;
	}
	
	//Cleanup
	return 0;
}