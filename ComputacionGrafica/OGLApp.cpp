#include "OGLApp.h"
#include <iostream>
#include "ShaderDirect.h"

OGLApp::OGLApp()
{
}

OGLApp::~OGLApp()
{
}

bool OGLApp::AppBegin()
{
	unsigned int w = 0, h = 0;
	GetWindowDims(w, h);

	std::cout << "iniciando " << w << ", " << h << std::endl;

	InitializeScene();
	InitializeOpenGL(w,h,false);

	BuildObjects();

	timer.Reset();

	return true;
}

bool OGLApp::AppUpdate()
{
	timer.Tick();
	float elapsedTime = timer.DeltaTime();


	RECT rc;
	GetClientRect(GetAppHwnd(), &rc);

	float identitymm[16]{
		1.f, 0.f, 0.f, 0.f,
		0.f, 1.f, 0.f, 0.f,
		0.f, 0.f, 1.f, 0.f,
		0.f, 0.f, 0.f, 1.f };

	rotAng += 3.1415926f / 180.f * (50.f * elapsedTime);
	rotAngEye += 3.1415926f / 180.f * (0.f * elapsedTime);


	float rotationMatrix[16] = {
		cosf(rotAng),   0.f, sin(rotAng),   0.f,
		0.f,		    1.f, 0.f,		    0.f,
		-sinf(rotAng),  0.f, cosf(rotAng),  0.f,
		0.f,			0.f, 0.f,			1.f
	};

	float perspectiveMatrix[4][4]{ 0 };
	orthoMatrix4x4(-1.0, 1.0, -1.3333f, 1.3333f, -0.01f, 100.f, perspectiveMatrix);

	float viewMatrix[4][4]{ 0 };
	float eye[3]	  { -sinf(rotAngEye), 0.5f, 2.f * cosf(rotAngEye) };
	float center[3]   { 0.f, 0.f, 0.f };
	float up[3]       { 0.f, 1.f, 0.f };

	lookAtRH(eye, center, up, viewMatrix);

	float* viewPtr = reinterpret_cast<float*>(viewMatrix);
	float* perspPtr = reinterpret_cast<float*>(perspectiveMatrix);

	glViewport(0, 0, rc.right - rc.left, rc.bottom - rc.top);
	glClearColor(0.3921, 0.5843, 0.9294f, 1.0f); //cornflower blue
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(directShader->getShaderProgram());

	directShader->setWorldMatrix(rotationMatrix);
	directShader->setCamera(viewPtr, perspPtr);

	glBindVertexArray(vao);
	glDrawArrays(GL_TRIANGLES, 0, vertxCount);

	glFlush();
	SwapBuffers(m_deviceContext);
	
	return false;
}

bool OGLApp::AppEnd()
{
	return false;
}

bool OGLApp::Size(HWND AppHwnd, WPARAM Wparam, LPARAM Lparam)
{
	return false;
}

bool OGLApp::DisplayChange(HWND AppHwnd, WPARAM Wparam, LPARAM Lparam)
{
	return false;
}

bool OGLApp::InitializeScene()
{
	bool result;
	HWND fakeWND = CreateWindow(
		GetLpszClassName(), L"Fake Window",
		WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
		0, 0,
		1, 1,
		NULL, NULL,
		GetAppInstance(), NULL);

	auto effr = GetLastError();
	HDC fakeDC = GetDC(fakeWND);
	if (!fakeDC) {
		return false;
	}

	PIXELFORMATDESCRIPTOR fakePFD;
	ZeroMemory(&fakePFD, sizeof(fakePFD));
	fakePFD.nSize = sizeof(fakePFD);
	fakePFD.nVersion = 1;
	fakePFD.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	fakePFD.iPixelType = PFD_TYPE_RGBA;
	fakePFD.cColorBits = 32;
	fakePFD.cAlphaBits = 8;
	fakePFD.cDepthBits = 24;
	int fakePFDID = ChoosePixelFormat(fakeDC, &fakePFD);
	if (fakePFDID == 0) {
		return false;
	}

	// temporary begin
	{
		int error = SetPixelFormat(fakeDC, fakePFDID, &fakePFD);
		if (error != 1) {
			return false;
		}

		HGLRC fakeRC = wglCreateContext(fakeDC);
		if (!fakeRC) {
			return false;
		}

		error = wglMakeCurrent(fakeDC, fakeRC);
		if (error != 1) {
			return false;
		}

		result = LoadExtensionList();
		if (!result) {
			return false;
		}

		wglMakeCurrent(NULL, NULL);
		wglDeleteContext(fakeRC);
		fakeRC = NULL;
	}
	// Temporary end
	ReleaseDC(fakeWND, fakeDC);
	fakeDC = 0;

	return true;
}

bool OGLApp::InitializeOpenGL(int screenWidth, int screenHeight, bool vsync)
{
	int result;

	m_deviceContext = GetDC(GetAppHwnd());
	if (!m_deviceContext) {
		return false;
	}

	int attributeListInt[19];
	attributeListInt[0] = WGL_SUPPORT_OPENGL_ARB;
	attributeListInt[1] = TRUE;

	attributeListInt[2] = WGL_DRAW_TO_WINDOW_ARB;
	attributeListInt[3] = TRUE;

	attributeListInt[4] = WGL_ACCELERATION_ARB;
	attributeListInt[5] = WGL_FULL_ACCELERATION_ARB;

	attributeListInt[6] = WGL_COLOR_BITS_ARB;
	attributeListInt[7] = 24;

	attributeListInt[8] = WGL_DEPTH_BITS_ARB;
	attributeListInt[9] = 24;

	attributeListInt[10] = WGL_DOUBLE_BUFFER_ARB;
	attributeListInt[11] = TRUE;

	attributeListInt[12] = WGL_SWAP_METHOD_ARB;
	attributeListInt[13] = WGL_SWAP_EXCHANGE_ARB;

	attributeListInt[14] = WGL_PIXEL_TYPE_ARB;
	attributeListInt[15] = WGL_TYPE_RGBA_ARB;

	attributeListInt[16] = WGL_STENCIL_BITS_ARB;
	attributeListInt[17] = 8;

	attributeListInt[18] = 0;

	unsigned int formatCount;
	int pixelFormat[1];
	result = wglChoosePixelFormatARB(m_deviceContext, attributeListInt, NULL, 1, pixelFormat, &formatCount);
	if (result != 1) {
		return false;
	}

	PIXELFORMATDESCRIPTOR pixelFormatDescriptor;
	result = SetPixelFormat(m_deviceContext, pixelFormat[0], &pixelFormatDescriptor);
	if (result != 1) {
		auto effr = GetLastError();
		return false;
	}
	int attributeList[5];
	attributeList[0] = WGL_CONTEXT_MAJOR_VERSION_ARB;
	attributeList[1] = 3;
	attributeList[2] = WGL_CONTEXT_MINOR_VERSION_ARB;
	attributeList[3] = 3;
	attributeList[4] = 0;

	m_renderingContext = wglCreateContextAttribsARB(m_deviceContext, 0, attributeList);
	if (m_renderingContext == NULL) {
		return false;
	}
	result = wglMakeCurrent(m_deviceContext, m_renderingContext);
	if (result != 1) {
		return false;
	}
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	//glFrontFace(GL_CW);

	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);

	char* vendorString = (char*)glGetString(GL_VENDOR);
	char* rendererString = (char*)glGetString(GL_RENDERER);
	char* versionString = (char*)glGetString(GL_VERSION);

	std::cout << vendorString << "-->" << rendererString
		<< " / " << versionString << std::endl;

	if (vsync) { result = wglSwapIntervalEXT(1); }
	else { result = wglSwapIntervalEXT(0); }

	if (result != 1) { return false; }

	return true;
}

void OGLApp::BuildObjects()
{
	directShader = new CShaderDirect();
	directShader->initializeShader("data/color.vxs", "data/color.pxs");

	GLfloat g_vertex_buffer_data[] = {

		//frente
		-0.2f, -0.2f, 0.0f,
		 0.2f, -0.2f, 0.0f,
		-0.2f, 0.2f, 0.0f,

		-0.2f, 0.2f, 0.0f,
		0.2f, -0.2f, 0.0f,
		0.2f, 0.2f, 0.0f,

		//derecha
		0.2f, -0.2f, -0.0f,
		0.2f, -0.2f, -0.4f,
		0.2f, 0.2f, 0.0f,

		0.2f, -0.2f, -0.4f,
		0.2f, 0.2f, -0.4f,
		0.2f, 0.2f, 0.0f,

		//atras
		0.2f, 0.2f, -0.4f,
		-0.2f, -0.2f, -0.4f,
		-0.2f, 0.2f, -0.4f,

		0.2f, 0.2f, -0.4f,
		0.2f, -0.2f, -0.4f,
		-0.2f, -0.2f, -0.4f,

		//arriba
		0.2f, 0.2f, 0.0f,
		0.2f, 0.2f, -0.4f,
		-0.05f, 0.50f, -0.2f,

		-0.05f, 0.50f, -0.2f,
		0.2f, 0.2f, -0.4f,
		-0.2f, 0.2f, -0.4f,

		-0.05f, 0.50f, -0.2f,
		-0.2f, 0.2f, 0.0f,
		0.2f, 0.2f, 0.0f,

		-0.2f, 0.2f, -0.4f,
		-0.2f, 0.2f, 0.0f,
		-0.05f, 0.50f, -0.2f,

		//izquierda
		-0.2f, 0.2f, -0.0f,
		-0.2f, 0.2f, -0.4f,
		-0.2f, -0.2f, -0.0f,
		
		-0.2f, 0.2f, -0.4f,
		-0.2f, -0.2f, -0.4f,
		-0.2f, -0.2f, -0.0f,
	};

	GLfloat colores[]{
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,

		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,
		0.0f, 1.0f, 1.0f,

		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f,

		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,
		1.0f, 0.0f, 0.0f,

		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		0.0f, 0.0f, 1.0f,
		
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,
		0.0f, 1.0f, 0.0f,

		0.456f, 0.0f, 0.543f,
		0.456f, 0.0f, 0.543f,
		0.456f, 0.0f, 0.543f,

		1.0f, 0.321f, 0.0f,
		1.0f, 0.321f, 0.0f,
		1.0f, 0.321f, 0.0f,

		0.2f, 0.6f, 0.6f,
		0.2f, 0.6f, 0.6f,
		0.2f, 0.6f, 0.6f,

		0.3f, 0.2f, 0.2f,
		0.3f, 0.2f, 0.2f,
		0.3f, 0.2f, 0.2f,

		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,
		1.0f, 1.0f, 1.0f,

		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f,
	};

	int pointCount = sizeof(g_vertex_buffer_data) / sizeof(g_vertex_buffer_data[0]);
	int colorCount = sizeof(colores) / sizeof(colores[0]);

	vertxCount = static_cast<int>(pointCount / 3.f);

	glGenVertexArrays(1, &vao);
	glBindVertexArray(vao);

	glGenBuffers(1, &vbo);
	glGenBuffers(1, &vboColor);
	//-----------------------------------
	GLuint posLoc = directShader->getPosLoc();
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, pointCount * sizeof(GLfloat), g_vertex_buffer_data, GL_STATIC_DRAW);

	glVertexAttribPointer(posLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(posLoc);

	//----------------color
	GLuint colorLoc = directShader->getColorLoc();
	glBindBuffer(GL_ARRAY_BUFFER, vboColor);
	glBufferData(GL_ARRAY_BUFFER, colorCount * sizeof(GLfloat), colores, GL_STATIC_DRAW);

	glVertexAttribPointer(colorLoc, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(colorLoc);

}

void OGLApp::ReleaseObjects()
{
}

void OGLApp::ProcessInput(float elapsedTime)
{
}

