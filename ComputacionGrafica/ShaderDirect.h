#pragma once
#include <string>
#include "ShaderBase.h"

class CShaderDirect: public CShaderBase
{
public:
	CShaderDirect();
	~CShaderDirect();

	void initializeParticularShader() override;

	GLuint getPosLoc() override { return positionLoc; }
	GLuint getColorLoc() override { return colorLoc; }

private:

	GLuint positionLoc;
	GLuint colorLoc;

};

