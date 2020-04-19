//
//  MainMenu.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/16/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "MainMenu.h"

#define MM_WIDTH 14
#define MM_HEIGHT 8

void MainMenu::Initialize() {
    fontTextureID = Util::LoadTexture("font1.png");
    state.nextScene = 1;
}

int MainMenu::ProcessInput() {
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
                    report = 2;
                }
                break;
        }
    }
    return report;
}

int MainMenu::Update(float deltaTime) {
    return 0;
}

void MainMenu::Render(ShaderProgram *program) {
    Util::DrawText(program, fontTextureID, "GET GEORGE TO THE SHIP", 0.5f, -0.25f, glm::vec3(-3, 1, 0));

    Util::DrawText(program, fontTextureID, "PRESS ENTER TO PLAY", 0.5f, -0.25f, glm::vec3(-2.5, -1, 0));
}
