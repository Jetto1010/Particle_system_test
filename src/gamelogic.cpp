#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <SFML/Audio/SoundBuffer.hpp>
#include <utilities/shader.hpp>
#include <glm/vec3.hpp>
#include <iostream>
#include <utilities/timeutils.h>
#include <utilities/mesh.h>
#include <utilities/shapes.h>
#include <utilities/glutils.h>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <fmt/format.h>
#include "gamelogic.h"
#include "sceneGraph.hpp"
#include "particleSystem.hpp"
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/transform.hpp>

#include "utilities/imageLoader.hpp"
#include "utilities/glfont.h"
#include "utilities/camera.hpp"
#include "utilities/texture.h"

enum KeyFrameAction {
    BOTTOM, TOP
};

SceneNode* rootNode;
SceneNode* skyNode;
SceneNode* particleNode;

// These are heap allocated, because they should not be initialised at the start of the program
Gloom::Shader* shader;
Gloom::Shader* text2DShader;
Gloom::Shader* particleShader;
Gloom::Shader* skyBoxShader;

CommandLineOptions options;

struct LightSource {
    SceneNode* lightNode;
    glm::vec3 lightPosition;
    glm::vec3 colour;
};
int const numLights = 1;
LightSource lightSources[numLights];
glm::mat4 VP;
Gloom::Camera camera(glm::vec3(0, 0, 0));

void mouseCallback(GLFWwindow* window, double x, double y) {
    camera.handleCursorPosInput(x, y);
}
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mode) {
    camera.handleKeyboardInputs(key, action);
}

void initGame(GLFWwindow* window, CommandLineOptions gameOptions) {

    options = gameOptions;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);
    glfwSetCursorPosCallback(window, mouseCallback);
    glfwSetKeyCallback(window, keyCallback);

    shader = new Gloom::Shader();
    shader->makeBasicShader("../res/shaders/simple.vert", "../res/shaders/simple.frag");

    text2DShader = new Gloom::Shader();
    text2DShader->makeBasicShader("../res/shaders/texture2D.vert", "../res/shaders/texture2D.frag");

    particleShader = new Gloom::Shader();
    particleShader->makeBasicShader("../res/shaders/particles.vert", "../res/shaders/particles.frag");

    skyBoxShader = new Gloom::Shader();
    skyBoxShader->makeBasicShader("../res/shaders/skybox.vert", "../res/shaders/skybox.frag");

    // Particle
    PNGImage particleTex = loadPNGFile("../res/textures/fire.png");
    unsigned int particleTexID = getTextureID(&particleTex);

    // Skybox texture
    std::vector<std::string> skyboxImages{
            "../res/textures/skybox/right.png",
            "../res/textures/skybox/left.png",
            "../res/textures/skybox/top.png",
            "../res/textures/skybox/bottom.png",
            "../res/textures/skybox/front.png",
            "../res/textures/skybox/back.png"
    };

    // Create meshes
    Mesh skybox = cube(glm::vec3(360), glm::vec2(90), true, true);
    Mesh particle = generateParticle();

    // Fill buffers
    unsigned int skyVAO  = generateBuffer(skybox);
    unsigned int particleVAO = generateBuffer(particle);

    // Construct scene
    rootNode = createSceneNode();
    skyNode  = createSceneNode();
    particleNode = createSceneNode();

    skyNode->vertexArrayObjectID   = skyVAO;
    skyNode->VAOIndexCount         = skybox.indices.size();
    skyNode->nodeType              = SKYBOX;
    skyNode->position              = glm::vec3(0, 0, 0);
    skyNode->textureID             = getCubeMapID(skyboxImages);
    rootNode->children.push_back(skyNode);

    // Particles
   /* particleNode->nodeType = PARTICLE;
    particleNode->vertexArrayObjectID = particleVAO;
    particleNode->particleSystem = ParticleSystem(500);
    particleNode->VAOIndexCount = particle.indices.size();
    particleNode->position = glm::vec3(10);
    skyNode->children.push_back(particleNode);*/

    /*glm::vec3 vert[4] = {
            glm::vec3(0.0f, 0.0f,  0.0f),
            glm::vec3(0.0f, 1.0f,  0.0f),
            glm::vec3(1.0f, 0.0f, 0.0f),
            glm::vec3(1.0f, 1.0f, 0.0f)
    };

    glm::vec2 tex[4] = {
            glm::vec2(0.0f, 1.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f)
    };


    unsigned int  posVBO;
    unsigned int  texVBO;
    unsigned int  particleVAO;

    glGenVertexArrays(1, &particleVAO);

    glBindVertexArray(particleVAO);

    glGenBuffers(1, &posVBO);
    glBindBuffer(GL_ARRAY_BUFFER, posVBO);

    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec3), vert, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &texVBO);
    glBindBuffer(GL_ARRAY_BUFFER, texVBO);

    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(glm::vec2), tex, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(2);

    glBindVertexArray(0);*/

    particleNode->nodeType = PARTICLE;
    particleNode->vertexArrayObjectID = particleVAO;
    particleNode->particleSystem = ParticleSystem(1000);
    particleNode->textureID = particleTexID;
    particleNode->position = glm::vec3(0,0,-30);
    particleNode->VAOIndexCount = particle.indices.size();
    skyNode->children.push_back(particleNode);


    getTimeDeltaSeconds();

    std::cout << fmt::format("Initialized scene with {} SceneNodes.", totalChildren(rootNode)) << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();

    camera.updateCamera(timeDelta);

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 400.0f);
    glm::mat4 view = camera.getViewMatrix();
    VP = projection * view;

    updateNodeTransformations(rootNode, glm::mat4(1));
}

void updateNodeTransformations(SceneNode* node, glm::mat4 transformationThusFar) {
    glm::mat4 transformationMatrix =
              glm::translate(node->position)
            * glm::translate(node->referencePoint)
            * glm::rotate(node->rotation.y, glm::vec3(0,1,0))
            * glm::rotate(node->rotation.x, glm::vec3(1,0,0))
            * glm::rotate(node->rotation.z, glm::vec3(0,0,1))
            * glm::scale(node->scale)
            * glm::translate(-node->referencePoint);

    node->currentTransformationMatrix = transformationThusFar * transformationMatrix;
    node->MVP = VP * node->currentTransformationMatrix;

    switch(node->nodeType) {
        case POINT_LIGHT:
            lightSources[node->vertexArrayObjectID].lightPosition = glm::vec3(node->currentTransformationMatrix * glm::vec4(0, 0, 0, 1));
            break;
        case PARTICLE:
            particleNode->particleSystem.update(getTimeDeltaSeconds(), node->position, glm::vec3(0.0));
            break;
        default: break;
    }

    for(SceneNode* child : node->children) {
        updateNodeTransformations(child, node->currentTransformationMatrix);
    }
}

void renderNode(SceneNode* node) {
    if (node->nodeType == GEOMETRY || node->nodeType == POINT_LIGHT || node->nodeType == NORMAL_MAPPED_GEOMETRY) {
        shader->activate();

        glm::mat3 normalMat = glm::transpose(glm::inverse(node->currentTransformationMatrix));

        glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(node->currentTransformationMatrix));
        glUniformMatrix4fv(4, 1, GL_FALSE, glm::value_ptr(node->MVP));
        glUniformMatrix3fv(5, 1, GL_FALSE, glm::value_ptr(normalMat));

        if (node->nodeType == NORMAL_MAPPED_GEOMETRY) {
            glUniform1i(11, GL_TRUE);
            glUniform1i(12, GL_TRUE);
        } else {
            glUniform1i(11, GL_FALSE);
            glUniform1i(12, GL_FALSE);
        }
    }

    switch(node->nodeType) {
        case GEOMETRY:
            if(node->vertexArrayObjectID != -1) {
                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
            }
            break;
        case POINT_LIGHT:
            for(int i = 0; i < numLights; i++) {
                GLint locationLightPosition = shader->getUniformFromName(fmt::format("sources[{}].lightPosition", i));
                glUniform3fv(locationLightPosition, 1, glm::value_ptr(lightSources[i].lightPosition));

                GLint locationLightColour = shader->getUniformFromName(fmt::format("sources[{}].colour", i));
                glUniform3fv(locationLightColour, 1, glm::value_ptr(lightSources[i].colour));
            }
            break;
        case TWO_D_GEOMETRY:
            if(node->vertexArrayObjectID != -1) {
                text2DShader->activate();
                glm::mat4 ortho = glm::ortho(0.0f, float(windowWidth), 0.0f, float(windowHeight));
                glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(ortho));
                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
            }
            break;
        case NORMAL_MAPPED_GEOMETRY:
            if(node->vertexArrayObjectID != -1) {
                glBindTextureUnit(0, node->textureID);
                glBindTextureUnit(1, node->normalMappedTextureID);
                glBindVertexArray(node->vertexArrayObjectID);
            }
            break;
        case PARTICLE:
            if (node->vertexArrayObjectID != -1) {
                /*particleShader->activate();
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(node->MVP));
                for (Particle particle : node->particleSystem.particles) {
                    if (particle.lifeTime > 0.0f) {
                        // glUniform3fv(1, 1, glm::value_ptr(particle.position));
                        //
                        glBindTextureUnit(GL_TEXTURE_2D, node->textureID);
                        glBindVertexArray(node->vertexArrayObjectID);
                        glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);

                    }
                }
                glBindVertexArray(0);
                glBindTexture(GL_TEXTURE_2D, 0);*/
                particleShader->activate();
                glBindVertexArray(node->vertexArrayObjectID);
                glBindTexture(GL_TEXTURE_2D, node->textureID);
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(VP));
                for (Particle particle : node->particleSystem.particles) {
                    if (particle.lifeTime > 0.0f) {
                        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
                        glUniform3fv(1, 1, glm::value_ptr(particle.position));
                        // glUniform3fv(2, 1, glm::value_ptr(particle.color));
                    }
                }
            }
            break;
        case SKYBOX:
            if(node->vertexArrayObjectID != -1) {
                skyBoxShader->activate();
                glDepthMask(GL_FALSE);
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(VP));
                glBindTexture(GL_TEXTURE_CUBE_MAP, node->textureID);
                glBindVertexArray(node->vertexArrayObjectID);
                glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
                glDepthMask(GL_TRUE);
            }
            break;
    }
    for(SceneNode* child : node->children) {
        renderNode(child);
    }
}

void renderFrame(GLFWwindow* window) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    glViewport(0, 0, windowWidth, windowHeight);

    renderNode(rootNode);
}
