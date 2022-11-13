#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstring>

using std::string;
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::stringstream;

static unsigned compileShader(unsigned type, const string& source)
{
	unsigned id = glCreateShader(type);
	const char* src = source.c_str();
	glShaderSource(id, 1, &src, nullptr);
	glCompileShader(id);

	int result;
	glGetShaderiv(id, GL_COMPILE_STATUS, &result);
	if (result == GL_FALSE) {
		int length;
		glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
		char* message = (char*)_malloca(length * sizeof(char));
		glGetShaderInfoLog(id, length, &length, message);
		cout << "Fail to compile " <<
			(type == GL_VERTEX_SHADER ? "vertex" : "fragment")
			<< " shader!" << endl;//输出错误信息
		cout << message << endl;//输出错误信息
		glDeleteShader(id);
	}

	return id;
}

static unsigned createShader(const string& path_vertex_shader,
	const string& path_fragment_shader,
	const string& path_geometry_shader = "")
{
	unsigned program = glCreateProgram();
	ifstream fout;
	fout.open(path_vertex_shader, ios::in);
	if (!fout.is_open()) {
		cout << "Fail to open " << path_vertex_shader << endl;
		return -1;
	}

	stringstream buffer1;
	buffer1 << fout.rdbuf();
	string vertex(buffer1.str());
	buffer1.clear();
	fout.close();

	fout.open(path_fragment_shader, ios::in);
	if (!fout.is_open()) {
		cout << "Fail to open " << path_fragment_shader << endl;
		return -1;
	}

	stringstream buffer2;
	buffer2 << fout.rdbuf();
	string fragment(buffer2.str());
	buffer2.clear();
	fout.close();

	bool flag = !(path_geometry_shader == "");
	string geometry;
	if (flag) {
		fout.open(path_geometry_shader, ios::in);
		if (!fout.is_open()) {
			cout << "Fail to open " << path_geometry_shader << endl;
			return -1;
		}

		stringstream buffer3;
		buffer3 << fout.rdbuf();
		geometry = buffer3.str();
		buffer3.clear();
		fout.close();
	}

	unsigned vs = compileShader(GL_VERTEX_SHADER, vertex);
	unsigned fs = compileShader(GL_FRAGMENT_SHADER, fragment);
	
	glAttachShader(program, vs);
	glAttachShader(program, fs);
	
	if (flag) {
		unsigned gs = compileShader(GL_GEOMETRY_SHADER, geometry);
		glAttachShader(program, gs);
		glDeleteShader(gs);
	}

	glLinkProgram(program);
	glValidateProgram(program);

	glDeleteShader(vs);
	glDeleteShader(fs);

	return program;
}

//关于着色器的抽象
class shader
{
public:
	shader(const string& path_vertex_shader,
		const string& path_fragment_shader,
		const string& path_geometry_shader = "");
	void use();
	void setBool(const string& name, bool value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
	}
	void setInt(const string& name, int value) const
	{
		glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setFloat(const string& name, float value) const
	{
		glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
	}
	void setVec2(const string& name, const glm::vec2& value) const
	{
		glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec2(const string& name, float x, float y) const
	{
		glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
	}
	void setVec3(const string& name, const glm::vec3& value) const
	{
		glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec3(const string& name, float x, float y, float z) const
	{
		glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
	}
	void setVec4(const string& name, const glm::vec4& value) const
	{
		glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
	}
	void setVec4(const string& name, float x, float y, float z, float w)
	{
		glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
	}
	void setMat2(const string& name, const glm::mat2& mat) const
	{
		glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat3(const string& name, const glm::mat3& mat) const
	{
		glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
	void setMat4(const string& name, const glm::mat4& mat) const
	{
		glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
	}
private:
	unsigned ID;
};

inline shader::shader(const string& path_vertex_shader,
	const string& path_fragment_shader,
	const string& path_geometry_shader)
{
	this->ID = createShader(path_vertex_shader, path_fragment_shader, path_geometry_shader);
}

void shader::use()
{
	glUseProgram(ID);
}


