#ifndef SHADER_H
#define SHADER_H

#ifdef __EMSCRIPTEN__
    #include <GLES3/gl3.h>
#else
#include "../include/glad/gl.h"
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "../include/cglm/cglm.h"

typedef struct {
    unsigned int ID;

} Shader;

Shader createShader(const char* vertexPath, const char* fragmentPath); 
void use(Shader* shader);

void setInt(Shader* shader, const char* name, int value);
void setFloat(Shader* shader, const char* name, float value);
void setMat4(Shader* shader, const char* name, mat4 value); 
void deleteShader(Shader* shader);
#endif
