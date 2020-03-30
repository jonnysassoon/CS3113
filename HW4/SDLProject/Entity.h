#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"
#include <deque>

enum EntityType { PLAYER, PLATFORM, ENEMY, GUN, BULLET};
enum AIType { WALKER, SHOOTER, JUMPER};
enum AIState { IDLE, WALKING, SHOOTING};

class Entity {
public:
    
    EntityType entityType;
    AIType aiType;
    AIState aiState;
    
    Entity *lastCollision;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = 1.0f;
    float height = 1.0f;

    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    int *animRight = NULL;
    int *animLeft = NULL;
    int *animUp = NULL;
    int *animDown = NULL;

    int *animIndices = NULL;
    int animFrames = 0;
    int animIndex = 0;
    float animTime = 0;
    int animCols = 0;
    int animRows = 0;
    
    bool isActive = true;
    bool collidedTop = false;
    bool collidedBottom = false;
    bool collidedLeft = false;
    bool collidedRight = false;
    
    int shotTime = 0;
    
    Entity();
    
    bool checkCollision(Entity *other);
    void checkCollisionsY(Entity *objects, int objectCount);
    void checkCollisionsX(Entity *objects, int objectCount);
    void checkBulletCollisionY(std::deque<Entity*> *bullets);
    void checkBulletCollisionX(std::deque<Entity*> *bullets);
    void Update(Entity* player, float deltaTime,
                Entity *platforms, int platformCount,
                Entity *enemy, int *enemiesKilled,
                int ellapsedTime, std::deque<Entity*> *bullets, GLuint bulletTexture);
    void Render(ShaderProgram *program);
    void DrawSpriteFromTextureAtlas(ShaderProgram *program, GLuint textureID, int index);
    
    void AI(Entity *player, int ellapsedTime, std::deque<Entity*> *bullets, GLuint bulletTexture);
    void AIWalker();
    void AIShooter(Entity *player, int ellapsedTime, std::deque<Entity*> *bullets, GLuint bulletTexture);
    void AIJumper(Entity *player, int ellapsedTime);
};
