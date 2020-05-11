//
//  Copter.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Copter.h"

Copter::Copter() : health(100) {}

void Copter::takeDamage(int damage) {
    health -= damage;
}

void Copter::addToHealth(int toAdd) {
    health += toAdd;
}

int Copter::getHealth() const { return health; }

void Copter::Render(ShaderProgram *program) {
    Entity::Render(program);
    
}
