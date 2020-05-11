//
//  Player.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Player_h
#define Player_h

#include "Copter.h"
#include "Powerup.h"
#include "Projectile.h"

class Player : public Copter {
    void givePowerup(Powerup* p);
public:
    int rockets = 0;
    int bullets;
    Player(GLuint textID);
    void Update(float deltaTime, std::vector<Projectile*>& eb, std::vector<Powerup*>& pups, Map *map);
};

#endif /* Player_h */
