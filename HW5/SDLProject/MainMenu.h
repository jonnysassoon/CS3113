//
//  MainMenu.hpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/16/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef MainMenu_h
#define MainMenu_h

#include "Scene.h"

class MainMenu : public Scene {
public:
    void Initialize() override;
    int ProcessInput() override;
    int Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};

#endif /* MainMenu_hpp */
