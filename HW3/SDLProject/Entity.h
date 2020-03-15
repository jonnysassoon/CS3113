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

//#define PLAYER 0
//#define PLATFORM 1
//#define TARGET 2

enum EntityType { PLAYER, PLATFORM, TARGET};

class Entity {
public:
    
    EntityType entityType;
    
    EntityType lastCollision;
    
    glm::vec3 position;
    glm::vec3 movement;
    glm::vec3 acceleration;
    glm::vec3 velocity;
    
    float width = 1.0f;
    float height = 1.0f;

    float speed;
    
    GLuint textureID;
    
    glm::mat4 modelMatrix;
    
    Entity();
    
    bool checkCollision(Entity *other);
    void checkCollisionsY(Entity *objects, int objectCount);
    void checkCollisionsX(Entity *objects, int objectCount);
    
    void Update(float deltaTime, Entity *platforms, int platformCount, Entity *target, int targetCount);
    void Render(ShaderProgram *program);
};
