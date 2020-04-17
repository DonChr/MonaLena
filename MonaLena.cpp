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

bool Param3(string cmd, int& v1, int& v2, int& v3) {
	int p = cmd.find(':');
	if (p < 0) { return false; }
	try {
		istringstream s(cmd.substr(p + 1));
		string ps;
		for (int n = 1; (n <= 3) && (getline(s, ps, ':')); n++) {
			if (n == 1) { v1 = stoi(ps); }
			if (n == 2) { v2 = stoi(ps); }
			if (n == 3) {
				v3 = stoi(ps);
				return true;
			}
		}
		return false;
	}
	catch (invalid_argument e) {
		cout << "Param3 stoi exception" << endl;
		return false;
	}
}


bool Param2(string cmd, double& v1, double& v2) {
	int p = cmd.find(':');
	if (p < 0) { return false; }
	try {
		istringstream s(cmd.substr(p + 1));
		string ps;
		for (int n = 1; (n <= 2) && (getline(s, ps, ':')); n++) {
			if (n == 1) { v1 = stod(ps); }
			if (n == 2) { 
				v2 = stod(ps); 
				return true;
			}
		}
		return false;
	}
	catch (invalid_argument e) {
		cout << "Param2 stod exception" << endl;
		return false;
	}
}

bool Param2(string cmd, int& v1, int& v2) {
	int p = cmd.find(':');
	if (p < 0) { return false; }
	try {
		istringstream s(cmd.substr(p + 1));
		string ps;
		for (int n = 1; (n <= 2) && (getline(s, ps, ':')); n++) {
			if (n == 1) { v1 = stoi(ps); }
			if (n == 2) {
				v2 = stoi(ps);
				return true;
			}
		}
		return false;
	}
	catch (invalid_argument e) {
		cout << "Param2 stoi exception" << endl;
		return false;
	}
}




bool ConvertToGray(string fileName, string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(),op.end(), ' '), op.end());
	double p1,p2,p3;
	if (op.find("ColorChannel") == 0) {
		if (Param(op, p1)) { cout << "param =" << p1 << endl; return img.ColorChannel(fileName, p1); }
		return img.ColorChannel(fileName);
	}
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
	cout << "Unknown Grayconverter operation " << op << endl;
	return false;
}

bool Preprocess(string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(),op.end(), ' '),op.end());
	double p1;
	if (op.find("Gauss5") == 0) {
		return img.Gauss55Filter();
	}
	if (op.find("Gauss7") == 0) {
		return img.Gauss77Filter();
	}
	if (op.find("Laplace") == 0) {
		if (Param(op, p1)) { return img.LaplaceSharpen(p1); }
		return img.LaplaceSharpen();
	}
	if (op.find("Edge") == 0) {
		if (Param(op, p1)) { return img.KnuthEdge(p1); }
		return img.KnuthEdge();
	}
	if (op.find("MedLaplace") == 0) {
		if (Param(op, p1)) { return img.Med5Laplace(p1); }
		return img.Med5Laplace();
	}
	if (op.find("Logistic") == 0) {
		if (Param(op, p1)) { return img.Logistic(p1); }
		return img.Logistic();
	}
	if (op.find("Rescale") == 0) {
		double p2;
		if (Param2(op, p1,p2)) { return img.Rescale(p1,p2); }
		return img.Rescale();
	}
	if (op.find("Median") == 0) {
		return img.MedianFilter9();
	}
	cout << "WARNING: Unknown Preprocessing operation " << op << endl;
	return false;
}

bool Halftoning(string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(), op.end(), ' '), op.end());
	int p1,p2;
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
	if (op.find("OptOstromoukhov") == 0) {
		if (Param2(op, p1, p2)) { return (img.OptOstromoukhov(p1, p2)>=0); }
		return (img.OptOstromoukhov()>=0);
	}
	if (op.find("OptFloydSteinberg") == 0) {
		if (Param2(op, p1, p2)) { return (img.OptFloydSteinberg(p1, p2) >= 0); }
		return (img.OptFloydSteinberg() >= 0);
	}
	if (op.find("OptJarvis") == 0) {
		if (Param2(op, p1, p2)) { return (img.OptJarvis(p1, p2) >= 0); }
		return (img.OptJarvis() >= 0);
	}

	if (op.find("Bayer44") == 0) {
		return img.Bayer44();
	}
	if (op.find("Bayer88") == 0) {
		return img.Bayer88();
	}
	if (op.find("BayerRnd88") == 0) {
		if (Param(op, p1)) { return img.BayerRnd88(p1); }
		return img.BayerRnd88();
	}
	if (op.find("Random") == 0) {
		return img.Random();
	}
	if (op.find("Threshold") == 0) {
		if (Param(op, p1)) { return img.Threshold(p1); }
		return img.Threshold();
	}
	cout << "WARNING: Unknown Halftoning operation " << op << endl;
	return false;
}

bool Postprocess(string op, MLGray& img) {
	if (op.empty()) { return false; }
	op.erase(remove(op.begin(), op.end(), ' '), op.end());
	int p1;
	if (op.find("SaltPepper") == 0) {
		if (Param(op, p1)) { return img.SaltPepper(p1); }
		return img.SaltPepper();
	}
	if (op.find("Gauss5") == 0) {
		return img.Gauss55Filter();
	}
	if (op.find("Gauss7") == 0) {
		return img.Gauss77Filter();
	}
	cout << "WARNING: Unknown Postprocessing operation " << op << endl;
	return false; 
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

			if (line.empty()) { continue; }
			if (line[0] == '#') { continue; } // Comment Line
			cout << line << endl;
			istringstream s(line);
			string field;
			for (int n = 0;(n<=5)&&(getline(s, field, ','));n++) {
				if(n==0) {
					if(field.empty()) { 
						cout << "Line "<<lineNr<<": Missing input FileName" << endl;
						break;
					}
					fileName="./image/"+field+".jpg"; 
				}
				if (n == 1) {
					if (!ConvertToGray(fileName, field, img)) {
						cout << "Line " << lineNr << ": Can not convert " << fileName << "with " << field << endl;
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


