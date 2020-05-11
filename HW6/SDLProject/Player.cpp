//
//  Player.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Player.h"

Player::Player(GLuint textID) : Copter() {
    textureID = textID;
    lastCollision = NULL;
    position = glm::vec3(1, -4, 0);
    movement = glm::vec3(1, 0, 0);
    acceleration = glm::vec3(0.0f, -5.0f, 0.0f);
    speed = 3.0f;
    entityType = EntityType::PLAYER;
    width = height = .75;
    bullets = 5;
}

void Player::Update(float deltaTime, std::vector<Projectile*>& eb, std::vector<Powerup*>& pups, Map *map) {
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    lastCollision = NULL;
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    checkCollisionsY(map);
    checkCollisionsX(map);
    
    for (Powerup* p : pups) {
        if (checkCollision(p)) {
            givePowerup(p);
            p->isActive = false;
        }
    }
    
    for (Projectile* p : eb) {
        if (checkCollision(p)) {
            takeDamage(p->power);
            p->isActive = false;
            if (getHealth() <= 0)
                isActive = false;
        }
    }
    
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
}

void Player::givePowerup(Powerup* p) {
    switch(p->type) {
        case BULLETS:
            bullets += 5;
            break;
        case ROCKETS:
            rockets += 2;
            break;
        case HEALTH:
            if (getHealth() < 100) {
                if (getHealth() > 90) {
                    addToHealth(100 - getHealth());
                }
                else {
                    addToHealth(10);
                }
            }
            break;
    }
}
