#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "../include/obj.h"
#include "../include/shader.h"
#include "../include/stb_image.h"
#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
    #include <emscripten/html5.h>
    #include <GLES3/gl3.h>
#else
    #include "../include/glad/gl.h"
#endif
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
  if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, 1);
}


const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

GLFWwindow* window;
Shader myShader;
Vertex* vertex;
uint32_t* indices;
size_t vertex_count;
size_t index_count;

unsigned int VAO, VBO, texture;
vec3 heartPositions[] = {
    { 0.0f * 9,  0.0f * 9,  0.0f * 5},
    { 2.0f * 9,  5.0f * 9, -14.0f * 5}, 
    {-1.5f * 9, -2.2f * 9, -2.5f * 5}, 
    {-0.8f * 9, -5.0f * 9, -12.3f * 5},  
    { 2.4f * 9, -0.4f * 9, -3.5f * 5}, 
    {-1.7f * 9,  3.0f * 9, -7.5f * 5}, 
    { 1.3f * 9, -2.0f * 9, -2.5f * 5}, 
    { 1.5f * 9,  2.0f * 9, -2.5f * 5},
    { -2.5f * 9,  -0.5f * 9, -2.5f * 5},
    {-1.3f * 13,  1.0f * 9, -1.5f * 5}
};


void main_loop() {
    processInput(window);
    glClearColor(0.15f, 0.15f, 0.15f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glBindTexture(GL_TEXTURE_2D, texture);
    use(&myShader);
    glBindVertexArray(VAO);


    mat4 view = GLM_MAT4_IDENTITY_INIT;
    mat4 projection = GLM_MAT4_IDENTITY_INIT;

    float timeValue = glfwGetTime(); 
    

    glm_translate(view, (vec3){0.0f, -7.0f, -20.0f});
    glm_perspective(glm_rad(80.0f), 800.0f/ 600.0f, 0.1f, 100.0f, projection);


    setMat4(&myShader, "view", view);
    setMat4(&myShader, "projection", projection);

    mat4 scale = GLM_MAT4_IDENTITY_INIT;
    glm_scale(scale, (vec3){0.6f, 0.6f, 0.6f});

    for (unsigned int i = 0; i < 10; i++) {
        mat4 model = GLM_MAT4_IDENTITY_INIT;


        glm_translate(model, heartPositions[i]);
        glm_rotate(model, glm_rad(-90.0f), (vec3){1.0f, 0.0f, 0.0f});
        glm_rotate(model, glm_rad(45.0f) * timeValue * (i*0.5f) , (vec3){0.0f, 0.0f, 1.0f});

        setMat4(&myShader, "model", model); 
        setMat4(&myShader, "scale", scale);
        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
        printf("Rendered heart number %d\n", i);
    }
    // glDrawArrays(GL_TRIANGLES, 0, 36);


#ifndef __EMSCRIPTEN__
    glfwSwapBuffers(window);
    glfwPollEvents();
#endif
}


int main(void) {
    //Initialize GLFW
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

#ifdef __EMSCRIPTEN__
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
#endif


    //Create Window
    window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "test", NULL, NULL);
    if (!window) {
        printf("Failed to create window\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    
#ifndef __EMSCRIPTEN__
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0) {
        printf("Failed to initialize glad\n");
        return -1;
    }
#endif

    char cwd[FILENAME_MAX]; 
    if (getcwd(cwd, sizeof(cwd)) != NULL) {
        printf("Current working dir: %s\n", cwd);
    } else {
        perror("GetCurrentDir failed");
        return 1;
    }

    //Load Obj
    loadObj("objs/heart.obj", &vertex, &indices, &vertex_count, &index_count);

    //Load Textures
    stbi_set_flip_vertically_on_load(1);
    int width, height, nrChannels;
    unsigned char *data = stbi_load("textures/abstract_deco.jpg", &width, &height, &nrChannels, 0);

    if (!data) {
        printf("Failed to load texture: %s\n", stbi_failure_reason());
        return -1;
    }

    printf("Loaded texture: %dx%d with %d channels\n", width, height, nrChannels);

    //Bind the textures and generate mipmaps
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

    //Create Shader
    myShader = createShader("shaders/vertex.glsl", "shaders/fragment.glsl");
    if (myShader.ID == 0) {
        return -1;
    }

    //Cube vertices and texture coordinates
    float cubeVertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    
    //Generate VAO and VBO stuff
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertex_count * sizeof(Vertex), vertex, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(0);  
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glEnable(GL_DEPTH_TEST);
    // glDisable(GL_CULL_FACE);

    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    //Emscripten loop
#ifdef __EMSCRIPTEN__
    //Emscripten: use requestAnimationFrame
    emscripten_set_main_loop(main_loop, 0, 1);
#else
    //Regular loop
    while (!glfwWindowShouldClose(window)) {
        main_loop();
    }
#endif

    glfwDestroyWindow(window);
    glfwTerminate();
    deleteShader(&myShader);
    glDeleteBuffers(1, &VBO);
    glDeleteVertexArrays(1, &VAO);
    glDeleteTextures(1, &texture);
    free(indices);
    free(vertex);

    
    return 0;  
}
