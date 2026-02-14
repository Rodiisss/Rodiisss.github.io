#include "../include/shader.h"

char* loadShaderSource(const char* filePath) {
  FILE* file;

#ifdef _WIN32
    fopen_s(&file, filePath, "rb");
#else
    file = fopen(filePath, "rb");
#endif
  
    if (!file) {
        fprintf(stderr, "Failed to open file: %s\n", filePath);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long length = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = malloc(length + 1);
    if (buffer) {
        fread(buffer, 1, length, file);
        buffer[length] = '\0'; // Null-terminate the string
    }
    fclose(file);
    return buffer;
}

// Helper function to check shader compilation/linking errors
void checkShaderErrors(GLuint shader, const char* type) {
    GLint success;
    GLchar infoLog[1024];
    if (strcmp(type, "PROGRAM") != 0) {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success) {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::SHADER_COMPILATION_ERROR of type: %s\n%s\\n-- ------------------------------------------------ --\\n", type, infoLog);
        }
    } else {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if (!success) {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            fprintf(stderr, "ERROR::PROGRAM_LINKING_ERROR of type: %s\n%s\\n-- ------------------------------------------------ --\\n", type, infoLog);
        }
    }

}

 Shader createShader(const char *vertexPath, const char *fragmentPath){
    char* vertexSource = loadShaderSource(vertexPath);
    char* fragmentSource = loadShaderSource(fragmentPath);
    if (!vertexSource || !fragmentSource) {
        if (vertexSource) free(vertexSource);
        if (fragmentSource) free(fragmentSource);
        return (Shader){0}; // Return a shader with ID 0 (invalid)
  }

    GLuint vertex, fragment;

    // Vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, (const GLchar* const*)&vertexSource, NULL);
    glCompileShader(vertex);
    checkShaderErrors(vertex, "VERTEX");

    // Fragment shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, (const GLchar* const*)&fragmentSource, NULL);
    glCompileShader(fragment);
    checkShaderErrors(fragment, "FRAGMENT");

    // Shader Program
    GLuint id = glCreateProgram();
    glAttachShader(id, vertex);
    glAttachShader(id, fragment);
    glLinkProgram(id);
    checkShaderErrors(id, "PROGRAM");

    // Delete the shaders as they're linked into our program now and no longer necessary
    glDeleteShader(vertex);
    glDeleteShader(fragment);

    // Free the source buffers
    free(vertexSource);
    free(fragmentSource);

    return (Shader){id};
}

void use(Shader* shader) {
  glUseProgram(shader->ID);
}

void setInt(Shader* shader, const char* name, int value) {
    glUniform1i(glGetUniformLocation(shader->ID, name), value);
}

void setFloat(Shader* shader, const char* name, float value) {
    glUniform1f(glGetUniformLocation(shader->ID, name), value);
}

void setMat4(Shader* shader, const char* name, mat4 value) {
    glUniformMatrix4fv(glGetUniformLocation(shader->ID, name), 1, GL_FALSE, (float*)value);
}

// Function to delete the shader program
void deleteShader(Shader* shader) {
    glDeleteProgram(shader->ID);
    shader->ID = 0;
}
