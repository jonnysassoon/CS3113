#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
float lastFrameTix = 0.0f;

ShaderProgram program;

glm::mat4 viewMatrix, p1Matrix, p2Matrix, ballMatrix, projectionMatrix;

float p1Y, p2Y, ballX, ballY, deltaP1, deltaP2;

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Pong!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 1280, 960);
    
    // Load the shaders for handling textures!
    program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

    viewMatrix = glm::mat4(1.0f);
    p1Matrix = glm::mat4(1.0f);
    p2Matrix = glm::mat4(1.0f);
    ballMatrix = glm::mat4(1.0f);
//
    p1Y = p2Y = ballX = ballY = 0.0f;
//
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
//
    glUseProgram(program.programID);
//
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
}

void ProcessInput() {
    float ticks = (float) SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastFrameTix;
    lastFrameTix = ticks;

    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYUP:
                deltaP1 = deltaP2 = 0.0f;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        deltaP1 = 5.0 * deltaTime;
                        break;
                    case SDLK_DOWN:
                        deltaP1 = -5.0 * deltaTime;
                        break;
                    case SDLK_w:
                        deltaP2 = 5.0 * deltaTime;
                        break;
                    case SDLK_s:
                        deltaP2 = -5.0 * deltaTime;
                        break;
                }
                break;
        }
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    program.SetColor(1.0f, 1.0f, 1.0f, 1.0f);

//     draw paddle1
    p1Matrix = glm::mat4(1.0f);
    p1Matrix = glm::translate(p1Matrix, glm::vec3(0.0f, p1Y, 0.0f));
    program.SetModelMatrix(p1Matrix);

    float p1[] = { 4.5, -0.5, 4.4, -0.5, 4.4, 0.5, 4.5, -0.5, 4.4, 0.5, 4.5, 0.5 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p1);
    glEnableVertexAttribArray(program.positionAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    // draw paddle2
    p2Matrix = glm::mat4(1.0f);
    p2Matrix = glm::translate(p2Matrix, glm::vec3(0.0f, p2Y, 0.0f));
    program.SetModelMatrix(p2Matrix);

    float p2[] = { -4.5, -0.5, -4.4, -0.5, -4.4, 0.5, -4.5, -0.5, -4.4, 0.5, -4.5, 0.5 };
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, p2);
    glEnableVertexAttribArray(program.positionAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    program.SetColor(1.0f, 0.0f, 0.0f, 1.0f);
    
    // draw ball
    ballMatrix = glm::mat4(1.0f);
    ballMatrix = glm::translate(ballMatrix, glm::vec3(ballX, ballY, 0.0f));
    program.SetModelMatrix(ballMatrix);

    float ball[] = {-0.05f, -0.05f, 0.05f, 0.05f, -0.05f, 0.05f, -0.05f, -0.05f, 0.05f, -0.05f, 0.05f, 0.05f};
    glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, ball);
    glEnableVertexAttribArray(program.positionAttribute);

    glDrawArrays(GL_TRIANGLES, 0, 6);
//
    glDisableVertexAttribArray(program.positionAttribute);
//
    SDL_GL_SwapWindow(displayWindow);
    
}

void Update() {
    program.SetModelMatrix(ballMatrix);
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);

    // update p1
    float p1NextY = p1Y + deltaP1;
    if (p1NextY >= 3.0f || p1NextY <= -3.0f) {
        deltaP1 = 0.0f;
    } else {
        p1Y = p1NextY;
    }

    // update p2
    float p2NextY = p2Y + deltaP2;
    if (p2NextY >= 3.0f || p2NextY <= -3.0f) {
        deltaP2 = 0.0f;
    } else {
        p2Y = p2NextY;
    }
    
    // update ball
    
    
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
    return 0;
}
