#include <algorithm> //std::transform
#include <fstream>
#include <iostream>
#include <string>

//Global Variables
int errorcount=0;
std::ofstream errorlog("error.log");
std::fstream alltime("alltime.log",std::ios::out|std::ios::app);

//Functions
int createfile(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::out|std::ios::trunc);
	if(!fs.is_open()){
		std::cout<<"Error!\n";
		errorlog<<"Error!\n";errorcount++;
		return 1;
	}else{alltime<<"Cleared/Created "<<fn<<".\n";}
fs.close();return 0;}

int readfile(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::in);
	if(!fs.is_open()){
		std::cout<<"Error! Unable to open "<<fn<<"!\n";
		errorlog<<"Error! Unable to open "<<fn<<"!\n";errorcount++;
		return 1;
	}
	else{
		std::string line;
		while(getline(fs,line)){
			std::cout<<line<<'\n';
		}
		alltime<<"Read "<<fn<<".\n";
	}
fs.close();return 0;}

int writefile(std::string fn){
	std::fstream fs;
	fs.open(fn,std::ios::out|std::ios::app);
	if(!fs.is_open()){
		std::cout<<"Error! Unable to write to "<<fn<<"!\n";
		errorlog<<"Error! Unable to write to "<<fn<<"!\n";errorcount++;
		return 1;
	}
	std::string line="init";
	while(line!=""){getline(std::cin,line);fs<<line<<'\n';}
	alltime<<"Write to "<<fn<<".\n";
fs.close();return 0;}

int filehandler(std::string fn,std::string ui){
	std::transform(ui.begin(),ui.end(),ui.begin(),[](unsigned char c){return std::tolower(c);});//ui tolowercase
	if (ui=="a"||ui=="w"||ui=="write"){return writefile(fn);}
	else if(ui=="c"||ui=="clear"||ui=="create"){return createfile(fn);}
	else if(ui=="r"||ui=="read"){return readfile(fn);}
return 1;}

int main(int argc, char *argv[]){
	//Variables
	int argb=argc-1;
	std::string filenames[argc];
	std::string userinput;
	
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
		std::getline(std::cin,userinput);
		filehandler(filenames[i],userinput);
	}
	
	//Error Counting
	if(errorcount>=1){
		if(errorcount==1){
			std::cout<<"There was "<<errorcount<<" error!\n";
			errorlog<<"There was "<<errorcount<<" error!\n";
		}else{
			std::cout<<"There was "<<errorcount<<" errors!\n";
			errorlog<<"There was "<<errorcount<<" errors!\n";
		}
		return 1;
	}
	
	//Cleanup
	errorlog.close();
	alltime.close();
	return 0;
}