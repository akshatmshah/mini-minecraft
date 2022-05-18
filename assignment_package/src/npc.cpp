#include "npc.h"
#include <iostream>
#include <glm/gtx/io.hpp>


NPC::NPC(OpenGLContext* context, glm::vec3 pos, Chunk* chunk, Model* model) :
    Drawable(context), Entity(pos), ticks(0), rotDegrees(0), afk(false), chunk(chunk), id(objCount++),
    ticksSinceLastDecision(0), ticksUntilNextDecision(0), decisionDirection(), m_velocity(0, 0, 0), m_acceleration(0, 0, 0), interfaceAx(-1), m_model(model)
{
//    createFromOBJ(modelPath);
}

void NPC::createVBOdata()
{
    std::vector<glm::vec4> pos {};
    std::vector<glm::vec4> nor {};
    std::vector<glm::vec4> col {};
    std::vector<GLuint> idx {};
    std::vector<glm::vec2> weights {};
    std::vector<glm::ivec2> IDs {};
    std::vector<glm::vec2> uvs {};

    for(const uPtr<Face> &face: m_model->faces){
        HalfEdge* curr = face.get()->he;
        int startIdx = pos.size();
        int i = 0;
        do {
            //calculating normal
//            glm::vec4 normal;
//            HalfEdge* edge = curr;
//            do {
//                normal = glm::vec4(glm::cross(glm::vec3(edge->sym->vert->pos - edge->vert->pos), glm::vec3(edge->next->vert->pos - edge->vert->pos)), 0);
//                edge = edge->next;
//            } while (glm::length(normal) < FLT_EPSILON && edge != curr);

           pos.push_back(glm::rotate(glm::mat4(1.0f), glm::radians((float)rotDegrees), glm::vec3(0.0f, 1.0f, 0.0f)) * (curr->vert->pos + glm::vec4(0, 4, 0, 0)) + glm::vec4(m_position, 0));
//           nor.push_back(glm::normalize(normal));
           col.push_back(face.get()->color);
//           std::cout << curr->vert->id << std::endl;
//           std::cout << face->uvMap[curr->vert->id] << std::endl;
           uvs.push_back(face.get()->uvMap[curr->vert->id]);

           if (i >= 2) {
               idx.push_back(startIdx);
               idx.push_back(pos.size() - 2);
               idx.push_back(pos.size() - 1);
           }
           if (m_model->isBound) {
               weights.push_back(curr->vert->weights);
               IDs.push_back(curr->vert->IDs);
           }

           curr = curr->next;
           i++;
        } while (curr != face.get()->he);
    }

    m_count = idx.size();

    generateCol();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufCol);
    mp_context->glBufferData(GL_ARRAY_BUFFER, col.size() * sizeof(glm::vec4), col.data(), GL_STATIC_DRAW);

    generateIdx();
    mp_context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_bufIdx);
    mp_context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, idx.size() * sizeof(GLuint), idx.data(), GL_STATIC_DRAW);

    generatePos();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufPos);
    mp_context->glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec4), pos.data(), GL_STATIC_DRAW);

//    generateNor();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufNor);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, nor.size() * sizeof(glm::vec4), nor.data(), GL_STATIC_DRAW);

//    generateWeights();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufWeights);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, weights.size() * sizeof(glm::vec2), weights.data(), GL_STATIC_DRAW);

//    generateIDs();
//    mp_context->glBindBuffer(GL_ARRAY_BUFFER, bufIDs);
//    mp_context->glBufferData(GL_ARRAY_BUFFER, IDs.size() * sizeof(glm::ivec2), IDs.data(), GL_STATIC_DRAW);

    generateUV();
    mp_context->glBindBuffer(GL_ARRAY_BUFFER, m_bufUV);
    mp_context->glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(glm::vec2), uvs.data(), GL_STATIC_DRAW);

}

void NPC::tick(float dT, const Terrain &terrain) {
    if (ticksSinceLastDecision == ticksUntilNextDecision) {
        //Half the decisions are not moving
        if (rand() % 2 == 0) {
            afk = true;
        } else {
            afk = false;
        }
        int degrees = rand() % 361;
        rotDegrees += degrees;
        rotDegrees %= 360;
        rotateOnUpGlobal(degrees);
        ticksSinceLastDecision = 0;
        ticksUntilNextDecision = rand() % 200;
    }
    ticksSinceLastDecision++;
    ticks++;
    //wait a bit for chunks to load
    if (ticks > 100) {
        computePhysics(dT, terrain);
    }
    destroyVBOdata();
    createVBOdata();
}

void NPC::computePhysics(float dT, const Terrain &terrain)
{
    bool isOnGround = onGround(terrain);
    if (!afk) {
        m_velocity = m_forward;
    }
    glm::vec3 rayDirection;
    if (bodyInLiquid(terrain)) {
        m_acceleration = glm::vec3();
//        if (!isOnGround) {
//            m_acceleration.y = -9.8f;
//        }
        m_velocity *= 0.66;
        //constant vertical swimming speed
        m_velocity.y = 5.f;
        rayDirection = m_velocity * dT;
    }
    //falling, acceleration is -g, update velocity
    else if (!isOnGround) {
        m_acceleration.y = -9.8f;
        m_velocity += m_acceleration * dT;
        rayDirection = m_velocity * dT;
        //hitting head
        if (collision(&rayDirection, terrain)) {
            m_velocity.y = 0.f;
        }
        rayDirection = m_velocity * dT;
    }
    else if (isOnGround) {
        m_acceleration = glm::vec3();
        m_velocity.y = 0.f;
        rayDirection = m_velocity * dT;
        //jump
        if (collision(&rayDirection, terrain)) {
            m_velocity += glm::vec3(0, 5.f, 0);
        }
        rayDirection = m_velocity * dT;
    }
    detectCollision(&rayDirection, terrain);
    this->moveAlongVector(rayDirection);
    glm::vec3 pos = this->m_position;
    glm::vec2 chunkOrigin = glm::vec2((int) floor(pos[0] / 16.f) * 16,
                                      (int) floor(pos[2] / 16.f) * 16);
    if (chunkOrigin[0] != chunk->x || chunkOrigin[1] != chunk->z) {
        if (terrain.hasChunkAt(chunkOrigin[0], chunkOrigin[1])) {
            Chunk *newChunk = terrain.getChunkAt(chunkOrigin[0], chunkOrigin[1]).get();
            chunk->moveNPC(id, newChunk);
            chunk = newChunk;
        }
    }
}

bool NPC::onGround(const Terrain &terrain)
{
//    // position is center of bottom face of cube, bottom left corner move -.5 in x and -.5 in z
//    glm::vec3 botLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
//    // check all corners
//    for (int i = 0; i <= 1; i++)
//    {
//        for (int k = 0; k <= 1; k++)
//        {
//            float x = .1f;
//            float z = .1f;
//            if (i == 1) {
//                x = .9f;
//            }
//            if (k == 1) {
//                z = .9f;
//            }
//            glm::vec3 p = glm::vec3(floor(botLeftVertex.x) + i, floor(botLeftVertex[1] - 0.1f),
//                                    floor(botLeftVertex.z) + k);
//            BlockType currBlock = terrain.getBlockAt(p);
//            if (currBlock != EMPTY && currBlock != WATER && currBlock != LAVA)
//            {
//                return true;
//            }
//        }
//    }
//    return false;
    glm::vec3 pos = glm::floor(this->m_position);
    pos[1]--;
    try {
        BlockType currBlock = terrain.getBlockAt(pos);
        return currBlock != EMPTY && currBlock != WATER && currBlock != LAVA;
    } catch (const std::out_of_range &e) {
        return false;
    }
}

bool NPC::collision(glm::vec3 *rayDir, const Terrain &terrain)
{
    glm::vec3 botLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    glm::ivec3 out_blockHit = glm::ivec3();
    glm::vec3 x_vec = glm::vec3(rayDir->x, 0, 0);
    glm::vec3 z_vec = glm::vec3(0, 0, rayDir->z);
    float out_dist = 0.f;

    // x direction
    for (int i = 0; i <= 2; i++)
    {
        for (int k = 0; k <= 2; k++)
        {
            for (int j = 0; j <= 2; j++)
            {
                float x = 0.5f * i;
                float y = 0.5f * j;
                float z = 0.5f * k;
                glm::vec3 rayOrigin = botLeftVertex + glm::vec3(x, y, z);
                if (gridMarch(rayOrigin, x_vec, terrain, &out_dist, &out_blockHit))
                {
                    return true;
                }
                if (gridMarch(rayOrigin, z_vec, terrain, &out_dist, &out_blockHit))
                {
                    return true;
                }
            }
        }
    }
    return false;
}

void NPC::detectCollision(glm::vec3 *rayDir, const Terrain &terrain)
{
    glm::vec3 botLeftVertex = this->m_position - glm::vec3(0.5f, 0.f, 0.5f);
    glm::ivec3 out_blockHit = glm::ivec3();
    float out_dist = 0.f;
    // check 8 vertices in the 2 stacked cube structure
    // components of rayDir
    // want to check these individually
    glm::vec3 x_vec = glm::vec3(rayDir->x, 0, 0);
    glm::vec3 y_vec = glm::vec3(0, rayDir->y, 0);
    glm::vec3 z_vec = glm::vec3(0, 0, rayDir->z);

    for (int i = 0; i <= 2; i++)
    {
        for (int k = 0; k <= 2; k++)
        {
            for (int j = 0; j <= 2; j++)
            {
                float x = 0.5f * i;
                float y = 0.5f * j;
                float z = 0.5f * k;
                if (x == 1)
                {
                    x = .9f;
                }
                else if (x == 0)
                {
                    x = .1f;
                }
                if (z == 1)
                {
                    z = .9f;
                }
                else if (z == 0)
                {
                    z = .1f;
                }
                glm::vec3 rayOrigin = botLeftVertex + glm::vec3(x, y, z);
                if (gridMarch(rayOrigin, x_vec, terrain, &out_dist, &out_blockHit))
                {
                    float distance = out_dist - 0.005f;
                    x_vec = distance * glm::normalize(x_vec);
                }
                if (gridMarch(rayOrigin, y_vec, terrain, &out_dist, &out_blockHit))
                {
                    float distance = out_dist - 0.005f;
                    y_vec = distance * glm::normalize(y_vec);
                }
                if (gridMarch(rayOrigin, z_vec, terrain, &out_dist, &out_blockHit))
                {
                    float distance = out_dist - 0.005f;
                    z_vec = distance * glm::normalize(z_vec);
                }
            }
        }
    }
    *rayDir = glm::vec3(x_vec.x, y_vec.y, z_vec.z);
}

// from slides
bool NPC::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit)
{
    float maxLen = glm::length(rayDirection); // farthest we search
    glm::ivec3 currCell = glm::ivec3(glm::floor(rayOrigin));
    rayDirection = glm::normalize(rayDirection); // now all t values represent world dist

    float curr_t = 0.f;
    while (curr_t < maxLen)
    {
        float min_t = glm::sqrt(3.f);
        float interfaceAxis = -1; // Track axis for which t is smallest
        for (int i = 0; i < 3; ++i)
        { // Iterate over the three axes
            if (rayDirection[i] != 0)
            { // Is ray parallel to axis i?
                float offset = glm::max(0.f, glm::sign(rayDirection[i]));

                // If the player is *exactly* on an interface then
                // they'll never move if they're looking in a negative direction
                if (currCell[i] == rayOrigin[i] && offset == 0.f)
                {
                    offset = -1.f;
                }
                int nextIntercept = currCell[i] + offset;
                float axis_t = (nextIntercept - rayOrigin[i]) / rayDirection[i];
                axis_t = glm::min(axis_t, maxLen); // Clamp to max len to avoid super out of bounds errors
                if (axis_t < min_t)
                {
                    min_t = axis_t;
                    interfaceAxis = i;
                }
            }
        }
        if (interfaceAxis == -1)
        {
            throw std::out_of_range("interfaceAxis was -1 after the for loop in gridMarch!");
        }
        interfaceAx = interfaceAxis;
        curr_t += min_t;
        rayOrigin += rayDirection * min_t;
        glm::ivec3 offset = glm::ivec3(0, 0, 0);
        // Sets it to 0 if sign is +, -1 if sign is -
        offset[interfaceAxis] = glm::min(0.f, glm::sign(rayDirection[interfaceAxis]));
        currCell = glm::ivec3(glm::floor(rayOrigin)) + offset;
        // If the currCell contains something other than empty, return curr_t
        BlockType cellType;
        try
        {
            cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);
        }
        catch (const std::out_of_range &e)
        {
            //hopefully this makes npc's not go into unloaded chunks
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }

        if (cellType != EMPTY && cellType != WATER && cellType != LAVA)
        {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }

    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

bool NPC::bodyInLiquid(const Terrain &terrain) {
    glm::vec3 pos = glm::floor(this->m_position);
    try {
        BlockType currBlock = terrain.getBlockAt(pos);
        return currBlock == WATER || currBlock == LAVA;
    } catch (const std::out_of_range &e) {
        return false;
    }
}



