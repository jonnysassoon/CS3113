#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include <SDL_mixer.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#include "Entity.h"
#include "Map.h"
#include "Scene.h"
#include "MainMenu.h"
#include "Level1.h"
#include "Level2.h"
#include "Level3.h"
#include "Util.h"
#include <vector>
#include<unistd.h>

#define FIXED_TIMESTEP 0.0166666f
#define PLAYER_LIVES 3
#define NUM_SCENES 4

const char *arenaTilePath = "platformPack_tile016.png";

Scene *scenes[NUM_SCENES];
Scene *currentScene;
int currSceneNum;
int livesLeft;

SDL_Window* displayWindow;
Mix_Music* music;
Mix_Chunk* success;

bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID, enemyTextureID, bulletTextureID;

float lastTicks = 0;
float accumulator = 0.0f;

class GameOver : public Scene {
    
    void Initialize() override  {
        fontTextureID = Util::LoadTexture("font1.png");
        state.nextScene = 1;
    }
    int ProcessInput() override  {
        int report = 0;
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                case SDL_WINDOWEVENT_CLOSE:
                    report = 1;
                    break;
                case SDL_KEYUP:
                    break;
                case SDL_KEYDOWN:
                    if (event.key.keysym.sym == SDLK_RETURN) {
                        livesLeft = PLAYER_LIVES;
                        
                        report = 2;
                    }
                    break;
            }
        }
        return report;
    }
    int Update(float deltaTime) override  {
        return 0;
    }
    void Render(ShaderProgram *program) override  {
        viewMatrix = glm::mat4(1.0f);
        if (livesLeft == 0)
            Util::DrawText(program, fontTextureID, "YOU LOSE", 0.5f, -0.25f, glm::vec3(-1.25, 0, 0));
        else
            Util::DrawText(program, fontTextureID, "YOU WIN", 0.5f, -0.25f, glm::vec3(-1.25, 0, 0));

        Util::DrawText(program, fontTextureID, "PRESS ENTER TO PLAY AGAIN", 0.5f, -0.25f, glm::vec3(-3, -1, 0));
        
    }
};

void SwitchToScene(Scene *scene) {
    if (currSceneNum == -1) {
        currSceneNum = 0;
    } else {
        currSceneNum = currentScene->state.nextScene;
    }
    currentScene = scene;
    currentScene->Initialize();
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
    displayWindow = SDL_CreateWindow("Platformer With AIs!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
    music = Mix_LoadMUS("crypto.mp3");
    Mix_PlayMusic(music, -1);
    
    success = Mix_LoadWAV("success.wav");
    
#ifdef _WINDOWS
    glewInit();
#endif
    
    glViewport(0, 0, 640, 480);
    
    program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");
    
    viewMatrix = glm::mat4(1.0f);
    modelMatrix = glm::mat4(1.0f);
    projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);
    
    program.SetProjectionMatrix(projectionMatrix);
    program.SetViewMatrix(viewMatrix);
    
    glUseProgram(program.programID);
    
    glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
    glEnable(GL_BLEND);

    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    scenes[0] = new MainMenu();
    scenes[1] = new Level1();
    scenes[2] = new Level2();
    scenes[3] = new Level3();
    
    currSceneNum = -1;
    SwitchToScene(scenes[0]);
    
    livesLeft = PLAYER_LIVES;
    fontTextureID = Util::LoadTexture("font1.png");
}

void ProcessInput() {
    switch (currentScene->ProcessInput()) {
        case 0:
            break;
        case 1:
            gameIsRunning = false;
            break;
        case 2:
            SwitchToScene(scenes[currentScene->state.nextScene]);
            break;
    }
}

void Update() {
    
    float ticks = (float)SDL_GetTicks() / 1000.0f;
    float deltaTime = ticks - lastTicks;
    lastTicks = ticks;
    
    deltaTime += accumulator;
    if (deltaTime < FIXED_TIMESTEP) {
        accumulator = deltaTime;
        return;
    }
    
    while (deltaTime >= FIXED_TIMESTEP) {
        if (currSceneNum != 0) {
            switch (currentScene->Update(FIXED_TIMESTEP)) {
                case 0:
                    // do nothing case
                    break;
                case 1:
                    // enemy dies
                    currentScene->state.player->lastCollision->isActive = false;
                    break;
                case 2:
                    // player dies
                    livesLeft -= 1;
                    if (livesLeft > 0)
                        currentScene->Initialize();
                    else {
                        currSceneNum = -1;
                        SwitchToScene(new GameOver());
                    }
                    break;
                case 3:
                    Mix_PlayChannel(-1, success, 0);
                    sleep(1);
                    if (currSceneNum != NUM_SCENES - 1)
                        SwitchToScene(scenes[currentScene->state.nextScene]);
                    else {
                        currSceneNum = -1;
                        SwitchToScene(new GameOver());
                    }
                    break;
            }
        }
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
    if (currSceneNum == 0) return;
    
    viewMatrix = glm::mat4(1.0f);
    
    if (currentScene->state.player->position.x > 5 && currentScene->state.player->position.x < 21) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-currentScene->state.player->position.x, 3.75, 0));
    } else if (currentScene->state.player->position.x <= 5) {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-5, 3.75, 0));
    } else {
        viewMatrix = glm::translate(viewMatrix, glm::vec3(-21, 3.75, 0));
    }
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    currentScene->Render(&program);
    program.SetViewMatrix((viewMatrix));
    
    SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
    SDL_Quit();
}

void freeze(Entity *object) {
    object->movement = glm::vec3(0);
    object->velocity = glm::vec3(0);
    object->acceleration = glm::vec3(0);
}

void freezeGamestate() {
    freeze(currentScene->state.player);
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
    }
    return 0;
}
