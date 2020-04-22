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
#include <vector>
using namespace std;

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
	memcpy(data, srcdata, sz * sizeof(int32_t));
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

bool MLGray::ColorChannel(const string fileName, int color) {
	if ((color < RED) || (color > BLUE)) { return false; }
	int ch, w, h;
	unsigned char* d = LoadImage(fileName, w, h, ch);
	if (d == nullptr) { return false; }
	CreateImage(w, h);
	if (ch == 1) { 
		CopyData(d); 
	}
	else {
		for (int y = 0; y < height; y++) {
			int lpos = line(y);
			int lpStb = lpos * ch;
			for (int x = 0; x < width; x++) {
				int px = lpos + x;
				int pxStb = lpStb + x * ch;
				data[px] = (int32_t)d[pxStb + color];
			}
		}
	}
	stbi_image_free(d);
	return true;
}


bool MLGray::Saturate(string fileName, double wRed, double wGreen, double wBlue) {
	int ch,w,h;
	unsigned char* d = LoadImage(fileName, w, h, ch);
	if (d == nullptr) { return false; }
	CreateImage(w, h);
	if (ch == 1) { 
		CopyData(d); 
	}
	else {
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
	if (ch == 1) { 
		CopyData(d); 
	}
	else {
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
	}
	stbi_image_free(d);
	return true;
}

bool MLGray::Value(const string fileName) {
	int ch;
	unsigned char* d = LoadImage(fileName, width, height, ch);
	if (d == nullptr) { return false; }
	data = new int32_t[width * height];
	if (ch == 1) { 
		CopyData(d); 
	}
	else {
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
	}
	stbi_image_free(d);
	return true;
}

bool MLGray::Helmholtz(const string fileName,double factor) {
	int ch;
	unsigned char* d = LoadImage(fileName, width, height, ch);
	if (d == nullptr) { return false; }
	data = new int32_t[width * height];
	if (ch == 1) { 
		CopyData(d); 
	}
	else {
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
	}
	stbi_image_free(d);
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
	// Ghost row at top. Propagates the error down to first row
	int32_t* tmp = new int32_t[width];
	memcpy(tmp, data, width * sizeof(int32_t));
	for (int x = 0; x < width - 1; x++) {
		int32_t v = tmp[x];
		int err = (v < threshold) ? v : WHITE - v;
		data[x + 1] += (int32_t)(err * f7 + 0.5);
		data[x] += (int32_t)(err * f7 + 0.5);
	}
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
	delete[] tmp;
	return true;
}

bool MLGray::FloydSteinberg(int32_t threshold) {
	if ((height <= 1) || (width <= 1)) { return false; }
	const double f7 = 0.4375; // 7/16
	const double f5 = 0.3125;
	const double f3 = 0.1875;
	const double f1 = 0.0625;
	const double f8 = f5 + f3;
	int lpos;
	int px;
	// Assumes a ghost-row at top and propagates the errors of this row down.
	int32_t* tmp = new int32_t[width];
	memcpy(tmp, data, width * sizeof(int32_t));
	for (int x = 0; x < width - 1; x++) {
		int32_t v = tmp[x];
		int32_t err = (v < threshold) ? v : WHITE - v;
		data[x + 1] += (int32_t)(err * f7 + 0.5);
		data[x] += (int32_t)(err * f5 + 0.5);
	}
	for (int y = 0; y < height - 1; y++) {
		lpos = line(y);
		int32_t v = data[lpos];
		data[lpos] = (v < threshold) ? BLACK : WHITE;
		int32_t err = v - data[lpos];
		data[lpos + 1] += (int32_t)(err * f7 +0.5);
		data[lpos + width] += (int32_t)(err * f8 + 0.5);  // No bug. Compensates for left-border effects
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
		data[px + width] += (int32_t)(err * f8 + 0.5);  // No Bug. Compensates for rigth border effects
	}
	lpos = line(height - 1);
	for (int x = 0; x < width - 1;x++) {
		px = lpos + x;
		int32_t v = data[px];
		data[px] = (v < threshold) ? BLACK : WHITE;
		int32_t err = v - data[px];
		data[px + 1] += (int32_t)(err * f7 + 0.5);
	}
	delete[]tmp;
	return true;
}

int MLGray::OptOstromoukhov(int from,int to) {
	return OptHalftone(from, to, OSTROMOUKHOV);
}

int MLGray::OptJarvis(int from, int to) {
	return OptHalftone(from, to,JARVIS);
}


int MLGray::OptFloydSteinberg(int from, int to) {
	return OptHalftone(from, to, FLOYDSTEINBERG);
}

int MLGray::OptHalftone(int from, int to,const int halftoneId) {
	if ((halftoneId != FLOYDSTEINBERG) && (halftoneId != OSTROMOUKHOV) && (halftoneId != JARVIS)) { return -1; }
	int32_t sz = width * height;
	double* G = new double[sz];
	double* oG = new double[sz];
	Gauss77FilterDbl(G);
	MLGray ot = MLGray(width, height, data);
	double bestVal = 1.0e20;
	int bestThres = 0;
	for (int thres = from; thres <= to; thres += 4) {
		memcpy(ot.data, data, sz * sizeof(int32_t));
		if (halftoneId == FLOYDSTEINBERG) { ot.FloydSteinberg(thres); }
		if (halftoneId == OSTROMOUKHOV) { ot.Ostromoukhov(thres); }
		if (halftoneId == JARVIS) { ot.Jarvis(thres); }
		ot.Gauss77FilterDbl(oG);
		int64_t dist = 0;
		double diff = L1Distance(G,oG,sz);
		if (diff < bestVal) {
			bestVal = diff;
			bestThres = thres;
		}
		else {
			if (dist > 1.5 * bestVal) { break; }
		}
	}
	int l = bestThres - 3;
	int h = bestThres + 3;
	bestVal = 1.0e20;
	for (int thres = l; thres <= h; thres++) {
		memcpy(ot.data, data, sz * sizeof(int32_t));
		if (halftoneId == FLOYDSTEINBERG) { ot.FloydSteinberg(thres); }
		if (halftoneId == OSTROMOUKHOV) { ot.Ostromoukhov(thres); }
		if (halftoneId == JARVIS) { ot.Jarvis(thres); }
		ot.Gauss77FilterDbl(oG);
		double diff = L1Distance(G,oG,sz);
		if (diff < bestVal) {
			bestVal = diff;
			bestThres = thres;
		}
	}
	double bestDist = bestVal / sz;
	if (halftoneId == FLOYDSTEINBERG) {
		std::cout << "OptFloydSteinberg: BEST-Threshold = " << bestThres << ", bestDist = " << bestDist << std::endl;
		ot.FloydSteinberg(bestThres);
	}
	if (halftoneId == OSTROMOUKHOV) { 
		std::cout << "OptOstromoukhov: BEST-Threshold = " << bestThres << ", bestDist = " << bestDist << std::endl;
		ot.Ostromoukhov(bestThres);
	}
	if (halftoneId == JARVIS) { 
		std::cout << "OptJarvis: BEST-Threshold = " << bestThres << ", bestDist = " << bestDist << std::endl;
		ot.Jarvis(bestThres);
	}
	int64_t dt = 0;
	memcpy(data, ot.data, sz * sizeof(int32_t));
	return bestThres;
}

bool MLGray::Ostromoukhov(int32_t threshold) {
	if ((height <= 1) || (width <= 1)) { return false; }
	int lpos;
	int px;
	double f0;
	double f1;
	double f2;
	// Assumes a ghost-row at top and propagates the errors of this row down.
	int32_t* tmp = new int32_t[width];
	memcpy(tmp, data, width * sizeof(int32_t));
	for (int x = 0; x < width - 1; x++) {
		int32_t v = clamp(tmp[x]);
		int32_t err = (v < threshold) ? v : WHITE - v;
		f0 = (double)OstromC[v] / OstromC[v + 3];
		f2 = (double)OstromC[v + 2] / OstromC[v + 3];
		data[x + 1] += (int32_t)(err * f0 + 0.5);
		data[x] += (int32_t)(err * f2 + 0.5);
	}

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
		data[lpos + width] += (int32_t)(err * (f2+f1) + 0.5); // Compensate left border effects
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
	delete[] tmp;
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

bool MLGray::Gauss55Filter() {
	if ((height <= 0) || (width <= 0)) { return false; }

	int sz = width * height;
	int32_t* td = new int32_t[sz];
	memcpy(td, data, sz * sizeof(int32_t));
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int px = lpos;
		data[px] = 11 * td[px] + 4 * td[px + 1] + td[px + 2];
		px += 1;
		data[px] = 5 * td[px - 1] + 6 * td[px] + 4 * td[px + 1] + td[px + 2];
		px = lpos + width - 2;
		data[px] = td[px - 2] + 4 * td[px - 1] + 6 * td[px] + 5 * td[px + 1];
		px++;
		data[px] = td[px - 2] + 4 * td[px - 1] + 11 * td[px];

		for (int x = 2; x < width - 2; x++) {
			px = lpos + x;
			data[px] += td[px - 2] + 4 * td[px - 1] + 6 * td[px] + 4 * td[px + 1] + td[px + 2];
		}
	}
	memcpy(td, data, sz * sizeof(int32_t));
	int w1 = width;
	int w2 = w1+ width;
	int w_1 = -width;
	int w_2 = w_1 - width;
	for (int x = 0; x < width; x++) {
		int py = x;
		data[py] = 11 * td[py] + 4 * td[py + w1] + td[py + w2];
		data[py] /= 256;
		py += width;
		data[py] = 5 * td[py + w_1] + 6 * td[py] + 4 * td[py + w1] + td[py + w2];
		data[py] /= 256;
		py = x + width * (height - 2);
		data[py] = td[py + w_2] + 4 * td[py + w_1] + 6 * td[py] + 5 * td[py + w1];
		data[py] /= 256;
		py += width;
		data[py] = td[py + w_2] + 4 * td[py + w_1] + 11 * td[py];
		for (int y = 2; y < height - 2; y++) {
			py = x + y * width;
			data[py] = td[py + w_2] + 4 * td[py + w_1] + 6 * td[py] + 4 * td[py + w1] + td[py + w2];
			data[py] /= 256;
		}
	}
	delete[] td;
	return true;
}

double MLGray::L1Distance(double* f1, double* f2, int sz) {
	double d1 = 0.0;
	for (int n = 0; n < sz; n++) {
		d1 += std::abs(f1[n]-f2[n]);
	}
	return d1;
}

double MLGray::L2Distance(double* f1, double* f2, int sz) {
	double d2 = 0.0;
	for (int n = 0; n < sz; n++) {
		double d = f1[n] - f2[n];
		d2 += d*d;
	}
	return d2;
}

bool MLGray::Gauss77FilterFromD() {
	int sz = width * height;
	double* fx = new double[sz];
	Gauss77FilterDbl(fx);
	for (int n = 0; n < sz; n++) {
		data[n] = (int32_t)(fx[n] + 0.5);
	}
	return true;
}


bool MLGray::Gauss77FilterDbl(double *f) {
	if ((height <= 0) || (width <= 0)) { return false; }

	int sz = width*height;
	double* fx = new double[sz];
	for (int n = 0; n < sz; n++) {
		f[n] = 0.0;
		fx[n] = 0.0;
	}
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int px = lpos;
		fx[px] = 42.0*data[px]+15.0*data[px+1]+6.0*data[px+2]+data[px+3];
		px += 1;
		fx[px] = 22.0*data[px-1]+20.0*data[px]+15.0*data[px+1]+6.0*data[px+2]+data[px+3];
		px += 1;
		fx[px] = 7.0*data[px-2]+15.0*data[px-1]+20.0*data[px]+15.0*data[px+1]+6.0*data[px+2]+data[px+3];
		px = lpos+width-3;
		fx[px] = data[px-3]+6.0*data[px-2]+15.0*data[px-1]+20.0*data[px]+15.0*data[px+1]+7.0*data[px+2];
		px++;
		fx[px] = data[px-3]+6.0*data[px-2]+15.0*data[px-1]+20.0*data[px]+22.0*data[px+1];
		px++;
		fx[px] = data[px-3]+6.0*data[px-2]+15.0*data[px-1]+42.0*data[px];

		for (int x = 3; x < width-3; x++) {
			px = lpos+x;
			fx[px] = data[px-3]+6.0*data[px-2]+15.0*data[px-1]+20.0*data[px]+15.0*data[px+1]+6.0*data[px+2]+data[px+3];
		}
	}
	int w1 = width;
	int w2 = w1+width;
	int w3 = w2+width;
	int w_1 = -width;
	int w_2 = w_1-width;
	int w_3 = w_2-width;
	for (int x = 0; x < width; x++) {
		int py = x;
	    f[py] = 42.0*fx[py]+15.0*fx[py+w1]+6.0*fx[py+w2]+fx[py+w3];
		f[py] /= 4096.0;
		py += width;
		f[py] = 22.0*fx[py+w_1]+20.0*fx[py]+15.0*fx[py+w1]+6.0*fx[py+w2]+fx[py+w3];
		f[py] /= 4096.0;
		py += width;
		f[py] = 7.0*fx[py+w_2]+15.0*fx[py+w_1]+20.0*fx[py]+15.0*fx[py+w1]+6.0*fx[py+w2]+fx[py+w3];
		f[py] /= 4096.0;
		py = x+width*(height-3);
		f[py] = fx[py+w_3]+6.0*fx[py+w_2]+15.0*fx[py+w_1]+20.0*fx[py]+15.0*fx[py+w1]+7.0*fx[py+w1];
		f[py] /= 4096.0;
		py += width;
		f[py] = fx[py+w_3]+6.0*fx[py+w_2]+15.0*fx[py+w_1]+20.0*fx[py]+22.0*fx[py+w1];
		f[py] /= 4096.0;
		py += width;
		f[py] = fx[py+w_3]+6.0*fx[py+w_2]+15.0*fx[py+w_1]+42.0*fx[py];
		f[py] /= 4096.0;

		for (int y = 3; y < height-3; y++) {
			py = x+y*width;
			f[py] = fx[py+w_3]+6.0*fx[py+w_2]+15.0*fx[py+w_1]+20.0*fx[py]+15.0*fx[py+w1]+6.0*fx[py+w2]+fx[py+w3];
			f[py] /= 4096.0;
		}
	}
	delete[] fx;
	return true;
}


bool MLGray::Gauss77Filter() {
	if ((height <= 0) || (width <= 0)) { return false; }

	int sz = width * height;
	int32_t* td = new int32_t[sz];
	memcpy(td, data, sz * sizeof(int32_t));
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		int px = lpos;
		data[px] = 42 * td[px] + 15 * td[px + 1] + 6*td[px + 2]+td[px+3];
		px += 1;
		data[px] = 22 * td[px-1] + 20 * td[px] + 15 * td[px + 1] + 6*td[px + 2]+ td[px + 3];
		px += 1;
		data[px] = 7*td[px-2]+15 * td[px-1] + 20 * td[px] + 15 * td[px + 1] + 6 * td[px + 2] + td[px + 3];
		px = lpos + width-3;
		data[px] = td[px-3]+6*td[px-2] + 15 * td[px-1] + 20 * td[px] + 15 * td[px + 1]+ 7 * td[px + 2];
		px++;
		data[px] = td[px-3] + 6 * td[px-2] + 15 * td[px-1] + 20 * td[px] + 22 * td[px + 1];
		px++;
		data[px] = td[px-3] + 6 * td[px-2] + 15 * td[px-1] + 42 * td[px];

		for (int x = 3; x < width-3; x++) {
			px = lpos + x;
			data[px] = td[px-3]+6*td[px-2] + 15 * td[px-1] + 20 * td[px] + 15 * td[px + 1] + 6*td[px + 2]+td[px+3];
		}
	}
	memcpy(td, data, sz * sizeof(int32_t));
	int w1 = width;
	int w2 = w1 + width;
	int w3 = w2 + width;
	int w_1 = -width;
	int w_2 = w_1 - width;
	int w_3 = w_2 - width;
	for (int x = 0; x < width; x++) {
		int py = x;
		data[py] = 42 * td[py] + 15 * td[py + w1] + 6*td[py + w2]+td[py+w3];
		data[py] /= 4096;
		py += width;
		data[py] = 22 * td[py + w_1] + 20 * td[py] + 15 * td[py + w1] + 6*td[py + w2]+td[py+w3];
		data[py] /= 4096;
		py += width;
		data[py] = 7 * td[py + w_2]+15 * td[py + w_1] + 20 * td[py] + 15 * td[py + w1] + 6 * td[py + w2] + td[py + w3];
		data[py] /= 4096;

		py = x + width * (height - 3);
		data[py] = td[py + w_3] + 6*td[py + w_2] + 15 * td[py + w_1] + 20 * td[py] + 15 * td[py + w1]+7 * td[py + w1];
		data[py] /= 4096;
		py += width;
		data[py] = td[py + w_3] + 6 * td[py + w_2] + 15 * td[py + w_1] + 20 * td[py] + 22 * td[py + w1];
		data[py] /= 4096;
		py += width;
		data[py] = td[py + w_3] + 6 * td[py + w_2] + 15 * td[py + w_1] + 42 * td[py];
		data[py] /= 4096;

		for (int y = 3; y < height - 3; y++) {
			py = x + y * width;
			data[py] = td[py + w_3] + 6 * td[py + w_2] + 15 * td[py + w_1] + 20 * td[py] + 15 * td[py + w1] + 6 * td[py + w2] + td[py + w3];
			data[py] /= 4096;
		}
	}
	delete[] td;
	return true;
}



bool MLGray::Med5Laplace(double factor) {
	if (!MedianFilter5()) { return false; }
	return LaplaceSharpen(factor);
}

bool MLGray::Rescale(double offset,double factor) {
	if ((height <= 0) || (width <= 0)) { return false; }

	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			data[px] = (int32_t)(offset + factor * data[px]+0.5);
		}
	}
	return true;
}


bool MLGray::KnuthEdge(double factor) {
	if ((height <= 0) || (width <= 0)||(factor<0)||(factor>=1.0)) { return false; }
	const double denom = 1.0 - factor;
	MLGray t = MLGray(width, height, data);
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			double mx = (double)t.Accumulate33(x,y) / 9.0;
			int32_t v = data[px];
			data[px] = (int32_t)((v-factor*mx)/denom + 0.5);
		}
	}
	return true;
	;
}


bool MLGray::MedianFilter9() {
	if ((height <= 0) || (width <= 0)) { return false; }

	MLGray t = MLGray(width, height, data);
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			data[px] = t.MedianMsk9(px);
		}
	}
	return true;
}

bool MLGray::MedianFilter5() {
	if ((height <= 0) || (width <= 0)) { return false; }

	MLGray t = MLGray(width, height, data);

	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			data[px] = t.MedianMsk5(px);
		}
	}
	return true;
}


bool MLGray::GameOfLife(int32_t life,int generations) {
	if ((height <= 0) || (width <= 0)) { return false; }
	//cout << "GoL life = " << life << ", generations = " << generations << endl;
	int sz=width*height;
	const int32_t W2=2*WHITE;
	const int32_t W3=3*WHITE;
	const int32_t W5=5*WHITE;
	const int32_t W6=6*WHITE;
	MLGray t = MLGray(width, height);
	for (int g = 0; g < generations; g++) {
		memcpy(t.data,data,sz*sizeof(int32_t));
		for (int y = 0; y < height; y++) {
			int lpos = line(y);
			for (int x = 0; x < width; x++) {
				int px = lpos + x;

				int v = t.data[px];
				int a = t.Accumulate8(x, y);
				if (v == WHITE) {
					if (life == 1) {
						data[px]=((a==W2)||(a==W3))?WHITE:BLACK; // Living White
					}
					else {
						data[px]=(a==W5)?BLACK:WHITE;     // Dead black
					}
				}
				else {
					if (life == 0) {
						data[px]==((a==W5)||(a==W6))?BLACK:WHITE; // Living Black
					}
					else {
						data[px]=(a==W3)?WHITE:BLACK;  // Dead White Pixel
					}
				}
			}
		}
	}
	return true;
}
  
bool MLGray::SaltPepper(int32_t threshold) {
	if ((height <= 0) || (width <= 0)) { return false; }
	int32_t wthreshold = threshold*WHITE;
	int32_t bthreshold = (9 - threshold) * WHITE;
	MLGray t = MLGray(width, height, data);
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		for (int x = 0; x < width; x++) {
			int px = lpos + x;
			int v = data[px];
			int a = t.Accumulate33(x,y);
			if (v == WHITE) {
				if (a <= wthreshold) { data [px]= BLACK; }
			}
			else {
				if (a >= bthreshold) { data[px] = WHITE; }
			}
		}
	}
	return true;
}

bool MLGray::Majority() {
	if ((height <= 0) || (width <= 0)) { return false; }
	MLGray t = MLGray(width, height, data);
	int32_t W5=5*WHITE;
	for (int y = 0; y < height; y++) {
		int lpos = line(y);
		for (int x = 0; x < width; x++) {
			int px = lpos + x;
			data[px] = (t.Accumulate33(x,y)>=W5)?WHITE:BLACK;
		}
	}
	return true;
}



bool MLGray::Bayer44() {
	if ((height <= 0) || (width <= 0)) { return false; }

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

bool MLGray::Threshold(int32_t threshold) {
	if ((height <= 0) || (width <= 0)) { return false; }
	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		int my = (y % 8) * 8;
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int32_t v = data[px];
			data[px] = (v >= threshold) ? WHITE : BLACK;
		}
	}
	return true;

}

bool MLGray::Random() {
	if ((height <= 0) || (width <= 0)) { return false; }
	std::mt19937 rnd(47114713); // seed the generator
	std::uniform_int_distribution<> unif(BLACK,WHITE); 


	for (int y = 1; y < height - 1; y++) {
		int lpos = line(y);
		for (int x = 1; x < width - 1; x++) {
			int px = lpos + x;
			int32_t r = unif(rnd);
			int32_t v = data[px];
			data[px] = (v >= r) ? WHITE : BLACK;
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



