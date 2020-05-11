//
//  Enemy.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Enemy_h
#define Enemy_h

#include "Copter.h"
#include "Projectile.h"

class Enemy : public Copter {
public:
    Enemy(GLuint textID);
    void Update(float deltaTime, std::vector<Projectile*>& projectiles, float playerY);
};

#endif /* Enemy_h */
