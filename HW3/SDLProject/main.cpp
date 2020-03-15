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

#define PLATFORM_COUNT 26
#define TARGET_COUNT 2

struct GameState {
    Entity *player;
    Entity *platforms;
    Entity *targets;
};

const char *targetPath = "platformPack_tile028.png";
const char *arenaTilePath = "platformPack_tile016.png";
const char *shipPath = "playerShip3_red.png";
const char *fontPath = "font1.png";

GameState state;

SDL_Window* displayWindow;
bool gameIsRunning = true;
bool collision = false;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint fontTextureID;

std::pair<float, float> arenaCoords[PLATFORM_COUNT] = {
    {-5, -3.25}, {-5, -2.25}, {-5, -1.25},
    {-5, -.25}, {-5, .75}, {-5, 1.75},
    {-5, 2.75}, {-5, 3.75}, {5, -3.25},
    {5, -2.25}, {5, -1.25}, {5, -.25},
    {5, .75}, {5, 1.75}, {5, 2.75},
    {5, 3.75}, {-4, -3.25}, {-3, -3.25},
    {-2, -3.25}, {1, -3.25}, {2, -3.25},
    {3, -3.25}, {4, -3.25}, {-.5, 0},
    {.5, 0}, {-1.5, 0}
};

std::pair<float, float> targetsCoords[TARGET_COUNT] = {{-1, -3.25}, {0, -3.25}};

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
        state.platforms[i].Update(0.0f, NULL, 0, NULL, 0);
        state.platforms[i].entityType = EntityType::PLATFORM;
    }
}

void Initialize() {
    SDL_Init(SDL_INIT_VIDEO);
    displayWindow = SDL_CreateWindow("Lunar Lander!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
    state.player->position = glm::vec3(0, 3, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
    state.player->speed = 1.0f;
    state.player->textureID = LoadTexture(shipPath);
    state.player->entityType = EntityType::PLAYER;
    
    state.platforms = new Entity[PLATFORM_COUNT];
    
    loadArena();
    
    state.targets = new Entity[TARGET_COUNT];
    GLuint targetTextID = LoadTexture(targetPath);
    
    for (int i = 0; i < TARGET_COUNT; i++) {
        state.targets[i].textureID = targetTextID;
        float x = targetsCoords[i].first;
        float y = targetsCoords[i].second;
        state.targets[i].position = glm::vec3(x, y, 0);
        state.targets[i].Update(0.0f, NULL, 0, NULL, 0);
        state.targets[i].entityType = EntityType::TARGET;
    }
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
                if (event.key.keysym.sym == SDLK_SPACE && collision) {
                    // restart
                    collision = false;
                    state.player->position = glm::vec3(0, 3, 0);
                    state.player->movement = glm::vec3(0);
                    state.player->acceleration = glm::vec3(0.0f, -1.0f, 0.0f);
                    state.player->speed = 1.0f;
                    state.player->lastCollision = EntityType::PLAYER;
                }
                break;
        }
    }
    
    if (!collision) {
    
        const Uint8 *keys = SDL_GetKeyboardState(NULL);

        if (keys[SDL_SCANCODE_LEFT]) {
            state.player->acceleration.x += -1.0f;
        }
        else if (keys[SDL_SCANCODE_RIGHT]) {
            state.player->acceleration.x += 1.0f;
        }
        
        if (keys[SDL_SCANCODE_UP]) {
            state.player->velocity.y += 0.1f;
        }

        if (glm::length(state.player->movement) > 1.0f) {
            state.player->movement = glm::normalize(state.player->movement);
        }
    }
}

#define FIXED_TIMESTEP 0.0166666f

float lastTicks = 0;
float accumulator = 0.0f;

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
        state.player->Update(FIXED_TIMESTEP, state.platforms, PLATFORM_COUNT, state.targets, TARGET_COUNT);
        
        deltaTime -= FIXED_TIMESTEP;
    }
    accumulator = deltaTime;
}

void Render() {
    glClear(GL_COLOR_BUFFER_BIT);
    
    for (int i = 0; i < PLATFORM_COUNT; i++) {
        state.platforms[i].Render(&program);
    }
    
    for (int i = 0; i < TARGET_COUNT; i++) {
        state.targets[i].Render(&program);
    }

    state.player->Render(&program);
    
    if (state.player->lastCollision == EntityType::PLATFORM) {
        DrawText(&program, fontTextureID, "Mission: Failed (Space to retry)", 0.5f, -0.25f, glm::vec3(-3.75, 0, 0));
        collision = true;
        
    } else if (state.player->lastCollision == EntityType::TARGET) {
        DrawText(&program, fontTextureID, "Mission: Success (Space to retry)", 0.5f, -0.25f, glm::vec3(-3.75, 0, 0));
        collision = true;
    }
    SDL_GL_SwapWindow(displayWindow);
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
        if (collision) {
            // freeze the player
            state.player->movement = glm::vec3(0);
            state.player->acceleration = glm::vec3(0);
            state.player->velocity = glm::vec3(0);
        }
        std::cout << state.player->lastCollision << std::endl;
    }
    Shutdown();
    return 0;
}
