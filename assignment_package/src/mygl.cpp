#include "mygl.h"
#include <glm_includes.h>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <qdatetime.h>
#include <QFileDialog>
#include "playerinfo.h"

MyGL::MyGL(QWidget *parent)
    : OpenGLContext(parent),
      m_geomQuad(this),
      m_worldAxes(this), m_progLambert(this), m_progFlat(this), m_progInstanced(this), m_progSky(this), m_terrain(this, models), m_player(glm::vec3(48.f, 200.f, 48.f), m_terrain), m_currMSecSinceEpoch(QDateTime::currentMSecsSinceEpoch()),
      m_time(0.f), m_frameBuffer(this, this->width(), this->height(), this->devicePixelRatio()), m_progWaterTinge(this),
      m_progLavaTinge(this), m_texture(this), models(),
      liquid(0), swimming(), flying(), openInventory(false), numGrass(10), numDirt(10), numStone(10),
      numSand(10), numWood(10), numLeaf(10), numSnow(10), numIce(10),
      currBlockType(GRASS), m_progTexture(this)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(tick()));
    // Tell the timer to redraw 60 times per second
    m_timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
    swimming.setSource(QUrl(":/swim.wav"));
    swimming.setVolume(0.8f);

    flying.setSource(QUrl(":/walk.mp3"));
    flying.setVolume(0.25f);
    setMouseTracking(true); // MyGL will track the mouse's movements even if a mouse button is not pressed
    setCursor(Qt::BlankCursor); // Make the cursor invisible
}

MyGL::~MyGL() {
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
    m_geomQuad.destroyVBOdata();
//    npc.destroyVBOdata();
}


void MyGL::moveMouseToCenter() {
    QCursor::setPos(this->mapToGlobal(QPoint(width() / 2, height() / 2)));
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_LINE_SMOOTH);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1);

    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    m_frameBuffer.create();

    m_geomQuad.createVBOdata();

    //Create the instance of the world axes
    m_worldAxes.createVBOdata();

    // Create and set up the diffuse shader
    m_progLambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    m_progFlat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    //m_progInstanced.create(":/glsl/instanced.vert.glsl", ":/glsl/lambert.frag.glsl");
    m_progTexture.create(":/glsl/flat.vert.glsl", ":/glsl/texture.frag.glsl");
    m_progWaterTinge.create(":/glsl/passthrough.vert.glsl", ":/glsl/water.frag.glsl");
    m_progLavaTinge.create(":/glsl/passthrough.vert.glsl", ":/glsl/lava.frag.glsl");
    m_progSky.create(":/glsl/sky.vert.glsl", ":/glsl/sky.frag.glsl");

    // Set a color with which to draw geometry.
    // This will ultimately not be used when you change
    // your program to render Chunks with vertex colors
    // and UV coordinates
    m_progLambert.setGeometryColor(glm::vec4(0,1,0,1));

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

//    m_terrain.CreateScene();
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    //m_texture.create(":/textures/minecraft_textures_all.png");
    m_texture.create(":/textures/test.png");
    m_texture.load(0);
    uPtr<Model> cowModel = mkU<Model>(this, "../assignment_package/objects/minecraft_cow.obj", "../assignment_package/textures/minecraft_cow.jpg");
    models.push_back(std::move(cowModel));
    uPtr<Model> sheepModel = mkU<Model>(this, "../assignment_package/objects/sheep.obj", "../assignment_package/textures/sheep.png");
    models.push_back(std::move(sheepModel));
    uPtr<Model> chickenModel = mkU<Model>(this, "../assignment_package/objects/chicken.obj", "../assignment_package/textures/chicken.png");
    models.push_back(std::move(chickenModel));
    uPtr<Model> horseModel = mkU<Model>(this, "../assignment_package/objects/horse.obj", "../assignment_package/textures/horse.png");
    models.push_back(std::move(horseModel));
    uPtr<Model> pigModel = mkU<Model>(this, "../assignment_package/objects/pig.obj", "../assignment_package/textures/pig.png");
    models.push_back(std::move(pigModel));
    uPtr<Model> squidModel = mkU<Model>(this, "../assignment_package/objects/squid.obj", "../assignment_package/textures/squid.png");
    models.push_back(std::move(squidModel));
    uPtr<Model> mooshroomModel = mkU<Model>(this, "../assignment_package/objects/mooshroom.obj", "../assignment_package/textures/mooshroom.png");
    models.push_back(std::move(mooshroomModel));
}

void MyGL::resizeGL(int w, int h) {
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.
    m_player.setCameraWidthHeight(static_cast<unsigned int>(w), static_cast<unsigned int>(h));
    glm::mat4 viewproj = m_player.mcr_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)

    m_progLambert.setViewProjMatrix(viewproj);
    m_progFlat.setViewProjMatrix(viewproj);
    m_progTexture.setViewProjMatrix(viewproj);

    m_progLavaTinge.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));
    m_progWaterTinge.setDimensions(glm::ivec2(w * this->devicePixelRatio(), h * this->devicePixelRatio()));

    m_frameBuffer.resize(this->width(), this->height(), this->devicePixelRatio());
    m_frameBuffer.destroy();
    m_frameBuffer.create();

    //pixels -> world
    m_progSky.setViewProjMatrix(glm::inverse(viewproj));
    //screen size
    m_progSky.setDimensions(glm::ivec2(width(), height()));
    //camera pos
    m_progSky.setEye(m_player.mcr_camera.mcr_position);

    printGLErrorLog();
}


// MyGL's constructor links tick() to a timer that fires 60 times per second.
// We're treating MyGL as our game engine class, so we're going to perform
// all per-frame actions here, such as performing physics updates on all
// entities in the scene.
void MyGL::tick() {
    m_player.mcr_prevposition = m_player.mcr_position;
    float dT = (QDateTime::currentMSecsSinceEpoch() - m_currMSecSinceEpoch) / 200.f;
    m_player.tick(dT, m_inputs);

    m_currMSecSinceEpoch = QDateTime::currentMSecsSinceEpoch();
    //tick NPCs in chunks around player
    glm::vec2 chunkOrigin = glm::vec2(floor(m_player.mcr_position.x / 16.f) * 16,
                                      floor(m_player.mcr_position.z / 16.f) * 16);
    for(int x = chunkOrigin.x - 128; x < chunkOrigin.x + 128; x += 16) {
        for(int z = chunkOrigin.y - 128; z < chunkOrigin.y + 128; z += 16) {
            if(m_terrain.hasChunkAt(x, z)){
                const uPtr<Chunk> &chunk = m_terrain.getChunkAt(x, z);
                for (int i = 0; i < (int) chunk->npcs.size(); i++) {
                    chunk->npcs[i]->tick(dT, m_terrain);
                }
            }
        }
    }
    printGLErrorLog();
    m_terrain.generateTerrain(m_player.mcr_position, m_player.mcr_prevposition);
    printGLErrorLog();
    update(); // Calls paintGL() as part of a larger QOpenGLWidget pipeline
    sendPlayerDataToGUI(); // Updates the info in the secondary window displaying player data
    sendInventoryDataToGUI();
}

void MyGL::sendPlayerDataToGUI() const {
    emit sig_sendPlayerPos(m_player.posAsQString());
    emit sig_sendPlayerVel(m_player.velAsQString());
    emit sig_sendPlayerAcc(m_player.accAsQString());
    emit sig_sendPlayerLook(m_player.lookAsQString());
    glm::vec2 pPos(m_player.mcr_position.x, m_player.mcr_position.z);
    glm::ivec2 chunk(16 * glm::ivec2(glm::floor(pPos / 16.f)));
    glm::ivec2 zone(64 * glm::ivec2(glm::floor(pPos / 64.f)));
    emit sig_sendPlayerChunk(QString::fromStdString("( " + std::to_string(chunk.x) + ", " + std::to_string(chunk.y) + " )"));
    emit sig_sendPlayerTerrainZone(QString::fromStdString("( " + std::to_string(zone.x) + ", " + std::to_string(zone.y) + " )"));
}

void MyGL::sendInventoryDataToGUI() const {
    emit sig_sendNumGrass(numGrass);
    emit sig_sendNumDirt(numDirt);
    emit sig_sendNumStone(numStone);
    emit sig_sendNumSand(numSand);
    emit sig_sendNumWood(numWood);
    emit sig_sendNumLeaf(numLeaf);
    emit sig_sendNumIce(numIce);
    emit sig_sendNumSnow(numSnow);
}

// This function is called whenever update() is called.
// MyGL's constructor links update() to a timer that fires 60 times per second,
// so paintGL() called at a rate of 60 frames per second.
void MyGL::paintGL() {
    // Clear the screen so that we only see newly drawn images
//    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    liquid = m_player.headInLiquid(m_terrain);
    //check if camera in water
    if (liquid != 0) {
//        if (!swimming.isPlaying()){
//            swimming.play();
//        }
//        std::cout << "in water" << std::endl;
        // Render to our framebuffer rather than the viewport
        m_frameBuffer.bindFrameBuffer();
        // Render on the whole framebuffer, complete from the lower left corner to the upper right
        glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    m_progFlat.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progLambert.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    m_progTexture.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    //m_progInstanced.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    renderNPCs();
    //pixels to world
    m_progSky.setViewProjMatrix(glm::inverse(m_player.mcr_camera.getViewProj()));
//    m_progInstanced.setViewProjMatrix(m_player.mcr_camera.getViewProj());
    //set camera pos
    m_progSky.setEye(m_player.mcr_camera.mcr_position);
    //set time
    m_progSky.setTime(m_time);

//    m_frameBuffer.bindToTextureSlot(1);//212121
    renderTerrain();
    //draw the box
    m_progSky.draw(m_geomQuad, 1);

    m_progLambert.setTime(m_time);
  
    if (liquid != 0) {
        performPostprocessRenderPass();
    }

    m_progLambert.setPlayerPosition(glm::vec4(m_player.mcr_position.x,
                                                  m_player.mcr_position.y,
                                                  m_player.mcr_position.z, 0));

//    if (npc.isBound) {
////        m_progSkeleton.draw(npc);
//    } else {
//        m_progFlat.draw(npc, 0);
//    }

    m_progLavaTinge.setTime(m_time);
    m_progWaterTinge.setTime(m_time);
    m_time++;
    glDisable(GL_DEPTH_TEST);
    m_progFlat.setModelMatrix(glm::mat4());
    m_progFlat.draw(m_worldAxes, 0);
    glEnable(GL_DEPTH_TEST);

//    if (m_time == 1000) {
//        m_terrain.drawStuff();
//    }

}

// TODO: Change this so it renders the nine zones of generated
// terrain that surround the player (refer to Terrain::m_generatedTerrain
// for more info)
void MyGL::renderTerrain() {
    m_texture.bind(0);
    glm::vec2 chunkOrigin = glm::vec2(floor(m_player.mcr_position.x / 16.f) * 16,
                                      floor(m_player.mcr_position.z / 16.f) * 16);
    m_terrain.draw(chunkOrigin.x - 128, chunkOrigin.x + 128, chunkOrigin.y - 128, chunkOrigin.y + 128, &m_progLambert);
}

void MyGL::renderNPCs() {
    glm::vec2 chunkOrigin = glm::vec2(floor(m_player.mcr_position.x / 16.f) * 16,
                                      floor(m_player.mcr_position.z / 16.f) * 16);
    for(int x = chunkOrigin.x - 128; x < chunkOrigin.x + 128; x += 16) {
        for(int z = chunkOrigin.y - 128; z < chunkOrigin.y + 128; z += 16) {
            if(m_terrain.hasChunkAt(x, z)){
                const uPtr<Chunk> &chunk = m_terrain.getChunkAt(x, z);
                for (int i = 0; i < (int) chunk->npcs.size(); i++) {
                    chunk->npcs[i]->m_model->bindTexture();
                    m_progFlat.draw(*(chunk->npcs[i].get()), 0);
                }
            }
        }
    }
}

void MyGL::performPostprocessRenderPass()
{
    // Render the frame buffer as a texture on a screen-size quad

    // Tell OpenGL to render to the viewport's frame buffer
    glBindFramebuffer(GL_FRAMEBUFFER, this->defaultFramebufferObject());
    // Render on the whole framebuffer, complete from the lower left corner to the upper right
    glViewport(0,0,this->width() * this->devicePixelRatio(), this->height() * this->devicePixelRatio());
    // Clear the screen
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // Bind texture
    m_frameBuffer.bindToTextureSlot(21);//212121

    if (liquid == 1) {
        m_progWaterTinge.draw(m_geomQuad, 21);
    } else if (liquid == 2) {
        m_progLavaTinge.draw(m_geomQuad, 21);
    }
}


void MyGL::keyPressEvent(QKeyEvent *e) {
    float amount = 2.0f;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum
    // This could all be much more efficient if a switch
    // statement were used, but I really dislike their
    // syntax so I chose to be lazy and use a long
    // chain of if statements instead
    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    } else if (e->key() == Qt::Key_Right) {
        m_player.rotateOnUpGlobal(-amount);
    } else if (e->key() == Qt::Key_Left) {
        m_player.rotateOnUpGlobal(amount);
    } else if (e->key() == Qt::Key_Up) {
        m_player.rotateOnRightLocal(-amount);
    } else if (e->key() == Qt::Key_Down) {
        m_player.rotateOnRightLocal(amount);
    } else if (e->key() == Qt::Key_W) {
        m_inputs.wPressed = true;
    } else if (e->key() == Qt::Key_S) {
        m_inputs.sPressed = true;
    } else if (e->key() == Qt::Key_D) {
        m_inputs.dPressed = true;
    } else if (e->key() == Qt::Key_A) {
        m_inputs.aPressed = true;
    } else if (e->key() == Qt::Key_F) {
        m_inputs.flightMode = !m_inputs.flightMode;
//        if (!flying.isPlaying()){
//            flying.play();
//        }
    } else if (e->key() == Qt::Key_I) {
        openInventory = !openInventory;
        emit sig_inventoryOpenClose(openInventory);
    }
    if (m_inputs.flightMode) {
        if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = true;
        } else if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = true;
        }
    } else {
        if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = true;
        }
    }
}

void MyGL::keyReleaseEvent(QKeyEvent *e) {
    if (!e->isAutoRepeat()) {
        if (e->key() == Qt::Key_W) {
            m_inputs.wPressed = false;
        } else if (e->key() == Qt::Key_S) {
            m_inputs.sPressed = false;
        } else if (e->key() == Qt::Key_D) {
            m_inputs.dPressed = false;
        } else if (e->key() == Qt::Key_A) {
            m_inputs.aPressed = false;
        } else if (e->key() == Qt::Key_Q) {
            m_inputs.qPressed = false;
        } else if (e->key() == Qt::Key_E) {
            m_inputs.ePressed = false;
        } else if (e->key() == Qt::Key_Space) {
            m_inputs.spacePressed = false;
        }
    }
}

void MyGL::mouseMoveEvent(QMouseEvent *e) {
    float dx = (this->width() * 0.5 - e->pos().x()) / width();
    float dy = (this->height() * 0.5 - e->pos().y()) / height();
    m_player.rotateOnUpGlobal(dx * 20.f);
    m_player.rotateOnRightLocal(dy * 20.f);
    moveMouseToCenter();
}

void MyGL::mousePressEvent(QMouseEvent *e) {
    if (e->button() == Qt::LeftButton) {
            BlockType removed = m_player.removeBlock(&m_terrain);
            if (removed == GRASS) {
                numGrass++;
            } else if (removed == DIRT) {
                numDirt++;
            } else if (removed == STONE) {
                numStone++;
            } else if (removed == SAND) {
                numSand++;
            } else if (removed == WOOD) {
                numWood++;
            } else if (removed == LEAF) {
                numLeaf++;
            } else if (removed == SNOW) {
                numSnow++;
            } else if (removed == ICE) {
                numIce++;
            }
        } else if (e->button() == Qt::RightButton) {
            if (currBlockType == GRASS && numGrass > 0 && m_player.placeBlock(&m_terrain, GRASS) != EMPTY) {
                numGrass--;
            } else if (currBlockType == DIRT && numDirt > 0 && m_player.placeBlock(&m_terrain, DIRT) != EMPTY) {
                numDirt--;
            } else if (currBlockType == STONE && numStone > 0 && m_player.placeBlock(&m_terrain, STONE) != EMPTY) {
                numStone--;
            } else if (currBlockType == SAND && numSand > 0 && m_player.placeBlock(&m_terrain, SAND) != EMPTY) {
                numSand--;
            } else if (currBlockType == WOOD && numWood > 0 && m_player.placeBlock(&m_terrain, WOOD) != EMPTY) {
                numWood--;
            } else if (currBlockType == LEAF && numLeaf > 0 && m_player.placeBlock(&m_terrain, LEAF) != EMPTY) {
                numLeaf--;
            } else if (currBlockType == SNOW && numSnow > 0 && m_player.placeBlock(&m_terrain, SNOW) != EMPTY) {
                numSnow--;
            } else if (currBlockType == ICE && numIce > 0 && m_player.placeBlock(&m_terrain, ICE) != EMPTY) {
                numIce--;
            }
        }
}
