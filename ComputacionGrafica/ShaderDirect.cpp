#include "ShaderDirect.h"

using namespace std;

CShaderDirect::CShaderDirect()
{
}

CShaderDirect::~CShaderDirect()
{
}

void CShaderDirect::initializeParticularShader()
{
	GLuint programme = getShaderProgram();

	positionLoc = glGetAttribLocation(programme, "in_Position");

	colorLoc = glGetAttribLocation(programme, "in_Color");

}
