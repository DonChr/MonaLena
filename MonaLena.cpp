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
		gimp.FloydSteinberg(128);
		gimp.SaveImage(fname, "GmpFloyd128");
		std::cout << "Floyd done" << std::endl;
	}
	MLGray ostrm;
	if (ostrm.SaturateGIMP(fileName)) {
		ostrm.LaplaceSharpen();
		ostrm.Ostromoukhov(128);
		ostrm.SaveImage(fname, "GmpOstrm128");
		std::cout << "Ostrom done" << std::endl;
	}
	MLGray bayer44;
	if (bayer44.SaturateGIMP(fileName)) {
		bayer44.Bayer44();
		bayer44.SaveImage(fname, "GmpBayer44");
		std::cout << "Bayer44 done" << std::endl;
	}
	MLGray bayer88;
	if (bayer88.SaturateGIMP(fileName)) {
		bayer88.Bayer88();
		bayer88.SaveImage(fname, "GmpBayer88");
		std::cout << "Bayer88 done" << std::endl;
	}

	/*
	MLGray linearB2w;
	linearB2w.LinearGradient();
	linearB2w.SaveImage("./image/LinearB2W.jpg");
	MLGray linearW2b;
	linearW2b.LinearGradient(false);
	linearW2b.SaveImage("./image/LinearW2B.jpg");
	MLGray radialB2w;
	radialB2w.RadialGradient();
	radialB2w.SaveImage("./image/RadialB2W.jpg");
	MLGray radialW2b;
	radialW2b.RadialGradient(false);
	radialW2b.SaveImage("./image/RadialW2B.jpg");
	*/

	std::cout << "MonaLena done" << std::endl;

	return 0;
}


