//
//  Projectile.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Projectile.h"

Projectile::Projectile(GLuint textid, glm::vec3 pos, glm::vec3 mov, int pow, bool e) : Entity(), power(pow), enemy(e) {
    textureID = textid;
    position = pos;
    movement = mov;
    speed = 3;
    width = .1;
    height = .1;
    if (power == 30) {
        width = height = .5;
    }
}

void Projectile::Update(float deltaTime) {
    Entity::Update(deltaTime);
    if (enemy)
        modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0,1,0));
}
