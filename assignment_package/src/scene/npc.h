#pragma once
#include "entity.h"
#include "drawable.h"
#include "face.h"
#include "vertex.h"
#include "halfedge.h"
#include "joint.h"
#include "terrain.h"
#include "texture.h"
#include <QJsonDocument>
#include <QJsonParseError>
#include <QJsonObject>
#include <QJsonValue>
#include <QJsonArray>
#include "model.h"

class Terrain;
class Chunk;
class NPC: public Entity, public Drawable
{
public:
    NPC(OpenGLContext* context, glm::vec3 pos, Chunk* chunk, Model* model, int modelIndex);

    Model* m_model;

    void createVBOdata();

    void tick(float dT, const Terrain &terrain);
    int ticksSinceLastDecision;
    int ticksUntilNextDecision;
    glm::vec3 decisionDirection;
    glm::vec3 m_velocity, m_acceleration;
    int interfaceAx;
    int ticks;
    int rotDegrees;
    bool afk;
    Chunk* chunk;

    static inline int objCount{ 0 };
    int id;
    int modelIndex;

private:
    void computePhysics(float dT, const Terrain &terrain);
    bool onGround(const Terrain &terrain);
    void detectCollision(glm::vec3 *rayDir, const Terrain &terrain);
    bool collision(glm::vec3 *rayDir, const Terrain &terrain);
    bool gridMarch(glm::vec3 rayStart, glm::vec3 rayDir,
                   const Terrain &terrain, float *out_dist,
                   glm::ivec3 *out_blockHit);
    bool bodyInLiquid(const Terrain &terrain);
};



