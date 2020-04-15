/***********************************************************************
*
* Copyright (c) 2020 Dr. Chrilly Donninger
*
* This file is part of CMonaLisa
*
***********************************************************************/
#include "MLGray.h"
#include "math.h"
#include <iostream>
#include <random>
#include <algorithm>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"


#pragma warning(disable : 26451)

MLGray::MLGray() {
	width = 0;
	height = 0;
	data = nullptr;
}

MLGray::MLGray(int w, int h) {
	width = w;
	height = h;
	data = new int32_t[width * height];
}

MLGray::MLGray(int w,int h,int32_t *srcdata) {
	width = w;
	height = h;
	int sz = width * height;
	data = new int32_t[sz];
	for (int i = 0; i < sz; i++) {
		data[i] = srcdata[i];
	}
}

MLGray::~MLGray() {
	delete[] data;
}

unsigned char* MLGray::LoadImage(string fileName, int& width, int& height, int& channels) {
	return stbi_load(fileName.c_str(),&width,&height,&channels,0);
}

bool MLGray::CopyData(const unsigned char* d) {
	int sz = width * height;
	for (int n = 0; n < sz; n++) {
		data[n] = (int32_t)d[n];
	}
	return true;
}

bool MLGray::CreateImage(int w, int h) {
	width = w;
	height = h;
	if (data != nullptr) { delete[] data; }
	data = new int32_t[width * height];
	if (data == nullptr) { return false; }
	return true;
}


bool MLGray::Saturate(string fileName, double wRed, double wGreen, double wBlue) {
	int ch,w,h;
	unsigned char* d = LoadImage(fileName, w, h, ch);
	if (d == nullptr) { return false; }
	CreateImage(w, h);
	if (ch == 1) { return CopyData(d); }
	int32_t r, g, b;
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int lpStb = lpos * ch;
		for (int x = 0; x < width; x++) {
			int px = lpos + x;
			int pxStb = lpStb + x * ch;
			r = d[pxStb];
			g = d[pxStb + 1];
			b = d[pxStb + 2];
			data[px] = (int32_t)(wRed * r + wGreen * g + wBlue * b + 0.5); // Last Term is Rounding
		}
	}
	stbi_image_free(d);
	return true;
}

bool MLGray::SaturateGIMP(const string fileName,double scaleFactor) {
	return Saturate(fileName, 0.3*scaleFactor,0.596*scaleFactor,0.11*scaleFactor);
}

bool MLGray::SaturateQt(const string fileName,double scaleFac) {
	return Saturate(fileName, 0.34375*scaleFac, 0.5 * scaleFac, 0.1625 * scaleFac);
}

bool MLGray::Desaturate(const string fileName) {
	int ch;
	unsigned char* d = LoadImage(fileName, width, height, ch);
	if (d == nullptr) { return false; }
	data = new int32_t[width * height];
	if (ch == 1) { return CopyData(d); }
	int32_t g, b;
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int lpStb = lpos * ch;
		for (int x = 0; x < width; x++) {
			int px = lpos + x;
			int pxStb = lpStb + x * ch;
			int32_t maxI = d[pxStb];
			int32_t minI = maxI;
			g = d[pxStb + 1];
			b = d[pxStb + 2];
			maxI = (g > maxI) ? g : maxI;
			minI = (g < minI) ? g : minI;
			maxI = (b > maxI) ? b : maxI;
			minI = (b < minI) ? b : minI;
			data[px] = (int32_t)((maxI + minI) * 0.5 + 0.5); // Last Term is Rounding
		}
	}
	return true;
}

bool MLGray::Value(const string fileName) {
	int ch;
	unsigned char* d = LoadImage(fileName, width, height, ch);
	if (d == nullptr) { return false; }
	data = new int32_t[width * height];
	if (ch == 1) { return CopyData(d); }
	int32_t g, b;
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int lpStb = lpos * ch;
		for (int x = 0; x < width; x++) {
			int px = lpos + x;
			int pxStb = lpStb + x * ch;
			int32_t maxI = d[pxStb];
			g = d[pxStb + 1];
			b = d[pxStb + 2];
			maxI = (g > maxI) ? g : maxI;
			maxI = (b > maxI) ? b : maxI;
			data[px] = maxI; 
		}
	}
	return true;
}

bool MLGray::Helmholtz(const string fileName,double factor) {
	int ch;
	unsigned char* d = LoadImage(fileName, width, height, ch);
	if (d == nullptr) { return false; }
	data = new int32_t[width * height];
	if (ch == 1) { return CopyData(d); }
	int32_t R, G, B;
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int lpStb = lpos * ch;
		for (int x = 0; x < width; x++) {
			int px = lpos + x;
			int pxStb = lpStb + x * ch;
			R = d[pxStb];
			G = d[pxStb + 1];
			B = d[pxStb + 2];
			double Y = (0.299 * R) + (0.587 * G) + (0.114 * B);
			double U = (B - Y) * 0.493;
			double V = (R - Y) * 0.877;
			data[px] = (int32_t)(int32_t)(Y + factor * (U + V) + 0.5); // Last Term is Rounding
		}
	}
	return true;
}

bool MLGray::Logistic(double scale) {
	if ((height <= 0) || (width <= 0)) { return false; }
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		for (int x = 0; x < width; x++) {
			double v = (data[lpos + x]-128)*scale;
			v = 1.0 / (1.0 + exp(-v));

			data[lpos + x] = (int32_t)(WHITE*v+0.5);
		}
	}
	return true;
}

bool MLGray::Jarvis(int32_t threshold) {
	if ((height <= 1) || (width <= 1)) { return false; }
	const double f7 = 7.0 / 48.0;
	const double f5 = 5.0 / 48.0;
	const double f3 = 3.0 / 48.0;
	const double f1 = 1.0 / 48.0;
	int lpos;
	int px;
	for (int y = 0; y < height; y++) {
		lpos = line(y);
		for (int x = 0; x < width; x++) {
			px = lpos + x;
			int32_t v = data[px];
			data[px] = (v < threshold) ? BLACK : WHITE;
			int32_t err = v - data[px];
			if (x < width - 1) {
				data[px + 1] += (int32_t)(err * f7 + 0.5);
				if (y < height - 1) {
					data[px + width+1] += (int32_t)(err * f5 + 0.5);
					if (y < height - 2) {
						data[px + 2*width + 1] += (int32_t)(err * f3 + 0.5);
					}
				}
				if (x < width - 2) {
					data[px + 2] += (int32_t)(err * f5 + 0.5);
					if (y < height - 1) {
						data[px + width + 2] += (int32_t)(err * f3 + 0.5);
						if (y < height - 2) {
							data[px + 2 * width + 2] += (int32_t)(err * f1 + 0.5);
						}
					}
				}
			}
			if (y < height - 1) { 
				data[px + width] += (int32_t)(err * f7 + 0.5); 
				if(y<height-2) { data[px + 2*width] += (int32_t)(err * f5 + 0.5); }
			}
			if (x>0) {
				if (y < height - 1) {
					data[px + width - 1] += (int32_t)(err * f5 + 0.5);
					if (y < height - 2) { data[px + 2*width - 1] += (int32_t)(err * f3 + 0.5); }
				}
				if (x > 1) {
					if (y < height - 1) {
						data[px + width - 2] += (int32_t)(err * f3 + 0.5);
						if (y < height - 2) { data[px + 2 * width - 2] += (int32_t)(err * f1 + 0.5); }
					}

				}
			}

		}
	}
	return true;
}


bool MLGray::FloydSteinberg(int32_t threshold) {
	if ((height <= 1) || (width <= 1)) { return false; }
	const double f7 = 0.4375; // 7/16
	const double f5 = 0.3125;
	const double f3 = 0.1875;
	const double f1 = 0.0625;
	int lpos;
	int px;
	for (int y = 0; y < height - 1; y++) {
		lpos = line(y);
		int32_t v = data[lpos];
		data[lpos] = (v < threshold) ? BLACK : WHITE;
		int32_t err = v - data[lpos];
		data[lpos + 1] += (int32_t)(err * f7 +0.5);
		data[lpos + width] += (int32_t)(err * f5 + 0.5);
		data[lpos + width + 1] += (int32_t)(err * f1 + 0.5);
		for (int x = 1; x < width - 1; x++) {
			px = lpos + x;
			int32_t v = data[px];
			data[px] = (v < threshold) ? BLACK : WHITE;
			int32_t err = v - data[px];
			data[px + 1] += (int32_t)(err * f7 + 0.5); 
			data[px + width-1] += (int32_t)(err * f3 + 0.5);
			data[px + width] += (int32_t)(err * f5 + 0.5); 
			data[px + width + 1] += (int32_t)(err * f1 + 0.5); 
		}
		px = lpos + width - 1;
		v = data[px];
		data[px] = (v < threshold) ? BLACK : WHITE;
		err = v - data[px];
		data[px + width - 1] += (int32_t)(err * f3 + 0.5);
		data[px + width] += (int32_t)(err * f5 + 0.5);
	}
	lpos = line(height - 1);
	for (int x = 0; x < width - 1;x++) {
		px = lpos + x;
		int32_t v = data[px];
		data[px] = (v < threshold) ? BLACK : WHITE;
		int32_t err = v - data[px];
		data[px + 1] += (int32_t)(err * f7 + 0.5);
	}
	return true;
}

bool MLGray::Ostromoukhov(int32_t threshold) {
	if ((height <= 1) || (width <= 1)) { return false; }
	int lpos;
	int px;
	double f0;
	double f1;
	double f2;
	for (int y = 0; y < height - 1; y++) {
		lpos = line(y);
		int32_t v = clamp(data[lpos]);
		data[lpos] = (v < threshold) ? BLACK : WHITE;
		int32_t err = v - data[lpos];
		v*=4;
		f0 = (double)OstromC[v] / OstromC[v + 3];
		f1 = (double)OstromC[v+1] / OstromC[v + 3];
		f2 = (double)OstromC[v + 2] / OstromC[v + 3];
		data[lpos + 1] += (int32_t)(err * f0 + 0.5);
		data[lpos + width] += (int32_t)(err * f2 + 0.5);
		for (int x = 1; x < width - 1; x++) {
			px = lpos + x;
			int32_t v = clamp(data[px]);
			data[px] = (v < threshold) ? BLACK : WHITE;
			int32_t err = v - data[px];
			v *= 4;
			f0 = (double)OstromC[v] / OstromC[v + 3];
			f1 = (double)OstromC[v + 1] / OstromC[v + 3];
			f2 = (double)OstromC[v + 2] / OstromC[v + 3];
			data[px + 1] += (int32_t)(err * f0 + 0.5);
			data[px + width - 1] += (int32_t)(err * f1 + 0.5);
			data[px + width] += (int32_t)(err * f2 + 0.5);
		}
		px = lpos + width - 1;
		v = clamp(data[px]);
		data[px] = (v < threshold) ? BLACK : WHITE;
		err = v - data[px];
		v *= 4;
		f1 = (double)OstromC[v + 1] / OstromC[v + 3];
		f2 = (double)OstromC[v + 2] / OstromC[v + 3];
		data[px + width - 1] += (int32_t)(err * f1 + 0.5);
		data[px + width] += (int32_t)(err * f2 + 0.5);
	}
	lpos = line(height - 1);
	for (int x = 0; x < width - 1; x++) {
		px = lpos + x;
		int32_t v = clamp(data[px]);
		data[px] = (v < threshold) ? BLACK : WHITE;
		int32_t err = v - data[px];
		v *= 4;
		f0 = (double)OstromC[v] / OstromC[v + 3];
		data[px + 1] += (int32_t)(err * f0 + 0.5);
	}
	return true;
}

bool MLGray::LaplaceSharpen(double factor) {
	if ((height <= 0) || (width <= 0)) { return false; }

	MLGray t = MLGray(width,height,data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int32_t lap33=t.Conv33(px,Laplace);
			data[px] += (int32_t)(factor*lap33 + 0.5);
		}
	}
	return true;
}

bool MLGray::MedianFilter33() {
	if ((height <= 0) || (width <= 0)) { return false; }

	MLGray t = MLGray(width, height, data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			data[px] = t.Median33(px);
		}
	}
	return true;
}

bool MLGray::SaltPepper(int32_t threshold) {
	if ((height <= 0) || (width <= 0)) { return false; }
	int32_t wthreshold = threshold*WHITE;
	int32_t bthreshold = (9 - threshold) * WHITE;
	MLGray t = MLGray(width, height, data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int v = data[px];
			int a = Accumulate33(px);
			if (v == WHITE) {
				if (a <= wthreshold) { data [px]= BLACK; }
			}
			else {
				if (a >= bthreshold) { data[px] = WHITE; }
			}


			data[px] = t.Median33(px);
		}
	}
	return true;
}



bool MLGray::Bayer44() {
	if ((height <= 0) || (width <= 0)) { return false; }

	MLGray t = MLGray(width, height, data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		int my = (y % 4)*4;
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int mx = my + (x % 4);
			int32_t v = data[px];
			data[px] = (v>=BayerMsk44[mx])?WHITE:BLACK;
		}
	}
	return true;
}

bool MLGray::Bayer88() {
	if ((height <= 0) || (width <= 0)) { return false; }

	MLGray t = MLGray(width, height, data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		int my = (y % 8) * 8;
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int mx = my + (x % 8);
			int32_t v = data[px];
			data[px] = (v >= BayerMsk88[mx]) ? WHITE : BLACK;
		}
	}
	return true;
}

bool MLGray::BayerRnd88(int32_t range) {
	if ((height <= 0) || (width <= 0)||(range<=0)) { return false; }
	std::mt19937 rnd(47114713); // seed the generator
	std::uniform_int_distribution<> unif(-range,range); // define the range


	MLGray t = MLGray(width, height, data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		int my = (y % 8) * 8;
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int mx = my + (x % 8);
			int32_t v = data[px]+unif(rnd);
			data[px] = (v >= BayerMsk88[mx]) ? WHITE : BLACK;
		}
	}
	return true;
}






bool MLGray::LaplaceFilter(int offset) {
	if ((height <= 0) || (width <= 0)) { return false; }
	MLGray t = MLGray(width, height,data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int px1 = px + width;
			int px_1 = px - width;
			int32_t lap33 = Conv33(px,Laplace);
			data[px] = offset+lap33;
		}
	}
	return false;
}

void MLGray::LinearGradient(bool blackToWhite) {
	CreateImage(512,512);
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			data[px] = (blackToWhite) ? x / 2 : WHITE - x / 2;
		}
	}
}

void MLGray::RadialGradient(bool blackToWhite) {
	CreateImage(512, 512);
	const double xm = 255.5;
	const double ym = 255.5;

	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			double dy = y - ym;
			double dx = x - xm;
			double d = sqrt(dy * dy + dx * dx);
			double v = (blackToWhite) ? d : WHITE - d;
			data[px] = (int32_t)(v + 0.5);
		}
	}
}


unsigned char* MLGray::ToStb() {
	int sz = width * height;
	unsigned char* img = new unsigned char[sz * RGB_Channels];
	for (int n = 0,ni=0; n < sz; n++,ni+=RGB_Channels) {
		unsigned char c = clamp(data[n]);
		img[ni] = c;
		img[ni + 1] = c;
		img[ni + 2] = c;
	}
	return img;
}

bool MLGray::SaveImage(string base, string fname,int quality) {
	string fileName = "./image/" + base + "_" + fname + ".jpg";
	return SaveImage(fileName, quality);
}

bool MLGray::SaveImage(string fileName, int quality) {
	unsigned char* img = ToStb();
	stbi_write_jpg(fileName.c_str(), width, height, RGB_Channels, img, quality);
	delete[] img;
	return true;

}



