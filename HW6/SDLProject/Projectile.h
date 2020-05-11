//
//  Projectile.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Projectile_h
#define Projectile_h

#include "Entity.h"
enum ProjectileType {BULLET, ROCKET};

class Projectile : public Entity {
    bool enemy;
public:
    Projectile(GLuint textid, glm::vec3 position, glm::vec3 movement, int pow, bool e);
    int power;
    ProjectileType ptype;
    void Update(float deltaTime);
};

#endif /* Projectile_h */
