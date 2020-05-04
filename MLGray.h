/***********************************************************************
*
* Copyright (c) 2020 Dr. Chrilly Donninger
* The code can be freely used for private and educational projects.
* Commerical users must ask the author for permission at c.donninger@wavenet.at
*
* This file is part of MonaLisa
*
***********************************************************************/
#pragma once
#include <cstdint>
#include <string>
#include "Calc.h"


using std::string;
/**
<summary>
    This class implements Operations on a Grayscale Image.
	The Grayscale is stored as an int32_t. Graylevels can be less than 0 or greater than 255. But they are clamped
    to this range when the image is saved to a *.jpg file.
    For Loading and Saving from/to JPG the stb_image Library Copyright (c) 2017 Sean Barrett is used.
    The library is wrapped by the LoadImage() and SaveImage() methods. If you want to use another Image-IO libary,
    modify the code in these 2 methods. The rest should work without any changes. 
    One must first read in a JPG (Color) image. There are several conversions from color to grayscale implemented.
    (see the handbook for a complete listing).
    The grayscale image can be optionally preprocessed with a filter. 
    In the next step one can apply Filters and the Halftoning algorithms. 
    The halftonig images can be postprocessed.
    Only the first conversion step is necessary. The following steps can be skipped.
    Halftone-Images are stored in the same way as grayscale. They have only 2 values, WHITE (255) and BLACK (0).
</summary>
*/
class MLGray
{
public:
	/**
	<summary> 
    Default Constructor.
    Typical usage: 
    MLGray img;
    img.SaturateGIMP("./image/Lena.jpg"); //Or any other of the Color to Grayscale conversion algorithms.
    </summary>
    */
    MLGray();
    /**
    <summary>Constructs an image with paramter width and height and allocates the data-array.</summary>
    <param name="width">  width of image. </param>
    <param name="height">  height of image. </param>
    */
    MLGray(int width, int height);
    /**
    Constructs an image with paramter width and height, allocates the data-array and copies the src-data.
    <param name="width">  width of image. </param>
    <param name="height">  height of image. </param>
    <param name="srcdata"> The data of the original image </param>
    */
    MLGray(int width, int height, int32_t* srcdata);
	~MLGray();
    /**
    <summary>
    Creates an image with the given width and height and allocates the data-array. Previous allocated memory is deleted.
    </summary>
    <param name="width">  width of image. </param>
    <param name="height">  height of image. </param>
    <returns>true if operation succeeds, false if memory can not be allocated. This is a serious problem.</returns>
    */
    bool CreateImage(int width, int height);
    /**
    <summary> A thin wrapper around the stbi_load() function. Loads the image from disc into an array 
    of unsigned char. The the pixels are stored line-wise without any padding. In case of a gray-scale image (channels==1)
    one char per pixel. In case of RGB images (channel==3) as triples [r,g,b]. If you want to replace stbi_image() you
    have to return the data in the same format.
    </summary>
    <param name="fileName"> Full Filename with ending. Example: "./image/Lena.jpg"
    <param name="width">  width of image. </param>
    <param name="height">  height of image. </param>
    <param name="channels">  channels == 1 for grayscale and 3 for RGB. stbi_image supports also ARGB with channels == 4</param>
    <returns>pointer to data if image can be loaded, nullptr if load failed</returns>
    */
    unsigned char* LoadImage(string fileName, int& width, int& height, int& channels);
    /**
    <returns>width of image</returns>
    */
	int GetWidth() { return width; }
    /**
    <returns>heigth of image</returns>
    */
    int GetHeight() { return height; }
    /**
    <returns>data-array aka pixel-values of image</returns>
    */
    int32_t *GetData() { return data; }
    /**
    Reads a color image and copies the specified color. Use this method if you read in a gray-image stored as RGB.
    The routine can of course also be used for fancy effects</summary>
    <param name="fileName"> Full filename of image. Example: "./image/Lena.jpg"</param>
    <param name="color">the color channel. R==0, G=1, B=2, Default: 0 (Red)</param>
    <returns>true if operation successfull, false if one can not read the image or invalid color channel</returns>
    */
    bool ColorChannel(const string fileName,int color = 0);
    /**
    <summary> Reads a color image and converts it to Saturated-Gray with gray intensity I.
    I = wRed*RED + wGreen*GREEN + wBlue*BLUE.
    The sum wRed+wGreen+wBlue can have any value. It can be greater or less than 1. The weights can also be negative.
    The only restriction is that they must not create an int32-Overflow.
    If the original image is gray, the values are just copied.
    </summary>
    <param name="fileName"> Full filename of image. Example: "./image/Lena.jpg"</param>
    <param name="wRed">weight of Red Image component</param>
    <param name="wGreen">weight of Green Image component</param>
    <param name="wBlue">weight of Blue Image component</param>
    <returns>true if operation successfull, false if one can not read the image file).</returns>
    */
    bool Saturate(const string fileName, double wRed, double wGreen, double wBlue);
    /**
    <summary>Convenience function. Calls Saturate() with the weights of GIMP (0.3,0.596,0.11)</summary>
    <param name="scaleFac">The GIMP-weights are multiplied by scaleFac. Default: 1.0</param>
    <returns>true if operation successfull, false if one can not read the image file.</returns>
    */
    bool SaturateGIMP(const string fileName, double scaleFac = 1.0);
    /**
     <summary>Convenience function. Calls Saturate() with the weights of Qt (0.34375,0.5,0.1625)
      Qt uses actually integer multiplication with 11/32, 16/32, 5/32	</summary>
     <param name="fileName"> Full filename of image. Example: "./image/Lena.jpg"</param>
     <param name="scaleFac">The Qt-weights are multiplied by scaleFac</param>
    <returns>true if operation successfull, false if one can not read the image file.</returns>
     */
    bool SaturateQt(const string fileName, double scaleFac = 1.0);
    /**
    <summary>Copies gray-scale data to the internal array. The width and height must match. This is not checked.
    This method is called when a gray-image is loaded.
    </summary>
     <param name="d">The original gray values</param>
     <returns>always true</return>
    */
    bool CopyData(const unsigned char* d);
  /**
	<summary> Converts a Color image with gray scale intensity I set to:
    I=(Max(R,G,B)+min(R,G,B))/2. 
    This is an option in GIMP
    </summary>
	<param name="fileName"> Full filename of image. Example: "./image/Lena.jpg"</param>
    <returns>true if operation successfull, false if one can not read the image file.</returns>
   */
	bool Desaturate(const string fileName);
	/**
	<summary> Converts a Color image with gray scale intensity I set to:
    I=Max(R,G,B). 
    This is an option in GIMP.
    </summary>
    <param name="fileName"> Full filename of image. Example: "./image/Lena.jpg"</param>
    <returns>true if operation successfull, false if one can not read the image file.</returns>
 	*/
	bool Value(const string fileName);
	/**
    <summary>Converts a Color image with gray scale intensity I set to: 
            Y = (0.299 * R) + (0.587 * G) + (0.114 * B);  Standard Conversion
            U = (B - Y) * 0.493;      Pure Blue or
            V = (R - Y) * 0.877;      Pure Red increases intensity.
            I= Y + factor*(U+V);     U+V is a measure of the effect.
            It tries to correct the grayscale by the Helmholtz–Kohlrausch effect.
    </summary>
    <param name="fileName"> Full filename of image. Example: "./image/Lena.jpg"</param>
    <param name="factor"> The weight of the Helmholtz-Kohlrausch effect</param>
    <returns>true if operation successfull, false if one can not read the image file.</returns>
    */
	bool Helmholtz(const string fileName,double factor=0.149);
    /**
    <summary> Applies a logistic transformation to the gray scales I of an existing image. Values of middle intensities
              are stretched, almost BLACK or WHITE moved towards BLACK or WHITE. 
              x = (I-128)*scale;
              v = 1.0 / (1.0 + exp(-x));
              I=WHITE*v;
    </summary>
    <param name="scale"> Defines the steepness of the logistic curve</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
	bool Logistic(double scale=0.025);
    /**
    <summary> Adds a Laplace-filter to the image. 
     1, 1, 1, 
     1,-8, 1, 
     1, 1, 1 
     Edges but also noise are enhanced. This filter can be applied as a preprocssing step for halftoning. 
     Bright edges are made more white, dark edges more black. But noise is also intensified. To overcome this
     problem use the Med5Laplace() filter.
     </summary>
     <param name="factor"> The filter is scaled by this value. Default -1.0</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool LaplaceSharpen(double factor=-1.0);
    /**
    <summary> Applies first a MedianFilter5() and calls then LaplaceSharpen(). The idea is to remove first
    salt and pepper and then call Laplace to enhance edges. This is a preprocessing method.
    </summary>
    <param name="factor"> The Laplace-filter is scaled by this value. Default -1.0</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Med5Laplace(double factor = -1.0);
    /**
    <summary> Filters the image with the median of the mask.
     1 1 1
     1 x 1
     1 1 1
    </summary>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool MedianFilter9();
    /**
    <summary> Filters the image with the median of the mask
      .  1  .
      1  x  1
      .  1  .
    </summary>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool MedianFilter5();
    /**
    <summary> Calcuates a Laplace-filter for the image. This can be used for edge detection. But is has no direct
       use for halftoning.
    </summary>
     <param name="offset">The value of the filter is added to this offset. The filter values can be negative. Default: 128</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool LaplaceFilter(int offset=128);
    /**
    <summary> Rescales the gray scale I of thhe image according to: 
    I=offset+factor*I. 
    The method can be used for preprocessing 
    </summary>
    <param name="offset">See the linear equation above. Default 25.5</param>
    <param name="factor">See the linear equation above. Default 0.8</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Rescale(double offset = 25.5, double factor = 0.8);
    /**
    <summary> Separable Gauss 5x5 Filter. 1 4 6 4 1.</summary> 
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Gauss55Filter();
    /**
    <summary> Separable Gauss 7x7 Filter. 1 6 15 20 15 6 1.</summary>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Gauss77Filter();
    /**
    <summary> Separable Gauss 7x7 Filter. 1 6 15 20 15 6 1.
    Does not modify the data. It stores the exact values of the convolution in the filter-array.
    </summary>
    <param name="filter">The convolution result is stored in this array. It must be at least have the size
    of width*height. </param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Gauss77FilterDbl(double *filter);
    /**
    <summary> Enhences edges by the method proposed in D.Knuth: Digital Halftones by Dot Diffusion
    I=(I-factor*meanI)/(1-factor). meanI is the mean value in a 3x3 mask 
    </summary>
    <note> The factor 0.9 is equivalent to the Laplace filter.</note>
    <param name="factor">See the equation above. Default 0.8. factor must be within [0,1.0) </param>
    <returns>true if operation successfull, false if image is empty or factor not in range.</returns>
    */
    bool KnuthEdge(double factor = 0.8);
    /**
    <summary>Implements the Floyd-Steinberg error diffusion halftoning algorithm. 
    .   x    7
    3   5    1   Divided by 16
    </summary>
    <param name="threshold">The pixel is set to WHITE if the diffused I>=threshold.</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
 	bool FloydSteinberg(int32_t threshold = 128);
    /**
    <summary> Selects the optimal threshold for FloydSteinberg Halftonig. Best is defined as the L1-distance between
    the 7x7 Gauss-Filter of the original image and the 7x7 Gauss-Filter of the Halftone.
    The image is dithered with the optimal threshold.
    </summary>
    <param name="from">The threshold search is done within range [from,to]. Default: 64</param>
    <param name="to">The threshold search is done within range [from,to]. Default: 192</param>
    <returns>the best threshold or -1 if image is empty.</returns>
    */
    int OptFloydSteinberg(int from = 64, int to = 192);
    /**
    <summary>Implements "minimized average error" halftoning algorithm by J.Jarvis, C.Judice and W. Ninke
    The algorithm is similar to Floyd-Steinberg. The diffusion mask is larger. 
      .   .   x   7   5
      3   5   7   5   3
      1   3   5   3   1     divided by 48
    </summary>
    <param name="threshold">The pixel is set to WHITE if the diffused I>=threshold.</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
	bool Jarvis(int32_t threshold = 128);
    /**
    <summary> Selects the optimal threshold for Jarvis Halftonig. Best is defined as the L1-distance between
    the 7x7 Gauss-Filter of the original image and the 7x7 Gauss-Filter of the Halftone</summary>
    The image is dithered with the optimal threshold.
    <param name="from">The threshold search is done within range [from,to]. Default: 64</param>
    <param name="to">The threshold search is done within range [from,to]. Default: 192</param>
    <returns>the best threshold or -1 if image is empty.</returns>
    */
    int OptJarvis(int from = 64, int to = 192);
    /**
    <summary>Implements "MECCA - A multiple error correction computation algorithm for bilevel hardcopy reproduction" by P. Stucki
    The algorithm is very similar to Jarvis. The mask is the same, the weights are slightly different. 
      .   .   x   8   4
      2   4   8   4   2
      1   2   4   2   1     divided by 42
    </summary>
    <param name="threshold">The pixel is set to WHITE if the diffused I>=threshold.</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
	bool Stucki(int32_t threshold = 128);
    /**
    <summary> Selects the optimal threshold for Stucki Halftonig. Best is defined as the L1-distance between
    the 7x7 Gauss-Filter of the original image and the 7x7 Gauss-Filter of the Halftone</summary>
    The image is dithered with the optimal threshold.
    <param name="from">The threshold search is done within range [from,to]. Default: 64</param>
    <param name="to">The threshold search is done within range [from,to]. Default: 192</param>
    <returns>the best threshold or -1 if image is empty.</returns>
    */
    int OptStucki(int from = 64, int to = 192);
    /**
    <summary> Implements the halftoning algorithm from:  Victor Ostromoukhov: "A Simple and Efficient Error-Diffusion Algorithm"
     The algorithm uses for each grayscale an own error-diffusion matrix. The Matrix was optimized to eliminate noise.
    </summary>
    <param name="threshold">The pixel is set to WHITE if the diffused I>=threshold.</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Ostromoukhov(int32_t threshold = 128);
    /**
    <summary> Selects the optimal threshold for Ostromoukhov Halftonig. Best is defined as the L1-distance between
    the 7x7 Gauss-Filter of the original image and the 7x7 Gauss-Filter of the Halftone.
    The image is dithered with the optimal threshold.
    </summary>
    <param name="from">The threshold search is done within range [from,to]. Default: 64</param>
    <param name="to">The threshold search is done within range [from,to]. Default: 192</param>
    <returns>the best threshold or -1 if image is empty.</returns>
    */
    int OptOstromoukhov(int from=64,int to=192);
     /**
    <summary> Selects the optimal threshold. Best is defined as the L1-distance between
     the 7x7 Gauss-Filter of the original image and the 7x7 Gauss-Filter of the Halftone.
     The image is dithered with the optimal threshold.
     The method is called by the Opt... methods above and does the real work.
    </summary>
    <param name="from">The threshold search is done within range [from,to]. Default: 64</param>
    <param name="to">The threshold search is done within range [from,to]. Default: 192</param>
     <param name="halftoneId">The underlying Halftone-Algo. One of FLOYDSTEINBERG,OSTROMOUKHOV,JARVIS</param>
    <returns>the best threshold or -1 if image is empty or halftoneId is invalid.</returns>
    */
    int OptHalftone(int from, int to,const int halftoneId);
    /**
    <summary>Implements ordered Dither with a 4x4 Bayer matrix.</summary>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Bayer44();
    /**
     <summary>Implements ordered Dither with a 8x8 Bayer matrix.</summary>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Bayer88();
    /**
    <summary>Implements ordered Dither with a 8x8 Bayer matrix. Before accessing the Bayer-Matrix
    a uniform distributed random number from [-range,range] is added to the pixel value. This should
    break the regular patterns created by the Bayer Matrix
    </summary>
    <param name="range">The uniform distribution is created int [-range,range]. Default: 20</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool BayerRnd88(int32_t range=20);
    /**
    <summary> Trivial Dither.</summary>
    <param name="threshold">The pixel is set to WHITE if the I>=threshold.</param>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Threshold(int32_t threshold = 128);
    /**
    <summary> Creates for each pixel a random integer in [0,255]. If I >= Rand Dither is WHITE, otherwise BLACK. </summary>
    <returns>true if operation successfull, false if image is empty.</returns>
    */
    bool Random();
    /**
    <summary> Postprocessing of image. Removes Salt and Pepper. Flips Pixel if there are too less
    of own color in 3x3 region.
    </summary>
    <param name="threshold">The nummer of pixels of same color. Default: 1</param>
    <returns>true if operation successfull, false if image is empty.< / returns>
    */
    bool SaltPepper(int32_t threshold = 1);
    /**
    <summary> Processes halftone image according the rules of Conway's game of life. A living pixel is living
    in the next generation, if it has 2 or 3 living neighbors. A dead pixel is living, if it has 3 living neighbors.
    Otherwise it is dead. 
    </summary>
    <param name="whiteAlive"> If true the white pixels are interpreted as living cells. If false the black pixels
    are living. Default: true is living</param>
    <param name="generations">Number of generations.</params>
    <returns>true if operation successfull, false if image is empty.< / returns>
    */
    bool GameOfLife(bool whiteAlive=true,int generations=1);
    /**
    <summary> A majority filter for halftoning. Sets the pixel to the majority in a 3x3 area. This is a special
    case of a median filter</summary>
    <returns>true if operation successfull, false if image is empty.< / returns>
     */
    bool Majority();
    /**
    <summary> Inverts the halftone image. WHITE to BLACK, BLACK to WHITE</summary>
    <returns>true if operation successfull, false if image is empty.< / returns>
     */
    bool Invert();
    /**
    <summary> Generates a linear gradient image from black to white with the size 512x512</summary>
    <param name="blackToWhite">If true the image starts on the left with BLACK and increase to the right to WHITE.
    If false, it starts with WHITE and decreases to BLACK.</param>
    */
    void LinearGradient(bool blackToWhite=true);
    /**
     <summary> Generates a radial gradient image from black to white with the size 512x512</summary>
    <param name="blackToWhite">If true the image starts in the middle with BLACK and radiates outside to WHITE.
    If false, it starts with WHITE and decreases to BLACK.  </param>
    */
    void RadialGradient(bool blackToWhite = true);
    /**
    <summary> Saves the image in *.jpg format. The image is stored as a RGB-color-image with same values for R,G,B.
    <param name="fileName"> Full Filename. example: "./image/LinearGradient.jpg"</param>
    <param name="quality">The compression quality of the image. Default: 100. Highest quality</param>
    <returns>true if operation successfull, false if failed.</returns>
    */
    bool SaveImage(string fileName, int quality = 100);
    /**
    <summary>Converts the data array to the representation of the Stb_image data</summary>
    */
    unsigned char* ToStb();

private:
    const int RGB_Channels = 3;
    const int32_t BLACK = 0;   // Halftone values.
    const int32_t WHITE = 255;
    const int RED = 0;     // Color Channels
    const int GREEN = 1;
    const int BLUE = 2;
    const int FLOYDSTEINBERG = 0;   // Method ids for threshold Optimizer
    const int OSTROMOUKHOV = 1; 
    const int JARVIS = 2;
    const int STUCKI=3;
    double L1Distance(double* f1, double* f2, int sz);
    double L2Distance(double* f1, double* f2, int sz);
    inline int pos(int x, int y) { return y * width + x; }
	inline int line(int y) { return y * width; }
	inline int clamp(int c) { return (c < 0) ? BLACK : (c <= WHITE) ? c : WHITE; }

    inline int32_t MedianMsk9(int x) {
        return Median9(data[x - width - 1], data[x - width], data[x - width + 1],
            data[x - 1], data[x], data[x + 1],
            data[x + width - 1], data[x + width], data[x + width + 1]);
    }

    inline int32_t MedianMsk5(int x) {
        return Median5(data[x - width],data[x - 1], data[x], data[x + 1],data[x + width]);
    }


    inline int32_t Accumulate33(int x,int y) {
        int32_t v=0;
        for (int dy = -1; dy <= 1; dy++) {
            int py = y + dy;
            if (py < 0) { continue; }
            if (py >= height) { break; }
            for (int dx = -1; dx <= 1; dx++) {
                int px = x + dx;
                if (px < 0) { continue; }
                if (px >= width) { break; }
                v += data[py * width + px];
            }
        }
        return v;
    }

     inline int32_t Accumulate8(int x,int y) {
        int32_t v=0;
        for (int dy = -1; dy <= 1; dy++) {
            int py = y + dy;
            if (py < 0) { continue; }
            if (py >= height) { break; }
            for (int dx = -1; dx <= 1; dx++) {
                if((dy==0)&&(dx==0)) { continue; }
                int px = x + dx;
                if (px < 0) { continue; }
                if (px >= width) { break; }
                v += data[py * width + px];
            }
        }
        return v;
    }


    const double* Gauss77Msk = new double[49]  {
          1.0,  6.0, 15.0, 20.0, 15.0,  6.0, 1.0,
          6.0, 36.0, 90.0,120.0, 90.0, 36.0, 6.0,
         15.0, 90.0,225.0,300.0,225.0, 90.0,15.0,
         20.0,120.0,300.0,400.0,300.0,120.0,20.0,
         15.0, 90.0,225.0,300.0,225.0, 90.0,15.0,
          6.0, 36.0, 90.0,120.0, 90.0, 36.0, 6.0,
          1.0,  6.0, 15.0, 20.0, 15.0,  6.0, 1.0};
  
    /**
    <summary>Calculates the value of a 3x3 Convolution/Filter. The convolution mask is given in w<summary>
    <attention> Do not call this function at the border of the image. </attention>
    <param name="x">the pixel index. This is y*width+x.</param>
    <param name="x">the weight of the mask. the order is from left-upper to right-lower</param>
    <returns>the convoljution value</returns>

    */
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
    /**
    <summary>The weights of the Laplace filter</summary>
    */
	const int32_t* Laplace =  new int32_t[9]{ 
        1, 1, 1, 
        1,-8, 1, 
        1, 1, 1 };
    /**
    <summary>The 4x4 Bayer ordered dither mask</summary>
    */
    const int32_t* BayerMsk44 = new int32_t[16]{
          15,143, 47,175,
         207, 79,239,111,
          63,191, 31,159,
         255,127,223, 95};
    /**
    <summary>The 8x8 Bayer ordered dither mask</summary>
    */
    const int32_t* BayerMsk88 = new int32_t[64]{
     4 *  0+3, 4 * 32+3, 4 * 8+3 , 4 * 40+3, 4 *  2+3, 4 * 34+3, 4 * 10+3, 4 * 42+3,
     4 * 48+3, 4 * 16+3, 4 * 56+3, 4 * 24+3, 4 * 50+3, 4 * 18+3, 4 * 58+3, 4 * 26+3,
     4 * 12+3, 4 * 44+3, 4 *  4+3, 4 * 36+3, 4 * 14+3, 4 * 46+3, 4 *  6+3, 4 * 38+3,
     4 * 60+3, 4 * 28+3, 4 * 52+3, 4 * 20+3, 4 * 62+3, 4 * 30+3, 4 * 54+3, 4 * 22+3,
     4 * 3+3 , 4 * 35+3, 4 * 11+3, 4 * 43+3, 4 *  1+3, 4 * 33+3, 4 *  9+3, 4 * 41+3,
     4 * 51+3, 4 * 19+3, 4 * 59+3, 4 * 27+3, 4 * 49+3, 4 * 17+3, 4 * 57+3, 4 * 25+3,
     4 * 15+3, 4 * 47+3, 4 *  7+3, 4 * 39+3, 4 * 13+3, 4 * 45+3, 4 *  5+3, 4 * 37+3,
     4 * 63+3, 4 * 31+3, 4 * 55+3, 4 * 23+3, 4 * 61+3, 4 * 29+3, 4 * 53+3, 4 * 21+3 };
    /**
    <summary>
    The diffusion coefficients for the Ostromoukhov-Algorithm. 
    The first line (4 values) is for Gray==0, the second for Gray==1 .....
    The last value in a line is the sum. The values before must be divided by this value. This entry is
    redundant, but it is specified in the implementation of Victor Ostromoukhov.
    </summary>
    */
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

