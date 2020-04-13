/***********************************************************************
*
* Copyright (c) 2020 Dr. Chrilly Donninger
*
* This file is part of MonaLisa
*
***********************************************************************/
#pragma once
#include <cstdint>
#include <string>

using std::string;
/**
<summary>This class implements Operations on a Grayscale Image.
	     The Grayscale is stored as an int32_t. Graylevels can be less than 0 or greater than 255.
</summary>
*/
class MLGray
{
public:
	/**
	<summary> Constructor </summary>
	<param name="w">width of Bitmap</param>
	<param name="h">height of Bitmap</param>
	*/
    MLGray();
	MLGray(int w, int h);
	MLGray(int w,int h,int32_t srcdata[]);
	~MLGray();
    unsigned char* LoadStbi(string fileName, int& width, int& height, int& channels);
	int GetWidth() { return width; }
	int GetHeight() { return height; }
	int32_t *GetData() { return data; }
    bool Saturate(const string fileName, double wRed, double wGreen, double wBlue);
    bool SaturateGIMP(const string fileName,double scaleFac=1.0);
    bool CopyData(const unsigned char* d);
    /**
	<summary> Converts a QImage to Saturated-Gray according to Gr=wRed*RED + wGreen*GREEN + wBlue*BLUE.
	The sum wRed+wGreen+wBlue can have any value. It can be greater or less than 1. The weights can also be negative.
	The only restriction is that the must not create an int32-Overflow.
	</summary>
	<param name="img">QImage image. Can be any QImage format. The width and height must match with the Gray-Image</param>
	<param name="wRed">weight of Red Image component</param>
	<param name="wGreen">weight of Green Image component</param>
	<param name="wGreen">weight of Blue Image component</param>
	<returns>true if operation successfull, false if failed (height or width mismatch).</returns> 
	*/
	/**
	<summary>Convenience function. Calls Saturate() with the weights of GIMP (0.3,0.596,0.11)</summary>
	<param name="img">QImage image. Can be any QImage format. The width and height must match with the Gray-Image</param>
	<param name="scaleFac">The GIMP-weights are multiplied by scaleFac</param>
	<returns>true if operation successfull, false if failed (height or width mismatch).</returns>
	*/
	/**
	<summary>Convenience function. Calls Saturate() with the weights of Qt (0.34375,0.5,0.1625)
	 Qt uses actually integer multiplication with 11/32,16/32,5/32	</summary>
	<param name="img">QImage image. Can be any QImage format. The width and height must match with the Gray-Image</param>
	<param name="scaleFac">The GIMP-weights are multiplied by scaleFac</param>
	<returns>true if operation successfull, false if failed (height or width mismatch).</returns>
	*/
	bool SaturateQt(const string fileName, double scaleFac = 1.0);
	/**
	<summary> Converts a QImage with Gr=(Max(R,G,B)+min(R,G,B))/2. This is an option im GIMP</summary>
	<param name="img">QImage image. Can be any QImage format. The width and height must match with the Gray-Image</param>
	<returns>true if operation successfull, false if failed (height or width mismatch).</returns>
	*/
	bool Desaturate(const string fileName);
	/**
	<summary> Converts a QImage with Gr=Max(R,G,B). This is an option im GIMP</summary>
	<param name="img">QImage image. Can be any QImage format. The width and height must match with the Gray-Image</param>
	<returns>true if operation successfull, false if failed (height or width mismatch).</returns>
	*/
	bool Value(const string fileName);
	/**
	*/
	bool Helmholtz(const string fileName,double factor=0.149);

	bool Logistic(double scale=0.025);
	bool LaplaceSharpen(double factor=-1.0);
	bool LaplaceFilter(int offset);
	bool KirschSharpen(double factor = 1.0);

	bool FloydSteinberg(int32_t threshold = 128);
	bool Jarvis(int32_t threshold = 128);
    bool Ostromoukhov(int32_t threshold = 128);
	bool SaveImage(string base, string filename,int quality=100);
    unsigned char* ToStb();

private:
    const int RGB_Channels = 3;
    const int32_t BLACK = 0;
    const int32_t WHITE = 255;
	inline int pos(int x, int y) { return y * width + x; }
	inline int line(int y) { return y * width; }
	inline int clamp(int c) { return (c < 0) ? 0 : (c < 256) ? c : 255; }
	inline int32_t Conv33(int x, const int32_t* w) {
		int32_t v = w[0] * data[x - width - 1];
		v += w[1] * data[x - width];
		v += w[2] * data[x - width+1];
		v += w[3] * data[x - 1];
		v += w[4] * data[x];
		v += w[5] * data[x+1];
		v += w[6] * data[x + width-1];
		v += w[7] * data[x + width];
		v += w[8] * data[x + width+1];
		return v;
	}
	int width;
	int height;
	int32_t* data;
	const int32_t* KirschN  = new int32_t[9]{ 5, 5, 5,-3, 0,-3,-3,-3,-3 };
	const int32_t* KirschNW = new int32_t[9]{ 5, 5,-3, 5, 0,-3,-3,-3,-3 };
	const int32_t* KirschW  = new int32_t[9]{ 5,-3,-3, 5, 0,-3, 5,-3,-3 };
	const int32_t* KirschSW = new int32_t[9]{-3,-3,-3, 5, 0,-3, 5, 5,-3 };
	const int32_t* KirschS  = new int32_t[9]{-3,-3,-3,-3, 0,-3, 5, 5, 5 };
	const int32_t* KirschSE = new int32_t[9]{-3,-3,-3,-3, 0, 5,-3, 5, 5 };
	const int32_t* KirschE =  new int32_t[9]{-3,-3, 5,-3, 0, 5,-3,-3, 5 };
	const int32_t* KirschNE = new int32_t[9]{-3, 5, 5,-3, 0, 5,-3,-3,-3 };
	const int32_t* Laplace =  new int32_t[9]{ 1, 1, 1, 1,-8, 1, 1, 1, 1 };
    const int32_t* OstromC =  new int32_t[1024] {
    13,     0,     5,    18,
    13,     0,     5,    18,     
    21,     0,    10,    31,     
     7,     0,     4,    11,     
     8,     0,     5,    13,     
    47,     3,    28,    78,     
    23,     3,    13,    39,     
    15,     3,     8,    26,     
    22,     6,    11,    39,     
    43,    15,    20,    78,     
     7,     3,     3,    13,     
   501,   224,   211,   936,     
   249,   116,   103,   468,     
   165,    80,    67,   312,     
   123,    62,    49,   234,     
   489,   256,   191,   936,     
    81,    44,    31,   156,     
   483,   272,   181,   936,     
    60,    35,    22,   117,     
    53,    32,    19,   104,     
   237,   148,    83,   468,     
   471,   304,   161,   936,     
     3,     2,     1,     6,     
   459,   304,   161,   924,     
    38,    25,    14,    77,     
   453,   296,   175,   924,     
   225,   146,    91,   462,     
   149,    96,    63,   308,     
   111,    71,    49,   231,     
    63,    40,    29,   132,     
    73,    46,    35,   154,     
   435,   272,   217,   924,     
   108,    67,    56,   231,     
    13,     8,     7,    28,     
   213,   130,   119,   462,     
   423,   256,   245,   924,     
     5,     3,     3,    11,     
   281,   173,   162,   616,     
   141,    89,    78,   308,     
   283,   183,   150,   616,     
    71,    47,    36,   154,     
   285,   193,   138,   616,     
    13,     9,     6,    28,     
    41,    29,    18,    88,     
    36,    26,    15,    77,     
   289,   213,   114,   616,     
   145,   109,    54,   308,     
   291,   223,   102,   616,     
    73,    57,    24,   154,     
   293,   233,    90,   616,     
    21,    17,     6,    44,     
   295,   243,    78,   616,     
    37,    31,     9,    77,     
    27,    23,     6,    56,     
   149,   129,    30,   308,     
   299,   263,    54,   616,     
    75,    67,    12,   154,     
    43,    39,     6,    88,     
   151,   139,    18,   308,     
   303,   283,    30,   616,     
    38,    36,     3,    77,     
   305,   293,    18,   616,     
   153,   149,     6,   308,     
   307,   303,     6,   616,     
     1,     1,     0,     2,     
   101,   105,     2,   208,     
    49,    53,     2,   104,     
    95,   107,     6,   208,     
    23,    27,     2,    52,     
    89,   109,    10,   208,     
    43,    55,     6,   104,     
    83,   111,    14,   208,     
     5,     7,     1,    13,     
   172,   181,    37,   390,     
    97,    76,    22,   195,     
    72,    41,    17,   130,     
   119,    47,    29,   195,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
    65,    18,    17,   100,     
    95,    29,    26,   150,     
   185,    62,    53,   300,     
    30,    11,     9,    50,     
    35,    14,    11,    60,     
    85,    37,    28,   150,     
    55,    26,    19,   100,     
    80,    41,    29,   150,     
   155,    86,    59,   300,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
   305,   176,   119,   600,     
   155,    86,    59,   300,     
   105,    56,    39,   200,     
    80,    41,    29,   150,     
    65,    32,    23,   120,     
    55,    26,    19,   100,     
   335,   152,   113,   600,     
    85,    37,    28,   150,     
   115,    48,    37,   200,     
    35,    14,    11,    60,     
   355,   136,   109,   600,     
    30,    11,     9,    50,     
   365,   128,   107,   600,     
   185,    62,    53,   300,     
    25,     8,     7,    40,     
    95,    29,    26,   150,     
   385,   112,   103,   600,     
    65,    18,    17,   100,     
   395,   104,   101,   600,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
   395,   104,   101,   600,     
    65,    18,    17,   100,     
   385,   112,   103,   600,     
    95,    29,    26,   150,     
    25,     8,     7,    40,     
   185,    62,    53,   300,     
   365,   128,   107,   600,     
    30,    11,     9,    50,     
   355,   136,   109,   600,     
    35,    14,    11,    60,     
   115,    48,    37,   200,     
    85,    37,    28,   150,     
   335,   152,   113,   600,     
    55,    26,    19,   100,     
    65,    32,    23,   120,     
    80,    41,    29,   150,     
   105,    56,    39,   200,     
   155,    86,    59,   300,     
   305,   176,   119,   600,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
     5,     3,     2,    10,     
   155,    86,    59,   300,     
    80,    41,    29,   150,     
    55,    26,    19,   100,     
    85,    37,    28,   150,     
    35,    14,    11,    60,     
    30,    11,     9,    50,     
   185,    62,    53,   300,     
    95,    29,    26,   150,     
    65,    18,    17,   100,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
     4,     1,     1,     6,     
   119,    47,    29,   195,     
    72,    41,    17,   130,     
    97,    76,    22,   195,     
   172,   181,    37,   390,     
     5,     7,     1,    13,     
    83,   111,    14,   208,     
    43,    55,     6,   104,     
    89,   109,    10,   208,     
    23,    27,     2,    52,     
    95,   107,     6,   208,     
    49,    53,     2,   104,     
   101,   105,     2,   208,     
     1,     1,     0,     2,     
   307,   303,     6,   616,     
   153,   149,     6,   308,     
   305,   293,    18,   616,     
    38,    36,     3,    77,     
   303,   283,    30,   616,     
   151,   139,    18,   308,     
    43,    39,     6,    88,     
    75,    67,    12,   154,     
   299,   263,    54,   616,     
   149,   129,    30,   308,     
    27,    23,     6,    56,     
    37,    31,     9,    77,     
   295,   243,    78,   616,     
    21,    17,     6,    44,     
   293,   233,    90,   616,     
    73,    57,    24,   154,     
   291,   223,   102,   616,     
   145,   109,    54,   308,     
   289,   213,   114,   616,     
    36,    26,    15,    77,     
    41,    29,    18,    88,     
    13,     9,     6,    28,     
   285,   193,   138,   616,     
    71,    47,    36,   154,     
   283,   183,   150,   616,     
   141,    89,    78,   308,     
   281,   173,   162,   616,     
     5,     3,     3,    11,     
   423,   256,   245,   924,     
   213,   130,   119,   462,     
    13,     8,     7,    28,     
   108,    67,    56,   231,     
   435,   272,   217,   924,     
    73,    46,    35,   154,     
    63,    40,    29,   132,     
   111,    71,    49,   231,     
   149,    96,    63,   308,     
   225,   146,    91,   462,     
   453,   296,   175,   924,     
    38,    25,    14,    77,     
   459,   304,   161,   924,     
     3,     2,     1,     6,     
   471,   304,   161,   936,     
   237,   148,    83,   468,     
    53,    32,    19,   104,     
    60,    35,    22,   117,     
   483,   272,   181,   936,     
    81,    44,    31,   156,     
   489,   256,   191,   936,     
   123,    62,    49,   234,     
   165,    80,    67,   312,     
   249,   116,   103,   468,     
   501,   224,   211,   936,     
     7,     3,     3,    13,     
    43,    15,    20,    78,     
    22,     6,    11,    39,     
    15,     3,     8,    26,     
    23,     3,    13,    39,     
    47,     3,    28,    78,     
     8,     0,     5,    13,     
     7,     0,     4,    11,     
    21,     0,    10,    31,     
    13,     0,     5,    18,     
    13,     0,     5,    18 };   
   

};

