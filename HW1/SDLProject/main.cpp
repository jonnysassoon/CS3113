#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <iostream>

SDL_Window* displayWindow;
bool gameIsRunning = true;
float lastFrameTix = 0.0f;

ShaderProgram program;

glm::mat4 viewMatrix, modelMatrix_mario, modelMatrix_luigi, projectionMatrix;
const char *luigi_path = "luigi.png";
const char *mario_path = "mario.png";

GLuint marioTextureID, luigiTextureID;

float luigi_x, mario_rotate;

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        exit(1);
    }
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    stbi_image_free(image);
    return textureID;
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Project 1!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);

    // Load the shaders for handling textures!
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
  
    viewMatrix = glm::mat4(1.0f);
    modelMatrix_mario = glm::mat4(1.0f);
    modelMatrix_luigi = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
        
    glUseProgram(program.programID);
    
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    // Good setting for transparency
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    marioTextureID = LoadTexture(mario_path);
    luigiTextureID = LoadTexture(luigi_path);
    
    mario_rotate = luigi_x = 0;
}

void ProcessInput() {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
            gameIsRunning = false;
        }
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    //draw mario
    program.SetModelMatrix(modelMatrix_mario);
    glBindTexture(GL_TEXTURE_2D, marioTextureID);
    
    float verticesM[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesM);
    glEnableVertexAttribArray(program.positionAttribute);

    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    // draw luigi
    program.SetModelMatrix(modelMatrix_luigi);
    glBindTexture(GL_TEXTURE_2D, luigiTextureID);

    float verticesL[] = { -4.5, -2.5, -3.5, -2.5, -3.5, -1.5, -4.5, -2.5, -3.5, -1.5, -4.5, -1.5 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, verticesL);
    glEnableVertexAttribArray(program.positionAttribute);
    
    glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program.texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program.positionAttribute);
    glDisableVertexAttribArray(program.texCoordAttribute);
    SDL_GL_SwapWindow(displayWindow);
    
}

void Update() {
    float ticks = (float) SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastFrameTix;
    lastFrameTix = ticks;
    
    luigi_x = 1.0f * deltaTime;
    mario_rotate = 180.0f * deltaTime;
    
    modelMatrix_luigi = glm::translate(modelMatrix_luigi, glm::vec3(luigi_x, 0.0f, 0.0f));
    modelMatrix_mario = glm::rotate(modelMatrix_mario, glm::radians(mario_rotate), glm::vec3(0.0f, 0.0f, 1.0f));
}

void Shutdown() {
    SDL_Quit();
}

int main(int argc, char* argv[]) {
    Initialize();
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    Shutdown();
}
