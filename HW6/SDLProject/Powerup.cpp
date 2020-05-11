//
//  Powerup.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Powerup.h"

Powerup::Powerup(GLuint tid, PowerupType t, glm::vec3 pos) {
    textureID = tid;
    type = t;
    position = pos;
    movement = glm::vec3(-1, 0, 0);
    speed = 3;
    height = width = .5;
}
