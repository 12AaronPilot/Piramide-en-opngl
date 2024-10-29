#include "mathfunc.h"
#include <cmath>
#include <cstring>


void crossProductVector3(float *a, float *b, float *res) 
{
	res[0] = a[1] * b[2] - b[1] * a[2];
	res[1] = a[2] * b[0] - b[2] * a[0];
	res[2] = a[0] * b[1] - b[0] * a[1];
}

float dotProductVector3(float* a, float* b)
{
	return a[0] * b[0] + a[1] * b[1] + a[2] * b[2];
}

void normalizeVector3(float *a) 
{
	float mag = sqrtf(a[0] * a[0] + a[1] * a[1] + a[2] * a[2]);

	a[0] /= mag;
	a[1] /= mag;
	a[2] /= mag;
}

void setIdentityMatrix(float *mat, int size) 
{
	for (int i = 0; i < size * size; ++i)
		mat[i] = 0.0f;

	for (int i = 0; i < size; ++i)
		mat[i + i * size] = 1.0f;
}

void multMatrix4x4(float *a, float *b) 
{
	float res[16];

	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			res[j * 4 + i] = 0.0f;
			for (int k = 0; k < 4; ++k) {
				res[j * 4 + i] += a[k * 4 + i] * b[j * 4 + k];
			}
		}
	}
	std::memcpy(a, res, 16 * sizeof(float));
}

void orthoMatrix4x4(	const float& b, const float& t,
							const float& l, const float& r,
							const float& n, const float& f,
	float(&M)[4][4])
{
	// set OpenGL perspective projection matrix
	M[0][0] = 2 / (r - l);
	M[0][1] = 0;
	M[0][2] = 0;
	M[0][3] = 0;

	M[1][0] = 0;
	M[1][1] = 2 / (t - b);
	M[1][2] = 0;
	M[1][3] = 0;

	M[2][0] = 0;
	M[2][1] = 0;
	M[2][2] = -2 / (f - n);
	M[2][3] = 0;

	M[3][0] = -(r + l) / (r - l);
	M[3][1] = -(t + b) / (t - b);
	M[3][2] = -(f + n) / (f - n);
	M[3][3] = 1;
}

void lookAtRH(float *eye, float *center, float *up, float(&M)[4][4])
{
	float f[3];
	subtractVector3(center, eye, f);
	normalizeVector3(f);

	float s[3];
	crossProductVector3(f, up, s);
	normalizeVector3(s);

	float u[3];
	crossProductVector3(s, f, u);

	M[0][0] = s[0];
	M[1][0] = s[1];
	M[2][0] = s[2];
	M[0][1] = u[0];
	M[1][1] = u[1];
	M[2][1] = u[2];
	M[0][2] = -f[0];
	M[1][2] = -f[1];
	M[2][2] = -f[2];

	M[0][3] = 0.f;
	M[1][3] = 0.f;
	M[2][3] = 0.f;
	M[3][3] = 1.f;

	M[3][0] = -dotProductVector3(s, eye);
	M[3][1] = -dotProductVector3(u, eye);
	M[3][2] = dotProductVector3(f, eye);
}

void subtractVector3(float* a, float* b, float* result) 
{
	result[0] = a[0] - b[0];
	result[1] = a[1] - b[1];
	result[2] = a[2] - b[2];
}

void setPerspectiveBTLR(
	const float& angleOfView, const float& imageAspectRatio,
	const float& n, const float& f,
	float& b, float& t, float& l, float& r) // out <--
{
	float scale = tanf(angleOfView * 0.5f * M_PI / 180.f) * n;
	r = imageAspectRatio * scale, l = -r;
	t = scale, b = -t;
}

void setFrustum4x4(
	const float& b, const float& t, const float& l, const float& r,
	const float& n, const float& f,
	float(&M)[4][4])
{
	M[0][0] = 2 * n / (r - l);
	M[0][1] = 0;
	M[0][2] = 0;
	M[0][3] = 0;

	M[1][0] = 0;
	M[1][1] = 2 * n / (t - b);
	M[1][2] = 0;
	M[1][3] = 0;

	M[2][0] = (r + l) / (r - l);
	M[2][1] = (t + b) / (t - b);
	M[2][2] = -(f + n) / (f - n);
	M[2][3] = -1;

	M[3][0] = 0;
	M[3][1] = 0;
	M[3][2] = -2 * f * n / (f - n);
	M[3][3] = 0;
}