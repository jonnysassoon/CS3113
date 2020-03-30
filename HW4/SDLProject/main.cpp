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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "Entity.h"
#include <vector>
#include <deque>

#define PLATFORM_COUNT 11
#define FIXED_TIMESTEP 0.0166666f

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *enemy;
    int enemiesKilled = 0;
    std::deque<Entity*> bullets;
};

const char *arenaTilePath = "platformPack_tile016.png";
const char *playerPath = "george_0.png";
const char *fontPath = "font1.png";
const char *enemyPath = "ctg.png";
const char *bulletPath = "turretBase_small.png";

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID, enemyTextureID, bulletTextureID;

float lastTicks = 0;
float accumulator = 0.0f;

std::pair<float, float> arenaCoords[PLATFORM_COUNT] = {
    {-5, -3.25}, {5, -3.25}, {-4, -3.25},
    {-3, -3.25}, {-2, -3.25}, {-1, -3.25},
    {0, -3.25}, {1, -3.25}, {2, -3.25},
    {3, -3.25}, {4, -3.25}
};

GLuint LoadTexture(const char* filePath) {
    int w, h, n;
    unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);
    
    if (image == NULL) {
        std::cout << "Unable to load image. Make sure the path is correct\n";
        assert(false);
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

void DrawText(ShaderProgram *program, GLuint fontTextureID, std::string text,
                                 float size, float spacing, glm::vec3 position) {
    float width = 1.0f / 16.0f;
    float height = 1.0f / 16.0f;
    std::vector<float> vertices;
    std::vector<float> texCoords;
    for(int i = 0; i < text.size(); i++) {
        int index = (int)text[i];
        float offset = (size + spacing) * i;
        float u = (float)(index % 16) / 16.0f;
        float v = (float)(index / 16) / 16.0f;
        vertices.insert(vertices.end(), {offset + (-0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (0.5f * size), -0.5f * size, offset + (0.5f * size), 0.5f * size, offset + (-0.5f * size), -0.5f * size,});
        texCoords.insert(texCoords.end(), { u, v,
            u, v + height,
            u + width, v,
            u + width, v + height,
            u + width, v,
            u, v + height,
        });
    } // end of for loop
    glm::mat4 mm = glm::mat4(1.0f);
    mm = glm::translate(mm, position);
    program->SetModelMatrix(mm);
    glUseProgram(program->programID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices.data());
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords.data());
    glEnableVertexAttribArray(program->texCoordAttribute);
    glBindTexture(GL_TEXTURE_2D, fontTextureID);
    glDrawArrays(GL_TRIANGLES, 0, (int)(text.size() * 6));
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
     

void loadArena() {
    GLuint platformTextureID = LoadTexture(arenaTilePath);

    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].textureID = platformTextureID;
        float x = arenaCoords[i].first;
        float y = arenaCoords[i].second;
        state.platforms[i].position = glm::vec3(x, y, 0);
        state.platforms[i].Update(NULL, 0.0f, NULL, 0, NULL, &state.enemiesKilled, lastTicks, NULL, bulletTextureID);
        state.platforms[i].entityType = EntityType::PLATFORM;
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Platformer With AIs!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
    SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
    SDL_GL_MakeCurrent(displayWindow, context);
    
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
    
    fontTextureID = LoadTexture(fontPath);
    
    // Initialize Player
    state.player = new Entity();
    state.player->position = glm::vec3(-4, -2.25, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0f, -3.0f, 0.0f);
    state.player->speed = 3.0f;
    state.player->textureID = LoadTexture(playerPath);
    state.player->entityType = EntityType::PLAYER;
    
    state.player->animRight = new int[4] {3, 7, 11, 15};
    state.player->animLeft = new int[4] {1, 5, 9, 13};
    state.player->animUp = new int[4] {2, 6, 10, 14};
    state.player->animDown = new int[4] {0, 4, 8, 12};

    state.player->animIndices = state.player->animRight;
    state.player->animFrames = 4;
    state.player->animIndex = 0;
    state.player->animTime = 0;
    state.player->animCols = 4;
    state.player->animRows = 4;
    
    state.platforms = new Entity[PLATFORM_COUNT];
    loadArena();
    
    enemyTextureID = LoadTexture(enemyPath);
    
    state.enemy = new Entity();
    state.enemy->textureID = enemyTextureID;
    state.enemy->entityType = EntityType::ENEMY;
    state.enemy->aiType = WALKER;
    state.enemy->position = glm::vec3(4, -2.25, 0);
    state.enemy->speed = 1.0f;
    
    bulletTextureID = LoadTexture(bulletPath);
}

void ProcessInput() {
    state.player->movement = glm::vec3(0);
    
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
            case SDL_WINDOWEVENT_CLOSE:
                gameIsRunning = false;
                break;
            case SDL_KEYUP:
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.sym == SDLK_w && state.player->collidedBottom) {
                    state.player->velocity.y = 3;
                }
                break;
        }
    }
    
    if (!state.player->isActive or state.enemiesKilled == 3) return;
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_A]) {
        state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_D]) {
        state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
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
        state.player->Update(state.player, FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.enemy, &state.enemiesKilled, lastTicks, &state.bullets, bulletTextureID);
        state.enemy->Update(state.player, FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.player, &state.enemiesKilled, lastTicks, &state.bullets, bulletTextureID);
        
        for (size_t i = 0; i < state.bullets.size(); i++) {
            if (state.bullets[i]->position.x < -5 || state.bullets[i]->position.x > 3.5) {
                state.bullets.erase(state.bullets.begin() + i);
            } else {
                state.bullets[i]->Update(state.player, deltaTime, state.platforms, PLATFORM_COUNT, state.enemy, &state.enemiesKilled, lastTicks, &state.bullets, bulletTextureID);
            }
        }
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
    
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }

    state.player->Render(&program);
    
    state.enemy->Render(&program);
    
    for (Entity *bullet : state.bullets) {
        bullet->Render(&program);
    }
    
    if (!state.player->isActive) {
        DrawText(&program, fontTextureID, "GAME OVER", 0.5f, -0.25f, glm::vec3(-1, 0, 0));
    }
    
    if (state.enemiesKilled == 3) {
        DrawText(&program, fontTextureID, "YOU WIN", 0.5f, -0.25f, glm::vec3(-1, 0, 0));
    }
    
    DrawText(&program, fontTextureID, "Enemies Killed: " + std::to_string(state.enemiesKilled), 0.5f, -0.25f, glm::vec3(-4.5, 3, 0));
    
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
    freeze(state.player);
    freeze(state.enemy);
    for (Entity * bullet : state.bullets) {
        freeze(bullet);
    }
}

int main(int argc, char* argv[]) {
    Initialize();
    
    while (gameIsRunning) {
        ProcessInput();
        Update();
        Render();
        if (!state.player->isActive) {
            break;
        }
        if (!state.enemy->isActive) {
            delete state.enemy;
            if (state.enemiesKilled == 1) {
                state.enemy = new Entity();
                state.enemy->textureID = enemyTextureID;
                state.enemy->entityType = EntityType::ENEMY;
                state.enemy->aiType = SHOOTER;
                state.enemy->aiState = IDLE;
                state.enemy->position = glm::vec3(4, -2.25, 0);
                state.enemy->speed = 1.0f;
                // load enemy # 2
            } else if (state.enemiesKilled == 2) {
                state.enemy = new Entity();
                state.enemy->textureID = enemyTextureID;
                state.enemy->entityType = EntityType::ENEMY;
                state.enemy->aiType = JUMPER;
                state.enemy->position = glm::vec3(0, 3, 0);
                state.enemy->acceleration = glm::vec3(0, -2, 0);
                state.enemy->speed = 1.0f;
                // load enemy # 3
            } else {
                break;
            }
        }
    }

    freezeGamestate();
    while (gameIsRunning) {
        ProcessInput();
        Render();
    }
    Shutdown();
    return 0;
}
