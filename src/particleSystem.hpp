#ifndef GLOWBOX_PARTICLESYSTEM_HPP
#define GLOWBOX_PARTICLESYSTEM_HPP
#include <vector>

#include <glad/glad.h>
#include <glm/glm.hpp>

struct Particle {
    Particle() {
        position = glm::vec3(0.0);
        velocity = glm::vec3(0.0);
        color    = glm::vec4(1.0);
        lifeTime = 0.0f;
    }

    glm::vec3 position;
    glm::vec3 velocity;
    glm::vec4 color;
    float     lifeTime;
};

class ParticleSystem {
public:
    ParticleSystem(unsigned int nrParticles = 0);
    void update(float deltaTime, glm::vec3 position, glm::vec3 offset);

    std::vector<Particle> particles;
    unsigned int nrParticles;
private:

    void spawnParticle(Particle &particle, glm::vec3 position, glm::vec3 offset);
};

#endif //GLOWBOX_PARTICLESYSTEM_HPP
