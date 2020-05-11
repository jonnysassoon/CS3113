//
//  Copter.h
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#ifndef Copter_h
#define Copter_h

#include "Entity.h"

class Copter : public Entity {
    int health;
public:
    Copter();
    void takeDamage(int damage);
    int getHealth() const;
    void addToHealth(int toAdd);
    
    void Render(ShaderProgram *program);
};

#endif /* Copter_h */
