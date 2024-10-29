#pragma once
#include <string>
#include <vector>
#include "opengl.h"

class CMeshBase;

enum class eBufferType {
	POSITION = 0,
	COLOR,
	NORMAL,
	TEXTURE
};

class CShaderBase
{
public:
	CShaderBase();
	~CShaderBase();
	void initializeShader(std::string vertexShaderFile, std::string pixelShaderFile);
	virtual void initializeParticularShader() { }

	void setCamera(float viewMatrix[16], float projMatrix[16]);
	void setWorldMatrix(float worldMatrix[16]);

	virtual void setUniforms() { }

	virtual void setLightPosition(float x, float y, float z) { }

	GLuint getShaderProgram() { return shader_programme; }


	//void addMesh(CMeshBase *mesh);
	//std::vector<CMeshBase *> getMeshVector() { return mvMeshes; }

	//virtual GLuint getShaderBuffer(eBufferType bufferType) { return -1; }

	virtual GLuint getPosLoc() { return -1; }
	virtual GLuint getColorLoc() { return -1; }
	virtual GLuint getUVLoc() { return -1; }

	virtual GLuint getNormalLoc() { return -1; }
	virtual GLuint getTangentLoc() { return -1; }
	virtual GLuint getBinormalLoc() { return -1; }

protected:
	virtual void setParticularUniforms() { }

private:
	bool verifyCompiledShader(const GLuint & shader);
	bool verifyLinkedProgram(const GLuint &sprogram);

private:

	GLuint shader_programme;

	GLuint projMatrixLoc, viewMatrixLoc;
	GLuint worldMatrixLoc;

	//std::vector<CMeshBase *> mvMeshes;

};
