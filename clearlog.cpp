#include <fstream>

int main(){
	std::fstream a("alltime.log",std::ios::out|std::ios::trunc);
	a.close();
	std::fstream e("error.log",std::ios::out|std::ios::trunc);
	e.close();
return 0;}