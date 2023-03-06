/**
 * A shader class based on the LearnOpenGL website.
 * https://learnopengl.com/Getting-started/Shaders
 */

#pragma once

#include <glad/glad.h>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>


class Shader
{
private:
	unsigned int id;

public:
	Shader(const std::string& vert_path, const std::string& frag_path)
	{
		/* retreive the shader source code from the file paths */
		std::string vert_code_str;
		std::string frag_code_str;

		if (std::ifstream vert_fin{ vert_path }) {
			std::stringstream vert_ss;
			vert_ss << vert_fin.rdbuf();
			vert_code_str = vert_ss.str();
		} else {
			std::cerr << "could not read vertex shader" << std::endl;
		}

		if (std::ifstream frag_fin{ frag_path }) {
			std::stringstream frag_ss;
			frag_ss << frag_fin.rdbuf();
			frag_code_str = frag_ss.str();
		} else {
			std::cerr << "could not read fragment shader" << std::endl;
		}

		const char* vert_code = vert_code_str.c_str();
		const char* frag_code = frag_code_str.c_str();

		/* compiler shaders */
		unsigned int vert_shader, frag_shader;
		int success;

		vert_shader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vert_shader, 1, &vert_code, nullptr);
		glCompileShader(vert_shader);
		glGetShaderiv(vert_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			std::cerr << "could not compile vertex shader" << std::endl;
		}

		frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(frag_shader, 1, &frag_code, nullptr);
		glCompileShader(frag_shader);
		glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
		if (!success) {
			std::cerr << "could not compile fragment shader" << std::endl;
		}

		/* link shaders into program */
		id = glCreateProgram();
		glAttachShader(id, vert_shader);
		glAttachShader(id, frag_shader);
		glLinkProgram(id);
		glGetProgramiv(id, GL_LINK_STATUS, &success);
		if (!success) {
			std::cerr << "could not link shaders into program" << std::endl;
		}

		/* delete shaders */
		glDeleteShader(vert_shader);
		glDeleteShader(frag_shader);
	}

	void use() const { glUseProgram(id); }

	/* functions to set uniforms */
	void setBool(const std::string& name, GLboolean value) const {
		glUniform1ui(glGetUniformLocation(id, name.c_str()), static_cast<GLuint>(value));
	}
	void setUnsignedInt(const std::string& name, GLuint value) const {
		glUniform1ui(glGetUniformLocation(id, name.c_str()), value);
	}
	void setInt(const std::string& name, GLint value) const {
		glUniform1i(glGetUniformLocation(id, name.c_str()), value);
	}
	void setFloat(const std::string& name, GLfloat value) const {
		glUniform1f(glGetUniformLocation(id, name.c_str()), value);
	}
	void setDouble(const std::string& name, GLdouble value) const {
		glUniform1d(glGetUniformLocation(id, name.c_str()), value);
	}
};