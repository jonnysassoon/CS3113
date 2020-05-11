//
//  Powerup.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Powerup_h
#define Powerup_h

#include "Entity.h"

enum PowerupType {BULLETS, ROCKETS, HEALTH};

class Powerup : public Entity {
public:
    PowerupType type;
    Powerup(GLuint tid, PowerupType t, glm::vec3 pos);
};


#endif /* Powerup_h */
