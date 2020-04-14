#include <iostream>
#include <sstream>
#include <string>
#include <fstream>
#include "MLGray.h"
using namespace std;

bool Param(string cmd, int& v) {
	int p = cmd.find(':');
	if (p < 0) { return false; }
	try {
		v = stoi(cmd.substr(p+1));
		return true;
	}
	catch (invalid_argument e) {
		return false;
	}
}

bool Param(string cmd, double& v) {
	int p = cmd.find(':');
	if (p < 0) { return false; }
	try {
		v = stod(cmd.substr(p + 1));
		return true;
	}
	catch (invalid_argument e) {
		cout << "Param stod exception" << endl;
		return false;
	}
}

bool Param3(string cmd, double& v1,double& v2,double &v3) {
	int p = cmd.find(':');
	if (p < 0) { return false; }
	try {
		istringstream s(cmd.substr(p + 1));
		string ps;
		for (int n=1; (n <= 3) && (getline(s, ps, ':')); n++) {
			if (n == 1) { v1 = stod(ps); }
			if (n == 2) { v2 = stod(ps); }
			if (n == 3) { 
				v3 = stod(ps);
				return true;
			}
		}
		return false;
	}
	catch (invalid_argument e) {
		cout << "Param3 stod exception" << endl;
		return false;
	}
}


bool ConvertToGray(string fileName, string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(),op.end(), ' '), op.end());
	double p1,p2,p3;
	if (op.find("GIMP") == 0) {
		if (Param(op, p1)) { cout << "param =" << p1 << endl; return img.SaturateGIMP(fileName, p1); }
		return img.SaturateGIMP(fileName); 
	}
	if (op.find("Qt") == 0) { 
		if (Param(op, p1)) { return img.SaturateQt(fileName, p1); }
		return img.SaturateQt(fileName);
	}
	if (op.find("Helmholtz") == 0) { 
		if (Param(op, p1)) { return img.Helmholtz(fileName, p1); }
		return img.Helmholtz(fileName);
	}
	if (op.find("Desaturate") == 0) { return img.Desaturate(fileName); }
	if (op.find("Value") == 0) { return img.Value(fileName); }
	if (op.find("Saturate") == 0) {
		cout << "Saturate" << endl;
		if (Param3(op, p1, p2, p3)) {
			cout << "p1=" << p1 << ", p2=" << p2 << ", p3=" << p3 << endl;
			return img.Saturate(fileName, p1, p2, p3); 
		}
		cout << "Param3 failed" << endl;
		return false;
	}

	return false;
}

bool Preprocess(string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(),op.end(), ' '),op.end());
	double p1;
	if (op.find("Laplace") == 0) {
		if (Param(op, p1)) { return img.LaplaceSharpen(p1); }
		return img.LaplaceSharpen();
	}
	if (op.find("Logistic") == 0) {
		if (Param(op, p1)) { return img.Logistic(p1); }
		return img.Logistic();
	}
	return false;
}

bool Halftoning(string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(), op.end(), ' '), op.end());
	int p1;
	if (op.find("FloydSteinberg") == 0) {
		if (Param(op, p1)) { return img.FloydSteinberg(p1); }
		return img.FloydSteinberg();
	}
	if (op.find("Jarvis") == 0) {
		if (Param(op, p1)) { return img.Jarvis(p1); }
		return img.Jarvis();
	}
	if (op.find("Ostromoukhov") == 0) {
		if (Param(op, p1)) { return img.Ostromoukhov(p1); }
		return img.Ostromoukhov();
	}
	if (op.find("Bayer44") == 0) {
		return img.Bayer44();
	}
	if (op.find("Bayer88") == 0) {
		return img.Bayer88();
	}
	return false;
}

bool Postprocess(string op, MLGray& img) {
	return false; // Postprocessing TBD
}


bool SaveImage(string field,MLGray &img) {
	if (field.empty()) { return false; }
	string fileName = "./result/" + field + ".jpg";
	img.SaveImage(fileName);
	return true;
}

int main(int argc, char* argv[])
{
	string cmdFile = "cmd";
	if (argc >= 1) { cmdFile = argv[1]; }
	cmdFile += ".csv";
	string line;
	ifstream myfile(cmdFile);
	string fileName;
	MLGray img;
	
	if (myfile.is_open())
	{
		for (int lineNr = 1;getline(myfile, line);lineNr++)
		{

			if ((lineNr==1)||(line.empty())) { continue; }
			cout << line << endl;
			istringstream s(line);
			string field;
			for (int n = 0;(n<=5)&&(getline(s, field, ','));n++) {
				if(n==0) {
					if(field.empty()) { 
						cout << "Missing input FileName" << endl;
						break;
					}
					fileName="./image/"+field+".jpg"; 
				}
				if (n == 1) {
					if (!ConvertToGray(fileName, field, img)) {
						cout << "Can not convert " << fileName << "with " << field << endl;
						break;
					}
				}
				if (n == 2) {
					Preprocess(field, img);
				}
				if (n == 3) {
					Halftoning(field, img);
				}
				if (n == 4) {
					Postprocess(field, img);
				}
				if (n == 5) {
					SaveImage(field,img);

				}
			}
		}
		myfile.close();
		return 0;
	}
	else {
		std::cout << "Can not open cmdfile " << cmdFile;
		return 1;
	}

}


