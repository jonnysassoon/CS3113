#include "Entity.h"

Entity::Entity() {
    position = glm::vec3(0);
    speed = 0;
    
    modelMatrix = glm::mat4(1.0f);
    movement = glm::vec3(0.0f);
    acceleration = glm::vec3(0.0f);
    velocity = glm::vec3(0.0f);

}

bool Entity::checkCollision(Entity *other) {
    if (!isActive or !other->isActive) return false;
    
    float xdist = fabs(position.x - other->position.x) - ((width + other->width) / 2.0f);
    float ydist = fabs(position.y - other->position.y) - ((height + other->height) / 2.0f);

    if (xdist < 0 && ydist < 0) {
        lastCollision = other;
        other->lastCollision = this;
        return true;
    }
    return false;
}

void Entity::checkBulletCollisionY(std::deque<Entity*> *bullets) {
    if (bullets == NULL) return;
    for (size_t i = 0; i < bullets->size(); i++) {
        Entity *object = (*bullets)[i];
        if (checkCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                object->collidedBottom = true;
            } else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                object->collidedTop = true;
            }
        }
    }
}

void Entity::checkBulletCollisionX(std::deque<Entity*> *bullets) {
    if (bullets == NULL) return;
    for (size_t i = 0; i < bullets->size(); i++) {
        Entity *object = (*bullets)[i];
        if (checkCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                object->collidedLeft = true;
            } else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                object->collidedRight = true;
            }
        }
    }
}

void Entity::checkCollisionsY(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        if (checkCollision(object)) {
            float ydist = fabs(position.y - object->position.y);
            float penetrationY = fabs(ydist - (height / 2.0f) - (object->height / 2.0f));
            if (velocity.y > 0) {
                position.y -= penetrationY;
                velocity.y = 0;
                collidedTop = true;
                object->collidedBottom = true;
            } else if (velocity.y < 0) {
                position.y += penetrationY;
                velocity.y = 0;
                collidedBottom = true;
                object->collidedTop = true;
            }
        }
    }
}

void Entity::checkCollisionsX(Entity *objects, int objectCount) {
    for (int i = 0; i < objectCount; i++) {
        Entity *object = &objects[i];
        if (checkCollision(object)) {
            float xdist = fabs(position.x - object->position.x);
            float penetrationX = fabs(xdist - (width / 2.0f) - (object->width / 2.0f));
            if (velocity.x > 0) {
                position.x -= penetrationX;
                velocity.x = 0;
                collidedRight = true;
                object->collidedLeft = true;
            } else if (velocity.x < 0) {
                position.x += penetrationX;
                velocity.x = 0;
                collidedLeft = true;
                object->collidedRight = true;
            }
        }
    }
}

void Entity::AIWalker() {
    movement = glm::vec3(-1, 0, 0);
}

void Entity::AIShooter(Entity *player, int ellapsedTime, std::deque<Entity*> *bullets, GLuint bulletTexture) {
    switch (aiState) {
        case IDLE:
            if (ellapsedTime % 2 == 0 && ellapsedTime > shotTime) {
                aiState = SHOOTING;
            }
            break;
        case WALKING:
            break;
        case SHOOTING:
            Entity *bullet = new Entity();
            bullet->position = glm::vec3(this->position.x - .5, this->position.y, 0);
            bullet->movement = glm::vec3(-1, 0, 0);
            bullet->height = .1;
            bullet->width = .1;
            bullet->speed = 1;
            bullet->textureID = bulletTexture;
            bullet->entityType = BULLET;
            bullets->push_back(bullet);
            shotTime = ellapsedTime;
            aiState = IDLE;
            break;
    }
}

void Entity::AIJumper(Entity *player, int ellapsedTime) {
    if (ellapsedTime % 3 == 0 && ellapsedTime > shotTime) {
        if (collidedBottom) {
            if (player->position.x > position.x) {
                movement.x = 1;
                velocity = glm::vec3(0, 4, 0);
            } else {
                movement.x = -1;
                velocity = glm::vec3(0, 4, 0);
            }
            shotTime = ellapsedTime;
        }
    } else if (collidedBottom) {
        movement = glm::vec3(0);
        velocity = glm::vec3(0);
        acceleration = glm::vec3(0, -3, 0);
    }
}

void Entity::AI(Entity *player, int ellapsedTime, std::deque<Entity*> *bullets, GLuint bulletTexture) {
    switch (aiType) {
        case WALKER:
            AIWalker();
            break;
        case SHOOTER:
            AIShooter(player, ellapsedTime, bullets, bulletTexture);
            break;
        case JUMPER:
            AIJumper(player, ellapsedTime);
            break;
    }
}

void Entity::Update(Entity* player, float deltaTime,
                    Entity *platforms, int platformCount,
                    Entity *enemy, int *enemiesKilled,
                    int ellapsedTime, std::deque<Entity*> *bullets, GLuint bulletTexture) {
    
    
    if (!isActive) return;
    
    collidedTop = false;
    collidedBottom = false;
    collidedLeft = false;
    collidedRight = false;
    
    if (animIndices != NULL) {
        if (glm::length(movement) != 0) {
            animTime += deltaTime;

            if (animTime >= 0.25f)
            {
                animTime = 0.0f;
                animIndex++;
                if (animIndex >= animFrames)
                {
                    animIndex = 0;
                }
            }
        } else {
            animIndex = 0;
        }
    }
    
    velocity.x = movement.x * speed;
    velocity += acceleration * deltaTime;
    
    position.y += velocity.y * deltaTime;
    checkCollisionsY(platforms, platformCount);
    if (entityType != BULLET)
        checkBulletCollisionY(bullets);
    if (enemy != NULL)
        checkCollisionsY(enemy, 1);
        
    position.x += velocity.x * deltaTime;
    checkCollisionsX(platforms, platformCount);
    if (entityType != BULLET)
        checkBulletCollisionX(bullets);
    
    if (enemy != NULL)
        checkCollisionsX(enemy, 1);
    
    if (entityType == PLAYER) {
        if (collidedLeft || collidedRight || collidedTop ||
            (collidedBottom && lastCollision->entityType == BULLET)) {
            isActive = false;
        } else if (collidedBottom && lastCollision->entityType == ENEMY) {
            lastCollision->isActive = false;
            *enemiesKilled += 1;
        }
    }
    if (entityType == ENEMY) {
        if ((collidedLeft || collidedRight) && lastCollision->entityType == PLAYER) {
            lastCollision->isActive = false;
        } else if ((collidedTop && lastCollision->entityType == PLAYER) ||
                 ((collidedLeft || collidedRight) && lastCollision->entityType == BULLET)) {
            isActive = false;
            *enemiesKilled += 1;
        }
    }
    
    if (entityType == ENEMY) {
        AI(player, ellapsedTime, bullets, bulletTexture);
    }
    modelMatrix = glm::mat4(1.0f);
    modelMatrix = glm::translate(modelMatrix, position);
    
}

void Entity::Render(ShaderProgram *program) {
    if (!isActive) return;
    if (entityType == BULLET && position.x > 3.45) return;
    
    program->SetModelMatrix(modelMatrix);
    
    if (animIndices != NULL) {
        DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
        return;
    }
    
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    float verticesB[] = {-0.05f, -0.05f, 0.05f, 0.05f, -0.05f, 0.05f, -0.05f, -0.05f, 0.05f, -0.05f, 0.05f, 0.05f};
    float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
    
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, entityType == BULLET ? verticesB : vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index) {
    int rows = 4;
    int cols = 4;
    
    float u = (float)(index % cols) / (float)cols;
    float v = (float)(index / cols) / (float)rows;
    float width = 1.0f / (float)cols;
    float height = 1.0f / (float)rows;
    float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
        u, v + height, u + width, v, u, v};
    float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
    glBindTexture(GL_TEXTURE_2D, textureID);
    glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
    glEnableVertexAttribArray(program->positionAttribute);
    glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
    glEnableVertexAttribArray(program->texCoordAttribute);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDisableVertexAttribArray(program->positionAttribute);
    glDisableVertexAttribArray(program->texCoordAttribute);
}
