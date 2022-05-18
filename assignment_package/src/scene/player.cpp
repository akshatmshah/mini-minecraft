#include "player.h"
#include <QString>
#include <iostream>

Player::Player(glm::vec3 pos, const Terrain &terrain)
    : Entity(pos), m_velocity(0, 0, 0), m_acceleration(0, 0, 0),
      m_camera(pos + glm::vec3(0, 1.5f, 0)), mcr_terrain(terrain),
      interfaceAx(-1), mcr_camera(m_camera), mcr_prevposition(0, 0, 0)
{
}

Player::~Player()
{
}

void Player::tick(float dT, InputBundle &input)
{
    processInputs(input, dT, mcr_terrain);
    computePhysics(dT, mcr_terrain, input);
}

void Player::processInputs(InputBundle &input, float dT, const Terrain &terrain)
{
    // TODO: Update the Player's velocity and acceleration based on the
    // state of the inputs.
    m_acceleration = glm::vec3(0.f);
    float a = 12.f;
    float b = 8.f;
    if (input.flightMode)
    {
        if (input.wPressed)
        {
            m_acceleration += this->m_forward * a * dT;
        }
        if (input.sPressed)
        {
            m_acceleration -= this->m_forward * a * dT;
        }
        if (input.dPressed)
        {
            m_acceleration += this->m_right * a * dT;
        }
        if (input.aPressed)
        {
            m_acceleration -= this->m_right * a * dT;
        }
        if (input.ePressed)
        {
            m_acceleration += this->m_up * a * dT;
        }
        if (input.qPressed)
        {
            m_acceleration -= this->m_up * a * dT;
        }
        if (!input.wPressed && !input.sPressed && !input.dPressed && !input.aPressed && !input.ePressed && !input.qPressed)
        {
            m_velocity = glm::vec3();
            m_acceleration = glm::vec3();
        }
    }
    else
    {
        if (input.wPressed)
        {
            m_acceleration += glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)) * b * dT;
        }
        if (input.sPressed)
        {
            m_acceleration -= glm::normalize(glm::vec3(m_forward.x, 0, m_forward.z)) * b * dT;
        }
        if (input.dPressed)
        {
            m_acceleration += glm::normalize(glm::vec3(m_right.x, 0, m_right.z)) * b * dT;
        }
        if (input.aPressed)
        {
            m_acceleration -= glm::normalize(glm::vec3(m_right.x, 0, m_right.z)) * b * dT;
        }
        if (onGround(input, terrain) && !input.wPressed && !input.sPressed && !input.dPressed && !input.aPressed)
        {
            m_velocity = glm::vec3();
            m_acceleration = glm::vec3();
        }
    }
}

void Player::computePhysics(float dT, const Terrain &terrain, InputBundle &input)
{
    // TODO: Update the Player's position based on its acceleration
    // and velocity, and also perform collision detection.

    if (input.flightMode) {
        //Max velocity cap
        if (glm::length(m_velocity) <= 10.f) {
            m_velocity += m_acceleration * dT;
        }
        glm::vec3 rayDirection = m_velocity * dT;
        this->moveAlongVector(rayDirection);
    }

    // handle collision & gravity while the player is in non-flightmode
    if (!input.flightMode) {
        //friction
        m_velocity *= .95f;
        bool isOnGround = onGround(input, terrain);
        if (bodyInLiquid(terrain)) {
            if (!isOnGround) {
                m_acceleration.y = -9.8f;
            }
            //Max velocity cap
            if (glm::length(m_velocity) <= 10.f) {
                m_velocity += m_acceleration * dT;
            }
            m_velocity *= 0.66;
            //constant vertical swimming speed
            if (input.spacePressed) {
                m_velocity.y = 5.f;
            }
        }
        //falling, acceleration is -g, update velocity
        else if (!isOnGround) {
            m_acceleration.y = -9.8f;
            //Max velocity cap
            if (glm::length(m_velocity) <= 10.f) {
                m_velocity += m_acceleration * dT;
            }
        }
        else if (isOnGround) {
            // jump, use g, initial velocity
            if (input.spacePressed) {
                m_velocity += glm::vec3(0, 5.f, 0);
            // walking on block
            } else {
                //Max velocity cap
                if (glm::length(m_velocity) <= 10.f) {
                    m_velocity += m_acceleration * dT;
                }
                m_velocity.y = 0.f;
            }
        }

        glm::vec3 rayDirection = m_velocity * dT;
        detectCollision(&rayDirection, terrain);
        this->moveAlongVector(rayDirection);
    }
}

bool Player::onGround(InputBundle &input, const Terrain &terrain)
{
    // position is center of bottom face of cube, bottom left corner move -.5 in x and -.5 in z
    glm::vec3 botLeftVertex = this->m_position - glm::vec3(0.5f, 0, 0.5f);
    // check all corners
    for (int i = 0; i <= 1; i++)
    {
        for (int k = 0; k <= 1; k++)
        {
            float x = .1f;
            float z = .1f;
            if (i == 1) {
                x = .9f;
            }
            if (k == 1) {
                z = .9f;
            }
            glm::vec3 p = glm::vec3(floor(botLeftVertex.x) + i, floor(botLeftVertex[1] - 0.1f),
                                    floor(botLeftVertex.z) + k);
            BlockType currBlock = terrain.getBlockAt(p);
            if (currBlock != EMPTY && currBlock != WATER && currBlock != LAVA && currBlock != GRASSDEC && currBlock != SHRUB && currBlock != DAISY && currBlock != ROSE && currBlock != REDMUSH && currBlock != BROWNMUSH)
            {
                return true;
            }
        }
    }
    return false;
}

bool Player::collision(glm::vec3 *rayDir, const Terrain &terrain)
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
            for (int j = 0; j <= 4; j++)
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

void Player::detectCollision(glm::vec3 *rayDir, const Terrain &terrain)
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
            for (int j = 0; j <= 4; j++)
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
bool Player::gridMarch(glm::vec3 rayOrigin, glm::vec3 rayDirection, const Terrain &terrain, float *out_dist, glm::ivec3 *out_blockHit)
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
        BlockType cellType = terrain.getBlockAt(currCell.x, currCell.y, currCell.z);

        if (cellType != EMPTY && cellType != WATER && cellType != LAVA && cellType != GRASSDEC && cellType != SHRUB && cellType != REDMUSH && cellType != BROWNMUSH && cellType != DAISY && cellType != ROSE)
        {
            *out_blockHit = currCell;
            *out_dist = glm::min(maxLen, curr_t);
            return true;
        }
    }

    *out_dist = glm::min(maxLen, curr_t);
    return false;
}

BlockType Player::removeBlock(Terrain *terrain)
{
    glm::vec3 rayOrigin = m_camera.mcr_position;
    // within 3 units of distance from the camera
    glm::vec3 rayDir = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 block = glm::ivec3();

    if (gridMarch(rayOrigin, rayDir, *terrain, &outDist, &block))
    {
        BlockType type = terrain->getBlockAt(block.x, block.y, block.z);
        terrain->setBlockAt(block.x, block.y, block.z, EMPTY);
        terrain->getChunkAt(block.x, block.z).get()->destroyVBOdata();
        terrain->getChunkAt(block.x, block.z).get()->createVBOdata();        
        auto& c = terrain->getChunkAt(block.x, block.z);
        c->createVBO(c->opaq, c->opaqIdx, c->trans, c->transIdx);
        return type;
    }
    return EMPTY;
}

BlockType Player::placeBlock(Terrain *terrain, BlockType currBlockType)
{
    glm::vec3 rayOrigin = m_camera.mcr_position;
    glm::vec3 rayDirection = 3.f * glm::normalize(this->m_forward);
    float outDist = 0.f;
    glm::ivec3 block = glm::ivec3();

        if (gridMarch(rayOrigin, rayDirection, *terrain, &outDist, &block)) {
            if (interfaceAx == 0) {
                //if (terrain->getBlockAt(block.x, block.y, block.z + glm::sign(rayDirection.z)) == EMPTY) {
                    terrain->setBlockAt(block.x, block.y, block.z - glm::sign(rayDirection.z), currBlockType);
                    terrain->getChunkAt(block.x, block.z - glm::sign(rayDirection.z)).get()->destroyVBOdata();
                    terrain->getChunkAt(block.x, block.z - glm::sign(rayDirection.z)).get()->createVBOdata();
                    auto& c = terrain->getChunkAt(block.x, block.z - glm::sign(rayDirection.z));
                    c->createVBO(c->opaq, c->opaqIdx, c->trans, c->transIdx);
                    //std::cout << "create z" << std::endl;
                    return currBlockType;
                //}
            } else if (interfaceAx == 1) {
                //if (terrain->getBlockAt(block.x, block.y + glm::sign(rayDirection.y), block.z) == EMPTY) {
                    terrain->setBlockAt(block.x, block.y - glm::sign(rayDirection.y), block.z, currBlockType);
                    terrain->getChunkAt(block.x, block.z).get()->destroyVBOdata();
                    terrain->getChunkAt(block.x, block.z).get()->createVBOdata();
                    auto& c = terrain->getChunkAt(block.x, block.z);
                    c->createVBO(c->opaq, c->opaqIdx, c->trans, c->transIdx);
                    //std::cout << "create y" << std::endl;
                    return currBlockType;
                //}
            } else if (interfaceAx == 2) {
                //if (terrain->getBlockAt(block.x + glm::sign(rayDirection.x), block.y, block.z) == EMPTY) {
                    terrain->setBlockAt(block.x - glm::sign(rayDirection.x), block.y, block.z, currBlockType);
                    terrain->getChunkAt(block.x - glm::sign(rayDirection.x), block.z).get()->destroyVBOdata();
                    terrain->getChunkAt(block.x - glm::sign(rayDirection.x), block.z).get()->createVBOdata();
                    auto& c = terrain->getChunkAt(block.x - glm::sign(rayDirection.x), block.z);
                    c->createVBO(c->opaq, c->opaqIdx, c->trans, c->transIdx);
                    //std::cout << "create x" << std::endl;
                    return currBlockType;
                 //}
            }
        }
        return EMPTY;
}

void Player::setCameraWidthHeight(unsigned int w, unsigned int h)
{
    m_camera.setWidthHeight(w, h);
}

void Player::moveAlongVector(glm::vec3 dir)
{
    Entity::moveAlongVector(dir);
    m_camera.moveAlongVector(dir);
}
void Player::moveForwardLocal(float amount)
{
    Entity::moveForwardLocal(amount);
    m_camera.moveForwardLocal(amount);
}
void Player::moveRightLocal(float amount)
{
    Entity::moveRightLocal(amount);
    m_camera.moveRightLocal(amount);
}
void Player::moveUpLocal(float amount)
{
    Entity::moveUpLocal(amount);
    m_camera.moveUpLocal(amount);
}
void Player::moveForwardGlobal(float amount)
{
    Entity::moveForwardGlobal(amount);
    m_camera.moveForwardGlobal(amount);
}
void Player::moveRightGlobal(float amount)
{
    Entity::moveRightGlobal(amount);
    m_camera.moveRightGlobal(amount);
}
void Player::moveUpGlobal(float amount)
{
    Entity::moveUpGlobal(amount);
    m_camera.moveUpGlobal(amount);
}
void Player::rotateOnForwardLocal(float degrees)
{
    Entity::rotateOnForwardLocal(degrees);
    m_camera.rotateOnForwardLocal(degrees);
}
void Player::rotateOnRightLocal(float degrees)
{
    float rad = glm::radians(degrees);
    float up = glm::radians(90.f);
    glm::mat4 rot(glm::rotate(glm::mat4(), rad, m_right));
    glm::vec3 newForward = glm::vec3(rot * glm::vec4(m_forward, 0.f));
    glm::vec3 newUp = glm::vec3(rot * glm::vec4(m_up, 0.f));
    //checking if applying the transformation causes camera to tilt vertically more than 90 degrees
    if (m_forward[1] != 1.f && newForward[1] > m_forward[1] && degrees < 0) {
        newForward[1] = -1.f;
    } else if (m_forward[1] != -1.f && newForward[1] < m_forward[1] && degrees > 0) {
        newForward[1] = 1.f;
    }
    //checking if hitting clamp
    if (newForward[1] != m_forward[1]) {
        m_forward = newForward;
        m_up = newUp;
        m_camera.setForward(newForward);
        m_camera.setUp(newUp);
    }

}
void Player::rotateOnUpLocal(float degrees)
{
    Entity::rotateOnUpLocal(degrees);
    m_camera.rotateOnUpLocal(degrees);

}
void Player::rotateOnForwardGlobal(float degrees)
{
    Entity::rotateOnForwardGlobal(degrees);
    m_camera.rotateOnForwardGlobal(degrees);
}
void Player::rotateOnRightGlobal(float degrees)
{
    Entity::rotateOnRightGlobal(degrees);
    m_camera.rotateOnRightGlobal(degrees);
}
void Player::rotateOnUpGlobal(float degrees)
{
    Entity::rotateOnUpGlobal(degrees);
    m_camera.rotateOnUpGlobal(degrees);
}

QString Player::posAsQString() const
{
    std::string str("( " + std::to_string(m_position.x) + ", " + std::to_string(m_position.y) + ", " + std::to_string(m_position.z) + ")");
    return QString::fromStdString(str);
}
QString Player::velAsQString() const
{
    std::string str("( " + std::to_string(m_velocity.x) + ", " + std::to_string(m_velocity.y) + ", " + std::to_string(m_velocity.z) + ")");
    return QString::fromStdString(str);
}
QString Player::accAsQString() const
{
    std::string str("( " + std::to_string(m_acceleration.x) + ", " + std::to_string(m_acceleration.y) + ", " + std::to_string(m_acceleration.z) + ")");
    return QString::fromStdString(str);
}
QString Player::lookAsQString() const
{
    std::string str("( " + std::to_string(m_forward.x) + ", " + std::to_string(m_forward.y) + ", " + std::to_string(m_forward.z) + ")");
    return QString::fromStdString(str);
}
int Player::headInLiquid(const Terrain &terrain) {
    glm::vec3 pos = glm::floor(this->m_position);
    pos.y++;
    try {
        BlockType currBlock = terrain.getBlockAt(pos);
        return currBlock == WATER ? 1 : currBlock == LAVA ? 2: 0;
    } catch (const std::out_of_range &e) {
        return 0;
    }

}
bool Player::bodyInLiquid(const Terrain &terrain) {
    glm::vec3 pos = glm::floor(this->m_position);
    try {
        BlockType currBlock = terrain.getBlockAt(pos);
        return currBlock == WATER || currBlock == LAVA;
    } catch (const std::out_of_range &e) {
        return false;
    }
}

