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
#include "utilities/camera.hpp"
#include "utilities/texture.h"

enum KeyFrameAction {
    BOTTOM, TOP
};

SceneNode* rootNode;
SceneNode* skyNode;
SceneNode* particleNode;
SceneNode* islandNode;

// These are heap allocated, because they should not be initialised at the start of the program
Gloom::Shader* shader;
Gloom::Shader* particleShader;
Gloom::Shader* skyBoxShader;
Gloom::Shader* objectShader;

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

    particleShader = new Gloom::Shader();
    particleShader->makeBasicShader("../res/shaders/particles.vert", "../res/shaders/particles.frag");

    skyBoxShader = new Gloom::Shader();
    skyBoxShader->makeBasicShader("../res/shaders/skybox.vert", "../res/shaders/skybox.frag");

    objectShader = new Gloom::Shader();
    objectShader->makeBasicShader("../res/shaders/object.vert", "../res/shaders/object.frag");

    // Skybox texture
    std::vector<std::string> skyboxImages{
            "../res/textures/skybox/right.png",
            "../res/textures/skybox/left.png",
            "../res/textures/skybox/top.png",
            "../res/textures/skybox/bottom.png",
            "../res/textures/skybox/front.png",
            "../res/textures/skybox/back.png"
    };

    // Island texture
    PNGImage islandTex = loadPNGFile("../res/textures/island.png");

    // Create meshes
    Mesh skybox = cube(glm::vec3(360), glm::vec2(90), true, true);
    Mesh particle = generateSphere(1.0f, 10, 10);
    // Mesh particle = cube(glm::vec3(1.0f), glm::vec2(90), false, false);
    // Mesh particle = generateParticle();
    Mesh island = Mesh("../res/objects/island.obj");

    // Fill buffers
    unsigned int skyVAO  = generateBuffer(skybox);
    unsigned int particleVAO = generateBuffer(particle);
    unsigned int islandVAO = generateBuffer(island);

    // Construct scene
    rootNode = createSceneNode();
    skyNode  = createSceneNode();
    particleNode = createSceneNode();
    islandNode = createSceneNode();

    // Skyvox
    skyNode->vertexArrayObjectID   = skyVAO;
    skyNode->VAOIndexCount         = skybox.indices.size();
    skyNode->nodeType              = SKYBOX;
    skyNode->textureID             = getCubeMapID(skyboxImages);
    rootNode->children.push_back(skyNode);

    // Particle
    particleNode->nodeType = PARTICLE;
    particleNode->vertexArrayObjectID = particleVAO;
    particleNode->particleSystem = ParticleSystem(14000, 100);
    // particleNode->textureID = particleTexID;
    particleNode->position = glm::vec3(0,0,-30);
    particleNode->VAOIndexCount = particle.indices.size();
    rootNode->children.push_back(particleNode);

    // Island
    islandNode->nodeType = GEOMETRY;
    islandNode->vertexArrayObjectID = islandVAO;
    islandNode->VAOIndexCount = island.indices.size();
    islandNode->textureID = getTextureID(&islandTex);
    islandNode->position = glm::vec3(0, -4, 0);


    getTimeDeltaSeconds();

    std::cout << fmt::format("Initialized scene with {} SceneNodes.", totalChildren(rootNode)) << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double deltaTime = getTimeDeltaSeconds();

    camera.updateCamera(deltaTime);

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 400.0f);
    glm::mat4 view = camera.getViewMatrix();
    VP = projection * view;

    updateNodeTransformations(rootNode, glm::mat4(1.0f), deltaTime);
}

void updateNodeTransformations(SceneNode* node, glm::mat4 transformationThusFar, double deltaTime) {
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
            node->particleSystem.update(deltaTime, node->position, camera.getPos());
            break;
        case SKYBOX:
            node->position = camera.getPos();
            break;
        default: break;
    }

    for(SceneNode* child : node->children) {
        updateNodeTransformations(child, node->currentTransformationMatrix, deltaTime);
    }
}

void renderNode(SceneNode* node) {
    if (node->nodeType == POINT_LIGHT || node->nodeType == NORMAL_MAPPED_GEOMETRY) {
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
            objectShader->activate();
            if(node->vertexArrayObjectID != -1) {
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(node->MVP));
                glBindTexture(GL_TEXTURE_2D, node->textureID);
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
        case NORMAL_MAPPED_GEOMETRY:
            if(node->vertexArrayObjectID != -1) {
                glBindTextureUnit(0, node->textureID);
                glBindTextureUnit(1, node->normalMappedTextureID);
                glBindVertexArray(node->vertexArrayObjectID);
            }
            break;
        case PARTICLE:
            if (node->vertexArrayObjectID != -1) {
                particleShader->activate();
                glBindVertexArray(node->vertexArrayObjectID);
                // glBindTexture(GL_TEXTURE_2D, node->textureID);
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(VP));
                for (Particle particle : node->particleSystem.particles) {
                    if (particle.lifeTime > 0.0f) {
                        glUniform3fv(1, 1, glm::value_ptr(particle.position));
                        glUniform4fv(2, 1, glm::value_ptr(particle.colour));
                        glDrawElements(GL_TRIANGLES, node->VAOIndexCount, GL_UNSIGNED_INT, nullptr);
                    }
                }
            }
            break;
        case SKYBOX:
            if(node->vertexArrayObjectID != -1) {
                skyBoxShader->activate();
                glDepthMask(GL_FALSE);
                glUniformMatrix4fv(0, 1, GL_FALSE, glm::value_ptr(VP));
                glUniform3fv(1, 1, glm::value_ptr(node->position));
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
