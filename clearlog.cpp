#include <fstream>

int main(int argc,char *argv[]){
	std::fstream a("alltime.log",std::ios::out|std::ios::trunc);a.close();
	std::fstream e("error.log",std::ios::out|std::ios::trunc);e.close();
return 0;}