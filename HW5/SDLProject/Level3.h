//
//  Level3.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/19/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Level3_h
#define Level3_h

#include "Scene.h"

class Level3 : public Scene {
public:
    void Initialize() override;
    int ProcessInput() override;
    int Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};

#endif /* Level3_h */
