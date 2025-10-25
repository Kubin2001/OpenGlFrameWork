#pragma once

#include <unordered_map>
#include <string>
#include <vector>

#include "glad\glad.h"


class ShaderLoader {
	private:
		int success;
		char infoLog[512];
		std::unordered_map<std::string,unsigned int> shaders;
		std::unordered_map<std::string, unsigned int> shaderPrograms;

	public:
		void LoadShader(const std::string& name, const std::string& path, GLenum shaderType);

		void LoadShaderStr(const std::string& name, const std::string& shaderText, GLenum shaderType);

		unsigned int& GetShader(const std::string& name);

		void CreateProgram(std::vector<std::string>& names, const std::string& programName, bool deleteShader = false);

		unsigned int LoadShaderStrRaw(const char* shaderText, GLenum shaderType);

		void CreateProgramStr(const std::string name, const char* vertexStr, const char* fragmentStr);

		unsigned int& GetProgram(const std::string& name);

		bool IsProgram(const std::string& name);

		~ShaderLoader();
};
