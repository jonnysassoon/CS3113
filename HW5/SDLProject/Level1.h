//
//  Level1.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 4/14/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Level1_h
#define Level1_h

#include "Scene.h"

class Level1 : public Scene {
public:
    void Initialize() override;
    int ProcessInput() override;
    int Update(float deltaTime) override;
    void Render(ShaderProgram *program) override;
};


#endif /* Level1_h */
