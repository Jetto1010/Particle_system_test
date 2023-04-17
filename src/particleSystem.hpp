#ifndef GLOWBOX_PARTICLESYSTEM_HPP
#define GLOWBOX_PARTICLESYSTEM_HPP
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Particle {
    Particle() {
        position     = glm::vec3(0.0);
        velocity     = glm::vec3(0.0);
        acceleration = glm::vec3(0.0);
        colour       = glm::vec4(0.0);
        lifeTime         = 0.0f;
        distanceToCamera = -1.0f;
    }

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec3 acceleration ;
    glm::vec4 colour;
    float lifeTime;
    float distanceToCamera;
};

class ParticleSystem {
public:
    ParticleSystem(unsigned int nrParticles = 0, unsigned int nrNewParticles = 0);
    void update(float deltaTime, glm::vec3 position, glm::vec3 cameraPosition);

    std::vector<Particle> particles;
private:
    unsigned int nrParticles;
    unsigned int nrNewParticles;
    double releaseInterval;
    double elapsedTime = 0;

    unsigned int firstUnusedParticle();
    void spawnParticle(Particle &particle, glm::vec3 position);
};

#endif //GLOWBOX_PARTICLESYSTEM_HPP
