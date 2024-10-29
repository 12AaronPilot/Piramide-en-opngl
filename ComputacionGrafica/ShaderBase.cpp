#include "ShaderBase.h"
#include <fstream>
#include <vector>
//#include "../meshes/CMeshBase.h"

using namespace std;

CShaderBase::CShaderBase()
{
}

CShaderBase::~CShaderBase()
{
	//for (CMeshBase *cubo : mvMeshes) {
	//	delete cubo; cubo = nullptr;
	//}
}

void CShaderBase::initializeShader(string vertexShaderFile, string pixelShaderFile)
{
	std::ifstream vfs(vertexShaderFile), pfs(pixelShaderFile);
	std::string vertexs((std::istreambuf_iterator<char>(vfs)),
		(std::istreambuf_iterator<char>()));
	std::string fragxs((std::istreambuf_iterator<char>(pfs)),
		(std::istreambuf_iterator<char>()));

	const char *vertex_shader = vertexs.c_str();
	const char *fragment_shader = fragxs.c_str();

	GLuint vert_shader, frag_shader;

	vert_shader = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vert_shader, 1, &vertex_shader, NULL);
	glCompileShader(vert_shader);

	if (!verifyCompiledShader(vert_shader)) { return; }

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(frag_shader, 1, &fragment_shader, NULL);
	glCompileShader(frag_shader);

	if (!verifyCompiledShader(frag_shader)) { return; }

	shader_programme = glCreateProgram();
	glAttachShader(shader_programme, frag_shader);
	glAttachShader(shader_programme, vert_shader);

	glLinkProgram(shader_programme);

	verifyLinkedProgram(shader_programme);

	projMatrixLoc = glGetUniformLocation(shader_programme, "projMatrix");
	viewMatrixLoc = glGetUniformLocation(shader_programme, "viewMatrix");
	worldMatrixLoc = glGetUniformLocation(shader_programme, "worldMatrix");

	initializeParticularShader();
}

//void CShaderBase::addMesh(CMeshBase *mesh)
//{
//	mvMeshes.push_back(mesh);
//}

bool CShaderBase::verifyCompiledShader(const GLuint &shader)
{
	GLint isCompiled = 0;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &isCompiled);

	if (isCompiled == GL_FALSE) {
		GLint maxLength = 0;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> errorLog(maxLength);
		glGetShaderInfoLog(shader, maxLength, &maxLength, &errorLog[0]);
		std::string errcd(errorLog.begin(), errorLog.end());
		glDeleteShader(shader);
		return false;
	}
	return true;
}

bool CShaderBase::verifyLinkedProgram(const GLuint &sprogram)
{
	GLint isLinked = 0;
	glGetProgramiv(sprogram, GL_LINK_STATUS, &isLinked);
	if (isLinked == GL_FALSE) {
		GLint maxLength = 0;
		glGetProgramiv(sprogram, GL_INFO_LOG_LENGTH, &maxLength);
		std::vector<char> infoLog(maxLength);
		glGetProgramInfoLog(sprogram, maxLength, &maxLength, &infoLog[0]);
		glDeleteProgram(sprogram);
		return false;
	}
	return true;
}

void CShaderBase::setCamera(float viewMatrix[16], float projMatrix[16])
{
	glUniformMatrix4fv(projMatrixLoc, 1, GL_FALSE, projMatrix);
	glUniformMatrix4fv(viewMatrixLoc, 1, GL_FALSE, viewMatrix);
}

void CShaderBase::setWorldMatrix(float worldMatrix[16])
{
	glUniformMatrix4fv(worldMatrixLoc, 1, GL_FALSE, worldMatrix);
}


