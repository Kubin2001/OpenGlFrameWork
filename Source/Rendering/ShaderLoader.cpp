#include "ShaderLoader.h"

#include <print>
#include <fstream>
#include <sstream>

std::string LoadShaderFile(const char* fileName) {
    std::ifstream file;
    std::stringstream buf;

    std::string ret = "";
    file.open(fileName);
    if (file.is_open()) {
        buf << file.rdbuf();
        ret = buf.str();
        std::println("Shader : {} loaded ", fileName);
    }
    else{
        std::println("Error file: {} not openned ", fileName);
    }
    file.close();

    return ret;
}

void ShaderLoader::LoadShader(const std::string& name, const std::string& path, GLenum shaderType) {
    //Compile Fragment Shader
    unsigned int shaderID;
    shaderID = glCreateShader(shaderType);
    std::string shaderSrc = LoadShaderFile(path.c_str());
    const GLchar* shader = shaderSrc.c_str();
    glShaderSource(shaderID, 1, &shader, nullptr);

    glCompileShader(shaderID);

    //catch compile error
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::println("Error with shader compilation NAME: {} ERROR:{}", name,infoLog);
    }
    else{
        shaders[name] = shaderID;
        std::println("Shader Compilation succesfull: {}",name);
    }
}

void ShaderLoader::LoadShaderStr(const std::string& name, const std::string& shaderText, GLenum shaderType) {
    //Compile Fragment Shader
    unsigned int shaderID;
    shaderID = glCreateShader(shaderType);
    std::string shaderSrc = shaderText;
    const GLchar* shader = shaderSrc.c_str();
    glShaderSource(shaderID, 1, &shader, nullptr);

    glCompileShader(shaderID);

    //catch compile error
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::println("Error with shader compilation NAME: {} ERROR:{}", name, infoLog);
    }
    else {
        shaders[name] = shaderID;
        std::println("Shader Compilation succesfull: {}", name);
    }
}

unsigned int& ShaderLoader::GetShader(const std::string& name) {
    if (shaders.find(name) != shaders.end()) {
        return shaders[name];
    }
    else {
        throw std::runtime_error("Shader: " + name + " not found");
    }
}


void ShaderLoader::CreateProgram(std::vector<std::string> &names, const std::string& programName, bool deleteShader) {
    for (auto& it : names) {
        if (shaders.find(it) == shaders.end()) {
            std::println("Shader {} not found program creation stopped", it);
            return;
        }
    }
   
    unsigned int shaderProgram = 0;

    shaderProgram = glCreateProgram();
    shaderPrograms[programName] = shaderProgram;
    for (auto& it : names) {
        glAttachShader(shaderPrograms[programName], ShaderLoader::GetShader(it));
    }
    glLinkProgram(shaderPrograms[programName]);

    //catch error
    glGetProgramiv(shaderPrograms[programName], GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderPrograms[programName], 512, nullptr, infoLog);
        std::println ("Cannot create shader program: {} ERROR: {}",programName,infoLog);
    }
    else{
        std::println("Linking succesfull");
    }

    //Usuwanie shaderów bo z racji po³¹czenia w program s¹ niepotrzebne
    if (deleteShader) {
        for (auto& it : names) {
            glDeleteShader(ShaderLoader::GetShader(it));
            shaders.erase(it);
        }
    }

}

unsigned int ShaderLoader::LoadShaderStrRaw(const char* shaderText, GLenum shaderType) {
    //Compile Shader
    unsigned int shaderID;
    shaderID = glCreateShader(shaderType);
    const GLchar* shader = shaderText;
    glShaderSource(shaderID, 1, &shader, nullptr);

    glCompileShader(shaderID);

    //catch compile error
    glGetShaderiv(shaderID, GL_COMPILE_STATUS, &success);

    if (!success) {
        glGetShaderInfoLog(shaderID, 512, nullptr, infoLog);
        std::println("Error with shader compilation  ERROR: {} in \n {}", infoLog, shaderText);
        return -1;
    }
    std::println("Raw Shader Compilation succesfull");
    return shaderID;
}

void ShaderLoader::CreateProgramStr(const std::string name, const char* vertexStr, const char* fragmentStr) {
    unsigned int vertex = LoadShaderStrRaw(vertexStr, GL_VERTEX_SHADER);
    unsigned int fragment = LoadShaderStrRaw(fragmentStr, GL_FRAGMENT_SHADER);

    unsigned int shaderProgram = 0;

    shaderProgram = glCreateProgram();
    shaderPrograms[name] = shaderProgram;
    glAttachShader(shaderPrograms[name], vertex);
    glAttachShader(shaderPrograms[name], fragment);

    glLinkProgram(shaderPrograms[name]);

    //catch error
    glGetProgramiv(shaderPrograms[name], GL_LINK_STATUS, &success);

    if (!success) {
        glGetProgramInfoLog(shaderPrograms[name], 512, nullptr, infoLog);
        std::println ("Cannot create shader program: {} ERROR: {}",name,infoLog);
    }
    else{
        std::println("Linking succesfull");
    }

    //Usuwanie shaderów bo z racji po³¹czenia w program s¹ niepotrzebne
    glDeleteShader(vertex);
    glDeleteShader(fragment);
}

unsigned int& ShaderLoader::GetProgram(const std::string& name) {
    if (shaderPrograms.find(name) != shaderPrograms.end()) {
        return shaderPrograms[name];
    }
    else {
        throw std::runtime_error("Shader Program: " +  name + " not found");
    }
}

bool ShaderLoader::IsProgram(const std::string& name) {
    if (shaderPrograms.find(name) == shaderPrograms.end()) {
        return false;
    }
    return true;
}


ShaderLoader::~ShaderLoader() {
    for (auto& [name, program] : shaderPrograms) {
        glDeleteProgram(program);
    }
    shaderPrograms.clear(); 
}