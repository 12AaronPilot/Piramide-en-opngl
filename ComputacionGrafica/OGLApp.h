#pragma once
#include "WinApplication.h"
#include "opengl.h"
#include "ShaderDirect.h"
#include "GameTimer.h"
#include "mathfunc.h"

class CShaderBase;

class OGLApp : public WinApplication
{
private:

	GameTimer timer;

	float rotAng;
	float rotAngEye;

	bool AppBegin() override;
	bool AppUpdate() override;
	bool AppEnd() override;

	bool Size(HWND AppHwnd, WPARAM Wparam, LPARAM Lparam) override;

	bool DisplayChange(HWND AppHwnd, WPARAM Wparam, LPARAM Lparam) override;

	bool InitializeScene();
	bool InitializeOpenGL(int screenWidth, int screenHeight, bool vsync);

	void BuildObjects();
	void ReleaseObjects();
	void ProcessInput(float elapsedTime);

public:

	OGLApp();
	virtual ~OGLApp();

private:
	HDC m_deviceContext;
	HGLRC m_renderingContext;

	RECT rc;

	unsigned int scWidth, scHeight;

	CShaderBase* directShader;

	int vertxCount;

	GLuint vao;
	GLuint vbo;
	GLuint vboColor;

	float identityMatrix[16];
	float projectionMM[16];
	float viewMM[16];
};