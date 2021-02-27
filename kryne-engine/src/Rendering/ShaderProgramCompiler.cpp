/**
 * @file
 * @author Max Godefroy
 * @date 27/11/2020.
 */

#include "kryne-engine/Rendering/ShaderProgramCompiler.h"


ShaderProgramCompiler::ShaderProgramCompiler()
{
    // TODO : Better initialization
    assertIsMainThread();
    this->vertexShaderId = glCreateShader(GL_VERTEX_SHADER);
    this->fragmentShaderId = glCreateShader(GL_FRAGMENT_SHADER);
}


void ShaderProgramCompiler::checkShaderCompile(const Shader *shader, const BufferGeometry *geometry)
{
    const auto &layoutCode = geometry->getShaderLayoutCode();
    bool vertexNeedsUpdate = shader->getNeedsUpdate() & SHADER_VERTEX_NEEDS_UPDATE;

    if (layoutCode.second != previousLayoutCode.second) {
        vertexNeedsUpdate = true;
        previousLayoutCode = make_pair(layoutCode.first, layoutCode.second);
    }

    string definesCode;

    if (vertexNeedsUpdate) {
        definesCode = shader->makeDefinesCode();
        this->compileVertex(shader->getVertexShader(), definesCode);
    }

    if (shader->getNeedsUpdate() & SHADER_FRAGMENT_NEEDS_UPDATE) {
        if (definesCode.empty())
            definesCode = shader->makeDefinesCode();

        this->compileFragment(shader->getFragmentShader(), definesCode);

        shader->linkProgram(this->vertexShaderId, this->fragmentShaderId);
    } else if (vertexNeedsUpdate) {
        shader->linkProgram(this->vertexShaderId, this->fragmentShaderId);
    }
}


void ShaderProgramCompiler::compileShader(const string &code, const GLuint &id)
{
    const GLchar* sourceCodeString = code.c_str();

    // Compiling shader
    glShaderSource(id, 1, &sourceCodeString, nullptr);
    glCompileShader(id);

    // Check if compile succeeded
    int  success;
    char infoLog[512];
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if(success == 0)
    {
        glGetShaderInfoLog(id, 512, nullptr, infoLog);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << infoLog << std::endl;
        ShaderProgramCompiler::printCode(code);
    }
}


void ShaderProgramCompiler::compileVertex(const string &shaderCode, const string &definesCode) const
{
    string code = BASE_GLSL_VERSION_STRING + "\n" +
                  previousLayoutCode.first + "\n" +
                  definesCode + '\n' +
                  replaceIncludes(shaderCode);
    compileShader(code, this->vertexShaderId);
}


void ShaderProgramCompiler::compileFragment(const string &shaderCode, const string &definesCode) const
{
    string code = BASE_GLSL_VERSION_STRING + "\n" +
                  definesCode + '\n' +
                  replaceIncludes(shaderCode);
    compileShader(code, this->fragmentShaderId);
}


string ShaderProgramCompiler::replaceIncludes(const string &baseCode, const string &indentation)
{
    regex includeRegex(R"((\n|^)(\s*)(#include\s*<([a-zA-Z0-9_-]+)>))");
    smatch results;

    string newCode, line;
    istringstream iss(baseCode);
    while (getline(iss, line)) {
        if (!newCode.empty())
            newCode.push_back('\n');
        if (regex_search(line, results, includeRegex)) {
            auto codeChunk = ShaderChunk::getInstance().getCodeChunk(results[4].str());
            newCode += ShaderProgramCompiler::replaceIncludes(codeChunk, indentation + results[2].str());
        } else {
            newCode += indentation + line;
        }
    }

    return newCode;
}


void ShaderProgramCompiler::printCode(const string &code)
{
    string line;
    istringstream iss(code);

    uint64_t count = 0;
    while (getline(iss, line))
        count ++;
    uint64_t lineCountMaxLength = to_string(count).length();

    uint64_t i = 1;
    iss = istringstream(code);
    while (getline(iss, line)) {
        string codeLine = to_string(i) + ": ";

        string space;
        space.resize(lineCountMaxLength + 2 - codeLine.length(), ' ');
        cout << space << codeLine << line << endl;

        i++;
    }
}
