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
SceneNode* boxNode;
SceneNode* skyNode;
SceneNode* textNode;
SceneNode* particleNode;

// These are heap allocated, because they should not be initialised at the start of the program
Gloom::Shader* shader;
Gloom::Shader* text2DShader;
Gloom::Shader* particleShader;
Gloom::Shader* skyBoxShader;

const glm::vec3 boxDimensions(180, 90, 90);

CommandLineOptions options;

struct LightSource {
    SceneNode* lightNode;
    glm::vec3 lightPosition;
    glm::vec3 colour;
};
int const numLights = 1;
LightSource lightSources[numLights];
glm::mat4 VP;
Gloom::Camera camera(glm::vec3(0, 2, -20), 15.0f);

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

    // Text
    PNGImage charmap = loadPNGFile("../res/textures/charmap.png");
    unsigned int textureID = getTextureID(&charmap);
    std::string textString = "Testing, testing, 1, 2, 3";

    // Particle
    PNGImage particleTex = loadPNGFile("../res/textures/particle.png");
    unsigned int particleTexID = getTextureID(&particleTex);

    // Wall texture
    PNGImage wallImage = loadPNGFile("../res/textures/Brick03_col.png");
    unsigned int wallImageID = getTextureID(&wallImage);
    PNGImage wallImageNM = loadPNGFile("../res/textures/Brick03_nrm.png");
    unsigned int wallImageNMID = getTextureID(&wallImageNM);

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
    Mesh box = cube(boxDimensions, glm::vec2(90), true, true);
    Mesh text = generateTextGeometryBuffer(textString, 39.0/29.0, textString.length() * 29.0);
    Mesh particleSphere = generateSphere(0.01, 10, 10);

    // Fill buffers
    unsigned int skyVAO  = generateBuffer(skybox);
    unsigned int boxVAO  = generateBuffer(box);
    unsigned int textVAO = generateBuffer(text);
    unsigned int particleVAO = generateBuffer(particleSphere);

    // Construct scene
    rootNode = createSceneNode();
    skyNode  = createSceneNode();
    boxNode  = createSceneNode();
    textNode = createSceneNode();
    particleNode = createSceneNode();

    rootNode->children.push_back(skyNode);
    rootNode->children.push_back(boxNode);
    rootNode->children.push_back(textNode);

    boxNode->vertexArrayObjectID   = boxVAO;
    boxNode->VAOIndexCount         = box.indices.size();
    boxNode->nodeType              = NORMAL_MAPPED_GEOMETRY;
    boxNode->textureID             = wallImageID;
    boxNode->normalMappedTextureID = wallImageNMID;

    skyNode->vertexArrayObjectID   = skyVAO;
    skyNode->VAOIndexCount         = skybox.indices.size();
    skyNode->nodeType              = SKYBOX;
    skyNode->position              = glm::vec3(0, 0, 0);
    skyNode->textureID             = getCubeMapID(skyboxImages);

    textNode->vertexArrayObjectID  = textVAO;
    textNode->VAOIndexCount        = text.indices.size();
    textNode->nodeType             = TWO_D_GEOMETRY;
    textNode->position             = glm::vec3(0, 0, 0);
    textNode->textureID            = textureID;

    // Lights
    for(int i = 0; i < numLights; i ++){
        lightSources[i].lightNode = createSceneNode();
        lightSources[i].lightNode->nodeType = POINT_LIGHT;
        lightSources[i].lightNode->vertexArrayObjectID = i;
    }

    lightSources[0].colour = glm::vec3(1,1,1);
    lightSources[0].lightNode->position = glm::vec3(3, 3, 3);
    boxNode->children.push_back(lightSources[0].lightNode);

    // Particles
    particleNode->nodeType = PARTICLE;
    particleNode->vertexArrayObjectID = particleVAO;
    particleNode->position = glm::vec3(0, 0, 0);
    particleNode->particleSystem = ParticleSystem(500);
    particleNode->textureID = particleTexID;
    //boxNode->children.push_back(particleNode);

    getTimeDeltaSeconds();

    std::cout << fmt::format("Initialized scene with {} SceneNodes.", totalChildren(rootNode)) << std::endl;
}

void updateFrame(GLFWwindow* window) {
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    double timeDelta = getTimeDeltaSeconds();

    camera.updateCamera(timeDelta);

    glm::mat4 projection = glm::perspective(glm::radians(80.0f), float(windowWidth) / float(windowHeight), 0.1f, 350.f);
    glm::mat4 view = camera.getViewMatrix();
    VP = projection * view;

    boxNode->position = { 0, -10, -80 };
    particleNode->particleSystem.update(timeDelta, particleNode->position, 2, glm::vec3(0.0));
    shader->activate();

    glUniform3fv(9, 1, glm::value_ptr(camera.getPos()));

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
            text2DShader->activate();
            if(node->vertexArrayObjectID != -1) {
                glm::mat4 ortho = glm::ortho(0.0f, float(windowWidth), 0.0f, float(windowHeight));
                glUniformMatrix4fv(3, 1, GL_FALSE, glm::value_ptr(ortho));
                glBindTextureUnit(0, node->textureID);
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
            particleShader->activate();
            for (Particle particle : node->particleSystem.particles) {
                if (particle.lifeTime > 0.0f) {
                    glUniformMatrix4fv(2, 1, GL_FALSE, glm::value_ptr(node->MVP));
                    glUniform3fv(3, 1, glm::value_ptr(particle.position));
                    glUniform3fv(4, 1, glm::value_ptr(particle.color));
                    glBindTextureUnit(0, node->textureID);
                    glBindVertexArray(node->vertexArrayObjectID);
                    glDrawArrays(GL_TRIANGLES, 0, 6);
                }
            }
            break;
        case SKYBOX:
            skyBoxShader->activate();
            if(node->vertexArrayObjectID != -1) {
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
