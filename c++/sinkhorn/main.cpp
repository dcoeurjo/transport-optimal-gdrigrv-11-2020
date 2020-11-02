#define _CRT_SECURE_NO_WARNINGS 1
#include <vector>
#include <cstring>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define EPSILON 1E-250

template<typename T> T sqr(const T x) { return x*x; };


class Gaussian2DKernel {
public:
	Gaussian2DKernel(double regularization = 25., int W = 0, int H = 0) {
		gamma = regularization;
		W_ = W;
		H_ = H;
		N = W_*H_;
		kernel1d = new double[std::max(W, H)];
		for (int i=0; i<std::max(W, H); i++) {
			kernel1d[i] = std::max(EPSILON, exp(-i*i / gamma));
		}
	}

	Gaussian2DKernel(const Gaussian2DKernel& b) {
		gamma = b.gamma;
		W_ = b.W_;
		H_ = b.H_;
		N = W_*H_;
		kernel1d = new double[std::max(W_, H_)];
		std::memcpy(kernel1d, b.kernel1d, std::max(W_, H_)*sizeof(double));
	}

	Gaussian2DKernel& operator=(const Gaussian2DKernel& b) {
		gamma = b.gamma;
		W_ = b.W_;
		H_ = b.H_;
		N = W_*H_;
		kernel1d = new double[std::max(W_, H_)];
		std::memcpy(kernel1d, b.kernel1d, std::max(W_, H_)*sizeof(double));
		return *this;
	}

	~Gaussian2DKernel() {
		delete[] kernel1d;
	}

	double operator()(int i, int j) {
		int x0 = i%W_;
		int y0 = i/W_;
		int x1 = j%W_;
		int y1 = j/W_;
		return std::max(EPSILON, exp(-(sqr(x0-x1)+sqr(y0-y1))/gamma));
	}

	void convolve(const double* u, double* result) const {
		double* tmp = new double[W_*H_]; // allocating here ; otherwise not thread-safe

		for (int i=0; i<H_; i++) {
			for (int j=0; j<W_; j++) {
				double conv = 0;
				for (int k=0; k<W_; k++) {
					conv+=kernel1d[abs(j-k)]*u[i*W_ + k];
				}
				tmp[i+j*H_] = conv;
			}
		}

		for (int j=0; j<W_; j++) {
			for (int i=0; i<H_; i++) {
				double conv = 0;
				for (int k=0; k<H_; k++) {
					conv+=kernel1d[abs(i-k)]*tmp[k + j*H_];
				}
				result[i*W_+j] = conv;
			}
		}
		delete[] tmp;
	}

	void convolveAdjoint(const double* u, double* result) const {
		convolve(u, result);
	}

	double gamma;
	int W_, H_;
	int N;
	double* kernel1d;
};


int main() {

	int W, H, C;
	
	unsigned char *image = stbi_load("evol1.bmp", &W, &H, &C, 1);
	std::vector<double> image_double(W*H);
	for (int i=0; i<W*H; i++)
		image_double[i] = image[i];
	
	std::vector<unsigned char> image_result(W*H, 0);
	for (int i = 0; i < H; i++) {
		for (int j = 0; j < W; j++) {
			image_result[i*W + j] = image_double[i*W+j]*0.5;
		}
	}
	stbi_write_png("image.png", W, H, 1, &image_result[0], 0);

	return 0;
}