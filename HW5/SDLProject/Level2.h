//
//  Level2.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/19/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Level2_h
#define Level2_h

#include "Scene.h"

class Level2 : public Scene {
public:
    void Initialize() override;
    int ProcessInput() override;
    int Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};

#endif /* Level2_h */
