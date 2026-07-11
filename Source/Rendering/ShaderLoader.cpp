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
        return 0;
    }
    std::println("Raw Shader Compilation succesfull");
    return shaderID;
}

bool ShaderLoader::CreateProgramStr(const std::string name, const char* vertexStr, const char* fragmentStr) {
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

    bool created = false;

    if (!success) {
        glGetProgramInfoLog(shaderPrograms[name], 512, nullptr, infoLog);
        std::println ("Cannot create shader program: {} ERROR: {}",name,infoLog);
    }
    else{
        std::println("Linking succesfull");
        created = true;
    }

    //Usuwanie shaderów bo z racji po³¹czenia w program s¹ niepotrzebne
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    return created;
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

void ShaderLoader::LoadSavedShaders() {
    if (!IsProgram("RenderRect")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec2 aColorPacked;

        out vec4 ourColor;
        out float vAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            // Rozpakowanie koloru (bez zmian)
            vec2 vRG = unpackHalfColor(aColorPacked.x);
            vec2 vBA = unpackHalfColor(aColorPacked.y);
            ourColor = vec4(vRG, vBA);
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
            #version 330 core

            out vec4 FragColor;

            in vec4 ourColor;

            void main(){
	            FragColor = vec4(ourColor.xyz,1.0 * ourColor.w);
            }
        )glsl";

        CreateProgramStr("RenderRect", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderRectEx")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec3 aColorRot;
        layout(location = 2) in vec2 aRotCenter;

        out vec4 ourColor;
        out float vAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        vec2 unpackHalfColor(float packedColor){
            int col = int(round(packedColor)); 
            float r  = float((col >> 8) & 255); 
            float g = float(col & 255); 
            r /= 255.0;
            g /= 255.0;
            return vec2(r, g);
        }

        vec2 indexPos[6] = vec2[](
            vec2(0.0, 0.0), // Left Down
            vec2(0.0, 1.0), // Left Up
            vec2(1.0, 0.0), // Right Down
            vec2(0.0, 1.0), // Left Up
            vec2(1.0, 1.0), // Right Up
            vec2(1.0, 0.0)  // Right Down
        );

        void main() {
            vec2 normalizedOffset = indexPos[gl_VertexID % 6];

            vec2 absolutePos = aRect.xy + (normalizedOffset * aRect.zw);

            vec2 posRelToPivot = absolutePos - aRotCenter;

            float rad = radians(aColorRot.z);
            float cosA = cos(rad);
            float sinA = sin(rad);
    
            vec2 rotatedPos;
            rotatedPos.x = posRelToPivot.x * cosA - posRelToPivot.y * sinA;
            rotatedPos.y = posRelToPivot.x * sinA + posRelToPivot.y * cosA;

            vec2 worldPos = aRotCenter + rotatedPos;

            float ndcX = (worldPos.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (worldPos.y / uVievPort.y) * 2.0;

            gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);

            ourColor = vec4(unpackHalfColor(aColorRot.x), unpackHalfColor(aColorRot.y));
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 ourColor;

        void main(){
	        FragColor = vec4(ourColor.xyz,1.0 * ourColor.w);
        }
        )glsl";

        CreateProgramStr("RenderRectEx", vertexStr, fragmentStr);
    }


    if (!IsProgram("FlatRenderCopy")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;

        out vec2 oUV;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            oUV = uvs[gl_VertexID % 6];
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 oUV;

        uniform sampler2D texture1;

        void main(){
	        vec4 texcolor = texture(texture1,oUV);
	        FragColor = texcolor;
        }
        )glsl";

        CreateProgramStr("FlatRenderCopy", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderCopy")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in float aAlpha;

        out vec2 outTexCoord;
        out float outAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            outTexCoord = uvs[gl_VertexID % 6];

            outAlpha = aAlpha;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 outTexCoord;
        in float outAlpha;

        uniform sampler2D texture1;


        void main(){
	        vec4 texcolor = texture(texture1,outTexCoord);
	        texcolor.a *= outAlpha;
	        FragColor = texcolor;
        }
        )glsl";

        CreateProgramStr("RenderCopy", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderCopyPart")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aSourceRect;
        layout(location = 2) in float aAlpha;

        out vec2 outTexCoord;
        out float outAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];

            float u = aSourceRect.x + texUvs.x * aSourceRect.z;
    
            float v0 = 1.0 - aSourceRect.y - aSourceRect.w; 
            float v = v0 + texUvs.y * aSourceRect.w;        

            outTexCoord = vec2(u, v);
            outAlpha = aAlpha;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 outTexCoord;
        in float outAlpha;

        uniform sampler2D texture1;


        void main(){
	        vec4 texcolor = texture(texture1,outTexCoord);
	        texcolor.a *= outAlpha;
	        FragColor = texcolor;
        }
        )glsl";

        CreateProgramStr("RenderCopyPart", vertexStr, fragmentStr);
    }


    if (!IsProgram("RenderCopyEx")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aSourceRect;
        layout(location = 2) in vec2 aAlphaRot;
        layout(location = 3) in vec2 aRotCenter;


        out vec2 outTexCoord;
        out float outAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 1.0), // 0:
            vec2(0.0, 0.0), // 1
            vec2(1.0, 1.0), // 2: 
            vec2(0.0, 0.0), // 3: 
            vec2(1.0, 0.0), // 4: 
            vec2(1.0, 1.0)  // 5:
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, 0.0), // Left Down
            vec2(0.0, 1.0), // Left Up
            vec2(1.0, 0.0), // Right Down
            vec2(0.0, 1.0), // Left Up
            vec2(1.0, 1.0), // Right Up
            vec2(1.0, 0.0)  // Right Down
        );

        void main() {   
            vec2 localPos = indexPos[gl_VertexID % 6];
            vec2 absolutePos = aRect.xy + (localPos * aRect.zw);

            vec2 posRelToPivot = absolutePos - aRotCenter;

            float rad = radians(aAlphaRot.y);
            float cosA = cos(rad);
            float sinA = sin(rad);
    
            // 2D Rotation Matrix
            vec2 rotatedPos;
            rotatedPos.x = posRelToPivot.x * cosA - posRelToPivot.y * sinA;
            rotatedPos.y = posRelToPivot.x * sinA + posRelToPivot.y * cosA;

            vec2 worldPos = aRotCenter + rotatedPos;

            float ndcX = (worldPos.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (worldPos.y / uVievPort.y) * 2.0;

            gl_Position = vec4(ndcX, ndcY, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];
            float u = aSourceRect.x + (texUvs.x * aSourceRect.z);
            float v = aSourceRect.y + (texUvs.y * aSourceRect.w);  

            outTexCoord = vec2(u, v);
            outAlpha = aAlphaRot.x;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 outTexCoord;
        in float outAlpha;

        uniform sampler2D texture1;


        void main(){
	        vec4 texcolor = texture(texture1,outTexCoord);
	        texcolor.a *= outAlpha;
	        FragColor = texcolor;
        }
        )glsl";

        CreateProgramStr("RenderCopyEx", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderCopyCircle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec2 aRadiusAlpha;

        out vec2 oTexCord;
        out vec2 oRadiusAlpha; 

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);
            gl_Position = vec4(finalPos, 0.0, 1.0);
            oTexCord = uvs[gl_VertexID % 6];
            oRadiusAlpha = aRadiusAlpha;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core
        in vec2 oTexCord;
        in vec2 oRadiusAlpha; 
        out vec4 FragColor;

        uniform sampler2D texture0;

        void main(){
            vec2 center = vec2(0.5, 0.5);

            float dist = distance(oTexCord.xy, center);
            if (dist > oRadiusAlpha.x)
                discard;

            vec4 texColor = texture(texture0, oTexCord.xy);
            FragColor = vec4(texColor.rgb, texColor.a * oRadiusAlpha.y);
        }
        )glsl";

        CreateProgramStr("RenderCopyCircle", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderCircle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec3 aRadiusColor;

        out vec4 oColor;
        out vec2 oUV;
        out float oRadius; 

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 1.0), 
            vec2(0.0, 0.0), 
            vec2(1.0, 1.0), 
            vec2(0.0, 0.0),
            vec2(1.0, 0.0), 
            vec2(1.0, 1.0)  
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);
            oColor = vec4(unpackHalfColor(aRadiusColor.y),unpackHalfColor(aRadiusColor.z));
            oUV = uvs[gl_VertexID % 6];
            oRadius = aRadiusColor.x;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core
        in vec4 oColor;
        in vec2 oUV;
        in float oRadius; 

        out vec4 FragColor;

        void main(){
            vec2 center = vec2(0.5, 0.5);
            float dist = distance(oUV, center);
            if (dist > oRadius)
                discard;

            FragColor = vec4(oColor);
        }
        )glsl";

        CreateProgramStr("RenderCircle", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderCopyFilter")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aSourceRect;
        layout(location = 2) in vec2 aColor;


        out vec2 oTexCord;
        out vec4 oFilter;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];

            float u = aSourceRect.x + texUvs.x * aSourceRect.z;
    
            float v0 = 1.0 - aSourceRect.y - aSourceRect.w; 
            float v = v0 + texUvs.y * aSourceRect.w;        

            oTexCord = vec2(u, v);

            oFilter = vec4(unpackHalfColor(aColor.x),unpackHalfColor(aColor.y));
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 oTexCord;
        in vec4 oFilter;

        uniform sampler2D texture1;


        void main(){
	        vec4 texcolor = texture(texture1,oTexCord);
	        texcolor *= oFilter; 
	        FragColor = texcolor;
        }
        )glsl";

        CreateProgramStr("RenderCopyFilter", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderRoundedRectangle")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec2 aColorPacked;
        layout(location = 2) in float aRounding;

        out vec4 oColor;
        out vec2 oUV;
        out vec2 oWH;
        out float oRounding;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 1.0), 
            vec2(0.0, 0.0), 
            vec2(1.0, 1.0), 
            vec2(0.0, 0.0), 
            vec2(1.0, 0.0),
            vec2(1.0, 1.0)  
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 vRG = unpackHalfColor(aColorPacked.x);
            vec2 vBA = unpackHalfColor(aColorPacked.y);
            oColor = vec4(vRG, vBA);
            oUV = uvs[gl_VertexID % 6];
            oWH = aRect.zw;
            oRounding = aRounding;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 oColor;
        in vec2 oUV;
        in vec2 oWH;
        in float oRounding;

        float roundedBoxSDF(vec2 p, vec2 size, float r){
            float safeR = min(r, min(size.x, size.y) * 0.5);
            vec2 halfSize = size * 0.5;
            vec2 d = abs(p) - halfSize + safeR;
    
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - safeR;
        }

        void main(){
            vec2 pX = oUV * oWH - oWH * 0.5;

            float d = roundedBoxSDF(pX, oWH, oRounding);

            float alpha = 1.0 - smoothstep(-1.0, 1.0, d);

            if (alpha <= 0.001) discard;

            FragColor = vec4(oColor.rgb, oColor.a * alpha);
        }
        )glsl";

        CreateProgramStr("RenderRoundedRectangle", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderCopyRounded")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in float aAlpha;
        layout(location = 2) in float aRounding;

        out vec2 oUV;
        out vec2 oWH;
        out float oAlpha;
        out float oRounding;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            oUV = uvs[gl_VertexID % 6];
            oWH = aRect.zw;
            oAlpha = aAlpha;
            oRounding = aRounding;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec2 oUV;
        in vec2 oWH;
        in float oAlpha;
        in float oRounding;

        uniform sampler2D texture1;

        float roundedBoxSDF(vec2 p, vec2 size, float r){
            float safeR = min(r, min(size.x, size.y) * 0.5);
            vec2 halfSize = size * 0.5;
            vec2 d = abs(p) - halfSize + safeR;
    
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - safeR;
        }

        void main(){

            vec2 pX = oUV * oWH - oWH * 0.5;
            float d = roundedBoxSDF(pX, oWH, oRounding);

            float alpha = 1.0 - smoothstep(-1.0, 1.0, d);

            if (alpha <= 0.001) discard;
            
            vec4 texcolor = texture(texture1,oUV);
	        texcolor.a *= oAlpha * alpha;

	        FragColor = texcolor;
        }
        )glsl";

        CreateProgramStr("RenderCopyRounded", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderBorder")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec2 aColorPacked;
        layout(location = 2) in float aWidth;

        out vec4 oColor;
        out vec2 oUV;
        out vec2 oRectSize;
        out float oWidth;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            oColor = vec4(unpackHalfColor(aColorPacked.x), unpackHalfColor(aColorPacked.y));

            oUV.xy = uvs[gl_VertexID % 6];
            oRectSize = aRect.zw;
            oWidth = aWidth;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        in vec4 oColor;
        in vec2 oUV;
        in vec2 oRectSize;
        in float oWidth;

        out vec4 FragColor;


        float boxSDF(vec2 p, vec2 size) {
            vec2 d = abs(p - size * 0.5) - (size * 0.5);
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
        }

        void main(){
            vec2 p_px = oUV * oRectSize;
    
            float d = boxSDF(p_px, oRectSize);

            float finalAlpha = smoothstep(-oWidth - 1.0, -oWidth, d);

            if (finalAlpha <= 0.001) discard;

            FragColor = vec4(oColor.rgb, oColor.a * finalAlpha);
        }
        )glsl";

        CreateProgramStr("RenderBorder", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderRoundedBorder")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec2 aColorPacked;
        layout(location = 2) in float aWidth;
        layout(location = 3) in float aRoundingSize;

        out vec4 oColor;
        out vec2 oUV;
        out vec2 oRectSize;
        out float oWidth;
        out float oRoundingSize;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            oColor = vec4(unpackHalfColor(aColorPacked.x), unpackHalfColor(aColorPacked.y));

            oUV.xy = uvs[gl_VertexID % 6];
            oRectSize = aRect.zw;
            oWidth = aWidth;
            oRoundingSize = aRoundingSize;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        in vec4 oColor;
        in vec2 oUV;
        in vec2 oRectSize;
        in float oWidth;
        in float oRoundingSize;

        out vec4 FragColor;


        float roundedBoxSDF(vec2 p, vec2 size, float r){
            vec2 d = abs(p - size * 0.5) - (size * 0.5 - vec2(r));
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r; 
        }

        void main(){
            vec2 pPX = oUV * oRectSize;

            float d = roundedBoxSDF(pPX, oRectSize, oRoundingSize);

            float alphaOuter = 1.0 - smoothstep(0.0, 1.0, d);

            float alphaInner = 1.0 - smoothstep(0.0, 1.0, d + oWidth);

            float finalAlpha = alphaOuter - alphaInner;

            finalAlpha = clamp(finalAlpha, 0.0, 1.0);

            if (finalAlpha <= 0.001) discard;

            FragColor = vec4(oColor.rgb, oColor.a * finalAlpha);
        }
        )glsl";

        CreateProgramStr("RenderRoundedBorder", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderMasked")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aSourceRect;
        layout(location = 2) in float aAlpha;

        out vec4 oTexCoord;
        out float oAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];

            float u = aSourceRect.x + texUvs.x * aSourceRect.z;
    
            float v0 = 1.0 - aSourceRect.y - aSourceRect.w; 
            float v = v0 + texUvs.y * aSourceRect.w;        

            oTexCoord = vec4(vec2(u, v),uvs[gl_VertexID % 6]);
            oAlpha = aAlpha;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 oTexCoord;
        in float oAlpha;

        uniform sampler2D texture1;
        uniform sampler2D texture2;


        void main(){
	        vec4 texcolor = texture(texture1,oTexCoord.xy);
	        vec4 texcolor2 = texture(texture2,oTexCoord.zw);
	        texcolor.a *= oAlpha;
	        FragColor = vec4(texcolor.r * texcolor2.r, texcolor.g * texcolor2.g, 
		        texcolor.b * texcolor2.b, texcolor.a);
        }
        )glsl";

        CreateProgramStr("RenderMasked", vertexStr, fragmentStr);
    }


    if (!IsProgram("RenderDoubleMasked")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aSourceRect;
        layout(location = 2) in vec4 aSourceRectTwo;
        layout(location = 3) in float aAlpha;

        out vec4 oTexCoord;
        out float oAlpha;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];

            float u = aSourceRect.x + texUvs.x * aSourceRect.z;
    
            float v0 = 1.0 - aSourceRect.y - aSourceRect.w; 
            float v = v0 + texUvs.y * aSourceRect.w;  
    
            float uTwo = aSourceRectTwo.x + texUvs.x * aSourceRectTwo.z;
    
            float v0Two = 1.0 - aSourceRectTwo.y - aSourceRectTwo.w; 
            float vTwo = v0Two + texUvs.y * aSourceRectTwo.w;  

            oTexCoord = vec4(vec2(u, v),vec2(uTwo, vTwo));
            oAlpha = aAlpha;
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 oTexCoord;
        in float oAlpha;

        uniform sampler2D texture1;
        uniform sampler2D texture2;


        void main(){
	        vec4 texcolor = texture(texture1,oTexCoord.xy);
	        vec4 texcolor2 = texture(texture2,oTexCoord.zw);
	        texcolor.a *= oAlpha;
	        FragColor = vec4(texcolor.r * texcolor2.r, texcolor.g * texcolor2.g, 
		        texcolor.b * texcolor2.b, texcolor.a);
        }
        )glsl";

        CreateProgramStr("RenderDoubleMasked", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderShape")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec2 aColor;

        out vec4 oFilter;
        out vec2 oUV;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), // 0
            vec2(0.0, 1.0), // 1
            vec2(1.0, 0.0), // 2
            vec2(0.0, 1.0), // 3
            vec2(1.0, 1.0), // 4
            vec2(1.0, 0.0)  // 5
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, -1.0), // Left Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, -1.0), // Right Down
            vec2(0.0, 0.0),  // Left Up
            vec2(1.0, 0.0),  // Right Up
            vec2(1.0, -1.0)  // Right Down
        );

        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        void main() {
            float ndcX = (aRect.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (aRect.y / uVievPort.y) * 2.0;
            float ndcW = (aRect.z / uVievPort.x) * 2.0;
            float ndcH = (aRect.w / uVievPort.y) * 2.0;

	        vec2 localPos = indexPos[gl_VertexID % 6];

            vec2 finalPos = vec2(ndcX + localPos.x * ndcW, ndcY + localPos.y * ndcH);

            gl_Position = vec4(finalPos, 0.0, 1.0);

            vec2 texUvs = uvs[gl_VertexID % 6];

            oFilter = vec4(unpackHalfColor(aColor.x),unpackHalfColor(aColor.y));
            oUV = uvs[gl_VertexID % 6];
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core
        out vec4 FragColor;

        in vec2 oUV;
        in vec4 oFilter;

        uniform sampler2D texture1;

        void main(){
	        vec4 texcolor = texture(texture1,oUV);
	        vec4 finalColor = oFilter;
	        finalColor.a = texcolor.a * oFilter.a;
	        FragColor = finalColor;
        }
        )glsl";

        CreateProgramStr("RenderShape", vertexStr, fragmentStr);
    }

    if (!IsProgram("RenderUPR")) {
        constexpr const char* vertexStr = R"glsl(
        #version 330 core
        layout(location = 0) in vec4 aRect;
        layout(location = 1) in vec4 aVecOne;
        layout(location = 2) in vec4 aVecTwo;
        layout(location = 3) in float aLastVal;
        layout(location = 4) in float aShaderId;

        out vec4 oVecOne;
        out vec4 oVecTwo;
        out vec2 oVecThree;
        flat out int oShaderId;

        layout(std140) uniform ViewportBlock {
            vec2 uVievPort;
        };


        const vec2 uvs[6] = vec2[6](
            vec2(0.0, 0.0), 
            vec2(0.0, 1.0), 
            vec2(1.0, 0.0), 
            vec2(0.0, 1.0), 
            vec2(1.0, 1.0), 
            vec2(1.0, 0.0)  
        );

        vec2 indexPos[6] = vec2[](
            vec2(0.0, 1.0), // Left Down
            vec2(0.0, 0.0), // Left Up
            vec2(1.0, 1.0), // Right Down
            vec2(0.0, 0.0), // Left Up
            vec2(1.0, 0.0), // Right Up
            vec2(1.0, 1.0)  // Right Down
        );


        vec2 unpackHalfColor(float packedColor){
	        int col = int(packedColor);
	        float r  = float((col >> 8) & 255); // in RG it would be R
	        float g = float(col & 255); // This would be B
	        r /=255.0;
	        g /=255.0;
	        return vec2(r, g);
        }

        vec4 GetStandardPos(vec4 aRect){
            vec2 localPos = indexPos[gl_VertexID % 6];
            vec2 absolutePos = aRect.xy + (localPos * aRect.zw);
            float ndcX = (absolutePos.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (absolutePos.y / uVievPort.y) * 2.0;
    
            return vec4(ndcX, ndcY, 0.0, 1.0);
        }

        vec4 GetRotPos(vec4 aRect, float rot, vec2 rotCenter){
            vec2 localPos = indexPos[gl_VertexID % 6];
    
            vec2 absolutePos = aRect.xy + (localPos * aRect.zw);

            vec2 posRelToPivot = absolutePos - rotCenter;

            float rad = radians(rot);
            float cosA = cos(rad);
            float sinA = sin(rad);
    
            // 2D Rotation Matrix
            vec2 rotatedPos;
            rotatedPos.x = posRelToPivot.x * cosA - posRelToPivot.y * sinA;
            rotatedPos.y = posRelToPivot.x * sinA + posRelToPivot.y * cosA;

            vec2 worldPos = rotCenter + rotatedPos;

            float ndcX = (worldPos.x / uVievPort.x) * 2.0 - 1.0;
            float ndcY = 1.0 - (worldPos.y / uVievPort.y) * 2.0;

            return vec4(ndcX, ndcY, 0.0, 1.0);
        }

        vec2 CalcUVS(vec4 source){
            vec2 texUvs = uvs[gl_VertexID % 6]; 
            float u = source.x + (texUvs.x * source.z);
            float v0 = 1.0 - source.y - source.w; 
            float v = v0 + (texUvs.y * source.w);

            return vec2(u, v);
        }


        void main() {
	        oShaderId = int(aShaderId);

            switch(oShaderId){
                case 0:{ // Render Rect
                    gl_Position = GetStandardPos(aRect);
                    oVecOne = aVecOne / 255.0;
                    break;
                }
                case 1:{ // Render Rect EX
                    gl_Position = GetRotPos(aRect,aVecTwo.x, vec2(aVecTwo.y, aVecTwo.z));
                    oVecOne = aVecOne / 255.0;
                    break;
                }
                case 2:{ // Render Copy
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = uvs[gl_VertexID % 6]; //UV
                    oVecOne.z = aVecOne.x; //ALpha
                    break;
                }
                case 3:{ // Render Copy Part 
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = CalcUVS(aVecOne);
                    oVecOne.z = aVecTwo.x;
                    break;
                }
                case 4:{ // Render Copy Ex
                    gl_Position = GetRotPos(aRect,aVecTwo.y, vec2(aVecTwo.z, aVecTwo.w));
                    oVecOne.xy = CalcUVS(aVecOne);
                    oVecOne.z = aVecTwo.x;
                    break;
                }
                case 5:{ // Render Copy Circle
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = uvs[gl_VertexID % 6]; //UV
                    oVecOne.z = aVecOne.x; // radius 
                    oVecOne.w = aVecOne.y; // alpha 
                    break;
                }
                case 6:{ // Render  Circle
                    gl_Position = GetStandardPos(aRect);
                    oVecOne = aVecOne / 255.0;
                    oVecTwo.xy = uvs[gl_VertexID % 6]; //UV
                    oVecTwo.z = aVecTwo.x; // radius 
                    break;
                }
                case 7:{ // Render  Rounded Rect 
                    gl_Position = GetStandardPos(aRect);
                    oVecOne = aVecOne / 255.0;
                    oVecTwo.xy = uvs[gl_VertexID % 6]; //UV
                    oVecTwo.zw = aRect.zw; // Rect width and Height
                    oVecThree.x = aVecTwo.x; // Rounding Radius
                    break;
                }
                case 8:{ // Render Copy Rounded Rect 
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = uvs[gl_VertexID % 6];
                    oVecOne.zw = aRect.zw; // Rect width and Height
                    oVecTwo.x = aVecOne.x; // alpha
                    oVecTwo.y = aVecOne.y; // Rounding Radius
                    break;
                }

                case 9:{ // Render Copy Filtered 
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = CalcUVS(aVecOne); // UV
                    oVecTwo.rgb = aVecTwo.rgb / 255.0;  // Color.rgb
                    oVecTwo.a = aVecTwo.a; // tex alpha
                    break;
                }
                case 10:{ // Render Border
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = uvs[gl_VertexID % 6]; // UV
                    oVecOne.zw = aRect.zw; // Rect width and Height
                    oVecTwo = aVecOne / 255.0; //Color + alpha
                    oVecThree.x = aVecTwo.x;
                    break;
                }
                case 11:{ // Render Rounded Border
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = uvs[gl_VertexID % 6]; // UV
                    oVecOne.zw = aRect.zw; // Rect width and Height
                    oVecTwo = aVecOne / 255.0; //Color + alpha
                    oVecThree.x = aVecTwo.x; // width
                    oVecThree.y = aVecTwo.y; // rounding size
                    break;
                }
                case 12:{ // Render Masked
                    gl_Position = GetStandardPos(aRect);
                    vec2 firstTexCoord = CalcUVS(aVecOne); // UV of sourced tex
                    oVecOne = vec4(firstTexCoord,uvs[gl_VertexID % 6]);
                    oVecTwo.x = aVecTwo.x; // Alpha
                    break;
                }
                case 13:{ // Render Double Masked
                    gl_Position = GetStandardPos(aRect);
                    vec2 firstTexCoord = CalcUVS(aVecOne); // UV of sourced tex
                    vec2 secondTexCoord = CalcUVS(aVecTwo); // UV of sourced tex
                    oVecOne = vec4(firstTexCoord,secondTexCoord);
                    oVecTwo.x = aLastVal; // Alpha
                    break;
                }
                case 14:{ // Render Shape
                    gl_Position = GetStandardPos(aRect);
                    oVecOne.xy = uvs[gl_VertexID % 6]; //UV
                    oVecTwo = aVecOne / 255.0;
                    break;
                }
            }
        }
        )glsl";

        constexpr const char* fragmentStr = R"glsl(
        #version 330 core

        out vec4 FragColor;

        in vec4 oVecOne;
        in vec4 oVecTwo;
        in vec2 oVecThree;
        flat in int oShaderId;

        uniform sampler2D texture1;
        uniform sampler2D texture2;

        float roundedBoxSDF(vec2 p, vec2 size, float r){
            float safeR = min(r, min(size.x, size.y) * 0.5);
            vec2 halfSize = size * 0.5;
            vec2 d = abs(p) - halfSize + safeR;
    
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - safeR;
        }

        float boxSDF(vec2 p, vec2 size) {
            vec2 d = abs(p - size * 0.5) - (size * 0.5);
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0);
        }

        float roundedBoxBorderSDF(vec2 p, vec2 size, float r){
            vec2 d = abs(p - size * 0.5) - (size * 0.5 - vec2(r));
            return length(max(d, 0.0)) + min(max(d.x, d.y), 0.0) - r; 
        }

        void main(){
            switch(oShaderId){
                case 0:{ // Render Rect
                    FragColor = oVecOne;
                    break;
                }
                case 1:{ // Render Rect EX
                    FragColor = oVecOne;
                    break;
                }
                case 2:{ // Render Copy
                    FragColor = texture(texture1,oVecOne.xy);
                    FragColor.a *= oVecOne.z;
                    break;
                }
                case 3:{ // Render Copy Part 
                    FragColor = texture(texture1,oVecOne.xy);
                    FragColor.a *= oVecOne.z;
                    break;
                }
                case 4:{ // Render Copy Ex
                    FragColor = texture(texture1,oVecOne.xy);
                    FragColor.a *= oVecOne.z;
                    break;
                }
                case 5:{ // Render Copy Circle
                    vec2 center = vec2(0.5, 0.5);
                    float dist = distance(oVecOne.xy, center);
                    if (dist > oVecOne.z)
                        discard;

                    vec4 texColor = texture(texture1, oVecOne.xy);
                    FragColor = vec4(texColor.rgb, texColor.a * oVecOne.w);
                    break;
                }

                case 6:{ // Render  Circle
                    vec2 center = vec2(0.5, 0.5);
                    float dist = distance(oVecTwo.xy, center);
                    if (dist > oVecTwo.z)
                        discard;

                    FragColor = vec4(oVecOne);
                    break;
                }

                case 7:{ // Render  Rounded Rect 
                    vec2 pX = oVecTwo.xy * oVecTwo.zw - oVecTwo.zw * 0.5;

                    float d = roundedBoxSDF(pX, oVecTwo.zw, oVecThree.x); 

                    float alpha = 1.0 - smoothstep(-1.0, 1.0, d);

                    if (alpha <= 0.001) discard;

                    FragColor = vec4(oVecOne.rgb, oVecOne.a * alpha);
                    break;
                }
                case 8:{ // Render Copy  Rounded Rect 
                    vec2 pX = oVecOne.xy * oVecOne.zw - oVecOne.zw * 0.5;
                    float d = roundedBoxSDF(pX, oVecOne.zw, oVecTwo.y); 

                    float alpha = 1.0 - smoothstep(-1.0, 1.0, d);

                    if (alpha <= 0.001) discard;
            
                    vec4 texcolor = texture(texture1,oVecOne.xy);
	                texcolor.a *= oVecTwo.x * alpha;

	                FragColor = texcolor;
                    break;
                }
                case 9:{ // Render Copy Filtered
	                vec4 texcolor = texture(texture1,oVecOne.xy);
	                texcolor *= oVecTwo; 
	                FragColor = texcolor;
                    break;
                }
                case 10:{ // Render Border
                    vec2 pX = oVecOne.xy * oVecOne.zw;
    
                    float d = boxSDF(pX, oVecOne.zw);
                    float width = oVecThree.x;

                    float finalAlpha = smoothstep(-width - 1.0, -width, d);

                    if (finalAlpha <= 0.001) discard;

                    FragColor = vec4(oVecTwo.rgb, oVecTwo.a * finalAlpha);
                    break;
                }
                case 11:{ // Render Rounded Border
                    vec2 pPX = oVecOne.xy *  oVecOne.zw;
                    float width = oVecThree.x;
                    float d = roundedBoxBorderSDF(pPX, oVecOne.zw, oVecThree.y); // 8.0 is the size of a curve if nedded the change uniform is requied

                    float alphaOuter = 1.0 - smoothstep(0.0, 1.0, d);

                    float alphaInner = 1.0 - smoothstep(0.0, 1.0, d + width);

                    float finalAlpha = alphaOuter - alphaInner;

                    finalAlpha = clamp(finalAlpha, 0.0, 1.0);

                    if (finalAlpha <= 0.001) discard;

                    FragColor = vec4(oVecTwo.rgb, oVecTwo.a * finalAlpha);
                    break;
                }
                case 12:{ // Render Masked
	                vec4 texcolor = texture(texture1,oVecOne.xy);
	                vec4 texcolor2 = texture(texture2,oVecOne.zw);
	                texcolor.a *= oVecTwo.x;
	                FragColor = vec4(texcolor.r * texcolor2.r, texcolor.g * texcolor2.g, 
		                texcolor.b * texcolor2.b, texcolor.a);
                    break;
                }
                case 13:{ // Render Double Masked
	                vec4 texcolor = texture(texture1,oVecOne.xy);
	                vec4 texcolor2 = texture(texture2,oVecOne.zw);
	                texcolor.a *= oVecTwo.x;
	                FragColor = vec4(texcolor.r * texcolor2.r, texcolor.g * texcolor2.g, 
		                texcolor.b * texcolor2.b, texcolor.a);
                    break;
                }
                case 14:{ // Render Shape
	                vec4 texColor = texture(texture1,oVecOne.xy);
                    vec4 finalColor = oVecTwo;
                    finalColor.a = texColor.a * oVecTwo.a;
	                FragColor = finalColor;
                    break;
                }
            }
        }
        )glsl";

        CreateProgramStr("RenderUPR", vertexStr, fragmentStr);
    }
}