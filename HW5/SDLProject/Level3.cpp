//
//  Level3.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/19/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Level3.h"

#define LEVEL3_WIDTH 29
#define LEVEL3_HEIGHT 8
#define NUM_ENEMIES 3

unsigned int level3_data[] = {
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    0, 0, 0, 0, 0, 0, 0, 1, 1, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    1, 1, 1, 1, 1, 1, 1, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2,
    2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2
};

void Level3::Initialize() {
    state.nextScene = -1;
    
    GLuint mapTextureID = Util::LoadTexture("tileset.png");
    state.map = new Map(LEVEL3_WIDTH, LEVEL3_HEIGHT, level3_data, mapTextureID, 1.0f, 4, 1);
    
    state.player = new Entity();
    state.player->lastCollision = NULL;
    state.player->position = glm::vec3(1, 0, 0);
    state.player->movement = glm::vec3(0);
    state.player->acceleration = glm::vec3(0.0f, -10.0f, 0.0f);
    state.player->speed = 3.0f;
    state.player->textureID = Util::LoadTexture("george_0.png");
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

    state.enemies = new Entity[NUM_ENEMIES];

    state.enemies[0].lastCollision = NULL;
    state.enemies[0].textureID = Util::LoadTexture("ctg.png");
    state.enemies[0].entityType = EntityType::ENEMY;
    state.enemies[0].aiType = WALKER;
    state.enemies[0].aiState = IDLE;
    state.enemies[0].position = glm::vec3(8, -4, 0);
    state.enemies[0].acceleration = glm::vec3(0.0f, -10.0f, 0.0f);
    state.enemies[0].speed = 0.75f;
    
    state.enemies[1].lastCollision = NULL;
    state.enemies[1].textureID = Util::LoadTexture("ctg.png");
    state.enemies[1].entityType = EntityType::ENEMY;
    state.enemies[1].aiType = WALKER;
    state.enemies[1].aiState = IDLE;
    state.enemies[1].position = glm::vec3(10, -3, 0);
    state.enemies[1].acceleration = glm::vec3(0.0f, -10.0f, 0.0f);
    state.enemies[1].speed = 0.75f;
    
    state.enemies[2].lastCollision = NULL;
    state.enemies[2].textureID = Util::LoadTexture("ctg.png");
    state.enemies[2].entityType = EntityType::ENEMY;
    state.enemies[2].aiType = WALKER;
    state.enemies[2].aiState = IDLE;
    state.enemies[2].position = glm::vec3(12, -2, 0);
    state.enemies[2].acceleration = glm::vec3(0.0f, -10.0f, 0.0f);
    state.enemies[2].speed = 0.75f;
    
    state.ship = new Entity();
    state.ship->textureID = Util::LoadTexture("playerShip3_green.png");
    state.ship->entityType = EntityType::SHIP;
    state.ship->position = glm::vec3(18, -1, 0);
    state.ship->acceleration = glm::vec3(0.0f, -10.0f, 0.0f);
    
    state.ship->Update(0, NULL, 0, NULL, state.map, NULL);
}

int Level3::ProcessInput() {
    int report = 0;
    state.player->movement = glm::vec3(0);
        
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
                if (event.key.keysym.sym == SDLK_UP && state.player->collidedBottom) {
                    state.player->velocity.y = 6;
                }
                break;
        }
    }
    
    const Uint8 *keys = SDL_GetKeyboardState(NULL);

    if (keys[SDL_SCANCODE_LEFT]) {
        if (state.player->position.x > 0.45)
            state.player->movement.x = -1.0f;
        state.player->animIndices = state.player->animLeft;
    }
    else if (keys[SDL_SCANCODE_RIGHT]) {
        if (state.player->position.x < 25.6)
            state.player->movement.x = 1.0f;
        state.player->animIndices = state.player->animRight;
    }
    if (glm::length(state.player->movement) > 1.0f) {
        state.player->movement = glm::normalize(state.player->movement);
    }
    return report;
}

int Level3::Update(float deltaTime) {
//    state.nextScene = 2;
    
    state.player->Update(deltaTime, state.enemies, NUM_ENEMIES, NULL, state.map, state.ship);
    if (state.player->lastCollision != NULL) {
        if (state.player->lastCollision->entityType == ENEMY) {
            // did player kill the enemy?
            if (state.player->collidedBottom && state.player->lastCollision->collidedTop)
                return 1;
            else
                // player got killed
                return 2;
        } else if (state.player->lastCollision->entityType == SHIP) {
            // TODO: ship takeoff animation?
            return 3;
        }
    }

    for (int i = 0; i < NUM_ENEMIES; i++) {
        state.enemies[i].Update(deltaTime, state.player, 1, state.player, state.map, NULL);
        if (state.enemies[i].lastCollision != NULL) {
            if (state.enemies[i].lastCollision->entityType == PLAYER) {
                // did player kill the enemy?
                if (state.player->collidedBottom && state.player->lastCollision->collidedTop)
                    return 1;
                else
                    // player got killed
                    return 2;
            }
        }
    }
    
    return 0;
}

void Level3::Render(ShaderProgram *program) {
    
    state.map->Render(program);
    state.player->Render(program);
    for (int i = 0; i < NUM_ENEMIES; i++)
        state.enemies[i].Render(program);
    state.ship->Render(program);
    
//    Util::DrawText(program, fontTextureID, "Level: ", 0.5f, -0.25f, glm::vec3(-4, 3, 0));
}
