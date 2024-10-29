#pragma once
// Tomado de: http://www.lighthouse3d.com/cg-topics/code-samples/opengl-3-3-glsl-1-5-sample/

#define M_PI       3.14159265358979323846f
#define TWO_PI     2.f*M_PI

void crossProductVector3(float *a, float *b, float *res);

float dotProductVector3(float* a, float* b);

void normalizeVector3(float *a);

void subtractVector3(float* a, float* b, float* result);

void setIdentityMatrix(float *mat, int size);

void multMatrix4x4(float *a, float *b);

void orthoMatrix4x4(
	const float& b, const float& t, const float& l, const float& r, 
	const float& n, const float& f, float(&M)[4][4]);

void lookAtRH(float* eye, float* center, float* up, float(&M)[4][4]);

void setPerspectiveBTLR(
	const float& angleOfView, const float& imageAspectRatio,
	const float& n, const float& f,
	float& b, float& t, float& l, float& r);

void setFrustum4x4(
	const float& b, const float& t, const float& l, const float& r,
	const float& n, const float& f,
	float(&M)[4][4]);

