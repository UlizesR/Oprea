#ifndef O_UTILS_H
#define O_UTILS_H

#include <GL/glew.h>
#include <stdio.h>
#include <stdlib.h>

#define BUFFER_SIZE 512

//------------------------------------------------------------------------------
// Helper funcs for compiling/linking
//------------------------------------------------------------------------------
// You can load GLSL from files. For simplicity, we read them as text here:
static char* ReadFile(const char* filename)
{
    FILE* f = fopen(filename, "rb");
    if(!f) return NULL;
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char* buffer = (char*)malloc(len+1);
    if(!buffer)
    {
        fclose(f);
        return NULL;
    }
    fread(buffer, 1, len, f);
    buffer[len] = '\0';
    fclose(f);
    return buffer;
}

static void CheckError(GLuint object, GLenum statusType, const char* label, void (*getiv)(GLuint, GLenum, GLint*), void (*getInfoLog)(GLuint, GLsizei, GLsizei*, GLchar*))
{
    GLint status;
    getiv(object, statusType, &status);
    if(status == GL_FALSE)
    {
        char buffer[BUFFER_SIZE];
        getInfoLog(object, BUFFER_SIZE, NULL, buffer);
        fprintf(stderr, "Error in %s:\n%s\n", label, buffer);
    }
}

static GLuint CompileShader(const char* src, GLenum type, int isPath)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, NULL);
    glCompileShader(shader);

    CheckError(shader, GL_COMPILE_STATUS, (type == GL_VERTEX_SHADER) ? "Vertex Shader" : "Fragment Shader", glGetShaderiv, glGetShaderInfoLog);

    return shader;
}

static GLuint LinkProgram(GLuint vs, GLuint fs)
{
    GLuint program = glCreateProgram();
    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    CheckError(program, GL_LINK_STATUS, "Shader Program", glGetProgramiv, glGetProgramInfoLog);
    return program;
}

#endif // O_UTILS_H