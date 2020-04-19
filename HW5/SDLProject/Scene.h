//
//  Scene.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/13/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#pragma once
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
#include "Util.h"
#include "Entity.h"
#include "Map.h"

struct GameState {
    Map *map;
    Entity *player;
    Entity *enemies;
    int nextScene;
    Entity *ship;
};

class Scene {
protected:
    GLuint fontTextureID;
public:
    GameState state;
    virtual void Initialize() = 0;
    virtual int ProcessInput() = 0;
    virtual int Update(float deltaTime) = 0;
    virtual void Render(ShaderProgram *program) = 0;
};
