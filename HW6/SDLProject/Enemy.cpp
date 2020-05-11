//
//  Enemy.cpp
//  SDLProject
//
//  Created by Jonathan Sassoon on 5/10/20.
//  Copyright © 2020 ctg. All rights reserved.
//

#include "Enemy.h"

Enemy::Enemy(GLuint textID) : Copter() {
    textureID = textID;
    lastCollision = NULL;
    entityType = EntityType::ENEMY;
    aiType = WALKER;
    aiState = IDLE;
    movement = glm::vec3(0, 1, 0);
    position = glm::vec3(8.5, -5, 0);
    acceleration = glm::vec3(0.0f, -10.0f, 0.0f);
    speed = 0.75f;
    width = height = .75;
}

void Enemy::Update(float deltaTime, std::vector<Projectile*>& projectiles, float playerY) {
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    lastCollision = NULL;
        
    if (playerY + .75 < position.y) {
        movement = glm::vec3(0, -1, 0);
    }
    if (playerY - .75 > position.y) {
        movement = glm::vec3(0, 1, 0);
    }
    
    velocity.y = movement.y * speed;
    if (!(position.y + velocity.y * deltaTime >= -2.2 || position.y + velocity.y * deltaTime <= -5))
        position.y += velocity.y * deltaTime;
    //    for (int i = 0; i < objectCount; i++)
    //        checkCollisionsY(&objects[i]);

    //    for (int i = 0; i < objectCount; i++)
    //        checkCollisionsX(&objects[i]);
    
    for (Projectile* p : projectiles) {
        if (checkCollision(p)) {
            p->isActive = false;
            takeDamage(p->power);
            if (getHealth() <= 0) {
                isActive = false;
            }
        }
    }
        
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    modelMatrix = glm::rotate(modelMatrix, 3.14f, glm::vec3(0,1,0));
}
