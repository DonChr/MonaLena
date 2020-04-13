#include <iostream>
#include <string>
#include "MLGray.h"


int main(int argc, char* argv[])
{
	string fname = "Lena";
	if (argc >= 1) { fname = argv[1]; }
	string fileName = "./image/" + fname + ".jpg";
	MLGray gimp;
	if (gimp.SaturateGIMP(fileName)) {
		gimp.SaveImage(fname, "Gimp");
		std::cout << "Gimp Done" << std::endl;
		return 0;
	}
	std::cout << fileName << " Gimp failed" << std::endl;
	return 1;
}


